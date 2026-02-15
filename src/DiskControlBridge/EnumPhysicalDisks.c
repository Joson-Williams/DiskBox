/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/EnumPhysicalDisks.c
/// Purpose:     To enum physical disks
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/15
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include "EnumPhysicalDisks.h"
#include "DCB_ErrorDefinitions.h"

#include <windows.h>
#include <setupapi.h>
#include <stdlib.h>

// 定义内存分配上限，避免分配过大内存
#define MAX_BUFFER_SIZE (1024 * 1024)  // 1MB上限

/**
 * @brief 枚举系统中的所有物理磁盘，并填充相关信息。
 *
 * 此函数通过 Windows SetupAPI 枚举所有连接的物理磁盘 (GUID_DEVINTERFACE_DISK)。
 * 它会为每个找到的磁盘打开一个句柄，查询其总大小，并将结果存储在一个
 * 动态分配的 `PhysicalDiskInfo` 结构体数组中。
 *
 * @param[out] ppDiskArray 指向 `PhysicalDiskInfo` 结构体数组指针的指针。
 *                     成功时，此函数会将 `*ppDisks` 设置为指向新分配的数组。
 *                     调用者在使用完毕后必须通过 `free()` 释放此数组。
 * @param[out] pdwDiskCount 指向一个 `DWORD` 类型变量的指针。
 *                         成功时，此函数会将 `*pdwDiskCount` 设置为实际找到并成功处理的磁盘数量。
 *                         这个数量可能小于系统中存在的总磁盘数，例如因权限不足无法访问某些磁盘。
 *
 * @return 返回一个 `EnumPhysicalDisksResult` 枚举值，指示操作是否成功。
 * @note
 * - 调用者有责任管理返回的数组内存，使用 `free(*ppDisks)` 进行释放。
 * - 调用者有责任管理返回的 `PhysicalDiskInfo` 结构中的 `hDevice` 句柄，
 *   在不再需要时，必须调用 `CloseHandle()` 显式关闭它以避免资源泄露。
 * - 如果某个磁盘的属性获取失败，该磁盘对应的 `PhysicalDiskInfo.pDescriptor` 字段将为 `NULL`。
 *   `pdwDiskCount` 反映的是成功处理（即成功打开并至少尝试获取属性）的磁盘数量。
 */
int EnumeratePhysicalDisks(PhysicalDiskInfo** ppDiskArray, PDWORD pdwDiskCount) {
    // 入参合法性检查（关键！原代码未检查空指针）
    if (ppDiskArray == NULL || pdwDiskCount == NULL) {
        return ENUM_PHYSICAL_DISKS_ERROR_INVALID_PARAMETER;
    }

    // 初始化输出参数，避免野指针
    *ppDiskArray = NULL;
    *pdwDiskCount = 0;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA devInterfaceData = {.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA)};
    PSP_DEVICE_INTERFACE_DETAIL_DATA_W pDevDetailDataW = NULL;
    DWORD dwIndex = 0, dwValidIndex = 0;
    DWORD dwRequiredSize = 0;
    DWORD dwError = ERROR_SUCCESS;
    DWORD dwDiskCount = 0;
    PhysicalDiskInfo* pDisks = NULL;
    int RetVal = ENUM_PHYSICAL_DISKS_SUCCESS;

    // 获取设备信息集：前置失败处理，减少缩进
    hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_DISK, NULL, NULL,
                                    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        RetVal = ENUM_PHYSICAL_DISKS_ERROR_GET_DISK_COUNT;
        goto cleanup;  // 统一通过cleanup处理，逻辑更一致
    }

    // 第一次遍历计数：严格检查错误，避免无效计数
    while (TRUE) {
        BOOL bResult = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_DISK,
                                                   dwIndex, &devInterfaceData);
        // 前置失败处理
        if (!bResult) {
            dwError = GetLastError();
            if (dwError == ERROR_NO_MORE_ITEMS) {
                break;  // 正常结束遍历
            } else {
                RetVal = ENUM_PHYSICAL_DISKS_ERROR_GET_DISK_COUNT;
                goto cleanup;
            }
        }
        dwIndex++;
    }

    dwDiskCount = dwIndex;
    // 前置无设备处理
    if (dwDiskCount == 0) {
        RetVal = ENUM_PHYSICAL_DISKS_ERROR_NO_DEVICES_FOUND;
        goto cleanup;
    }

    // 分配内存（初始化为0）：前置分配失败处理
    pDisks = (PhysicalDiskInfo*)calloc(dwDiskCount, sizeof(PhysicalDiskInfo));
    if (!pDisks) {
        RetVal = ENUM_PHYSICAL_DISKS_ERROR_MEMORY_ALLOCATION;
        goto cleanup;
    }

    // 第二次遍历获取详细信息：修复循环条件，避免无限循环
    dwIndex = 0;
    dwValidIndex = 0;
    while (TRUE) {
        // 先检查是否遍历完所有设备
        BOOL bEnumResult = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_DISK,
                                                      dwIndex, &devInterfaceData);
        // 前置枚举失败处理
        if (!bEnumResult) {
            dwError = GetLastError();
            if (dwError == ERROR_NO_MORE_ITEMS) {
                break;  // 正常结束遍历
            } else {
                dwIndex++;
                continue;
            }
        }

        // 获取所需缓冲区大小
        BOOL bDetailResult = SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInterfaceData, NULL, 0,
                                                             &dwRequiredSize, NULL);
        dwError = GetLastError();
        // 前置缓冲区大小无效处理
        if (bDetailResult || dwError != ERROR_INSUFFICIENT_BUFFER || dwRequiredSize == 0 || dwRequiredSize > MAX_BUFFER_SIZE) {
            dwIndex++;
            continue;  // 缓冲区大小无效，跳过该设备
        }

        // 分配缓冲区：前置分配失败处理
        pDevDetailDataW = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)calloc(dwRequiredSize, 1);
        if (!pDevDetailDataW) {
            RetVal = ENUM_PHYSICAL_DISKS_ERROR_MEMORY_ALLOCATION;
            goto cleanup;
        }
        pDevDetailDataW->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        // 获取设备详情：前置获取失败处理
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInterfaceData,
                                              pDevDetailDataW, dwRequiredSize,
                                              NULL, NULL)) {
            free(pDevDetailDataW);
            pDevDetailDataW = NULL;
            dwIndex++;
            continue;
        }

        // 打开设备
        HANDLE hDevice = CreateFileW(pDevDetailDataW->DevicePath,
                                     GENERIC_READ,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,
                                     NULL);

        // 安全转换设备路径
        size_t convertedChars = 0;
        errno_t err = wcstombs_s(&convertedChars,
                                  pDisks[dwValidIndex].devicePath,
                                  sizeof(pDisks[dwValidIndex].devicePath),  // 使用结构体实际长度，更安全
                                  pDevDetailDataW->DevicePath,
                                  _TRUNCATE);
        if (err != 0) {
            pDisks[dwValidIndex].devicePath[0] = '\0';
        }

        // 初始化设备属性默认值
        pDisks[dwValidIndex].hDevice = INVALID_HANDLE_VALUE;
        pDisks[dwValidIndex].pDescriptor = NULL;
        pDisks[dwValidIndex].dwPropertySize = 0;

        // 设备句柄有效时才处理属性
        if (hDevice != INVALID_HANDLE_VALUE) {
            pDisks[dwValidIndex].hDevice = hDevice;

            // 查询设备属性
            STORAGE_PROPERTY_QUERY query = {
                .PropertyId = StorageDeviceProperty,
                .QueryType = PropertyStandardQuery
            };

            DWORD dwBytesReturned = 0;
            // 第一次调用获取大小
            BOOL bIoResult = DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                                           &query, sizeof(query),
                                           NULL, 0,
                                           &dwBytesReturned, NULL);
            dwError = GetLastError();

            // 前置大小获取失败/无效处理
            if (bIoResult || dwError != ERROR_INSUFFICIENT_BUFFER || dwBytesReturned == 0 || dwBytesReturned > MAX_BUFFER_SIZE) {
                free(pDevDetailDataW);
                pDevDetailDataW = NULL;
                dwIndex++;
                dwValidIndex++;
                if (dwValidIndex >= dwDiskCount) break;
                continue;
            }

            // 分配属性缓冲区
            pDisks[dwValidIndex].pDescriptor = malloc(dwBytesReturned);
            // 前置缓冲区分配失败处理
            if (pDisks[dwValidIndex].pDescriptor == NULL) {
                free(pDevDetailDataW);
                pDevDetailDataW = NULL;
                dwIndex++;
                dwValidIndex++;
                if (dwValidIndex >= dwDiskCount) break;
                continue;
            }

            // 第二次调用获取属性
            BOOL bPropResult = DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                                             &query, sizeof(query),
                                             pDisks[dwValidIndex].pDescriptor,
                                             dwBytesReturned,
                                             &dwBytesReturned, NULL);
            // 前置属性获取失败处理
            if (!bPropResult) {
                free(pDisks[dwValidIndex].pDescriptor);
                pDisks[dwValidIndex].pDescriptor = NULL;
                pDisks[dwValidIndex].dwPropertySize = 0;
            } else {
                pDisks[dwValidIndex].dwPropertySize = dwBytesReturned;
            }
        }

        // 释放临时缓冲区
        free(pDevDetailDataW);
        pDevDetailDataW = NULL;

        dwIndex++;
        dwValidIndex++;

        // 防止dwValidIndex超出磁盘总数（边界保护）
        if (dwValidIndex >= dwDiskCount) {
            break;
        }
    }

    // 更新输出参数：确保只有成功时才赋值
    *pdwDiskCount = dwValidIndex;
    *ppDiskArray = pDisks;
    RetVal = ENUM_PHYSICAL_DISKS_SUCCESS;

cleanup:
    // 释放临时缓冲区（防止泄漏）
    if (pDevDetailDataW != NULL) {
        free(pDevDetailDataW);
    }

    // 销毁设备信息集
    if (hDevInfo != INVALID_HANDLE_VALUE) {
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    // 错误时清理所有已分配资源：确保句柄和内存都释放
    if (RetVal != ENUM_PHYSICAL_DISKS_SUCCESS) {
        if (pDisks != NULL) {
            for (DWORD i = 0; i < dwValidIndex; i++) {
                // 关闭设备句柄
                if (pDisks[i].hDevice != INVALID_HANDLE_VALUE) {
                    CloseHandle(pDisks[i].hDevice);
                    pDisks[i].hDevice = INVALID_HANDLE_VALUE;
                }
                // 释放属性缓冲区
                if (pDisks[i].pDescriptor != NULL) {
                    free(pDisks[i].pDescriptor);
                    pDisks[i].pDescriptor = NULL;
                }
            }
            free(pDisks);
            pDisks = NULL;
        }
        // 重置输出参数
        *ppDiskArray = NULL;
        *pdwDiskCount = 0;
    }

    return RetVal;
}