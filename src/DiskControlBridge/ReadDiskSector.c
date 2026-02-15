/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/ReadDiskSector.h
/// Purpose:     To read sectors on hard disk
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/13
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <winioctl.h> // 用于 IOCTL_DISK_GET_DRIVE_GEOMETRY_EX
#include <stdio.h>
#include <stdlib.h> // 用于 malloc, free
#include <stdint.h>
#include <string.h>
#include "ReadDiskSector.h"
#include "DCB_ErrorDefinitions.h"

int ReadDiskSector(const char* diskPath,
                   const long long sectorIndex,
                   unsigned int* pSectorSize,
                   uint8_t* pSectorBuffer,
                   const size_t bufferSize) {
    HANDLE hDisk  = INVALID_HANDLE_VALUE;
    int errorCode = READ_DISK_SECTOR_SUCCESS; // 初始假设成功

    // 验证输入参数
    if (!diskPath || !pSectorSize || !pSectorBuffer || sectorIndex < 0) {
        return READ_DISK_SECTOR_ERROR_INVALID_PARAMETER;
    }

    // 打开磁盘句柄
    hDisk = CreateFileA(
            diskPath,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE, // 共享模式，允许读写共享
            NULL,                               // 默认安全属性
            OPEN_EXISTING,
            0,   // 原始磁盘访问不需要特殊标志
            NULL // 无模板文件
            );

    if (hDisk == INVALID_HANDLE_VALUE) {
        errorCode = READ_DISK_SECTOR_ERROR_DISK_OPEN_FAILED;
        goto cleanup_and_return; // 使用 goto 在失败时进行集中清理
    }

    // 获取磁盘几何结构以确定扇区大小
    DISK_GEOMETRY_EX diskGeometry;
    DWORD bytesReturned    = 0;
    const BOOL ioctlResult = DeviceIoControl(
            hDisk,
            IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,    // 使用扩展几何结构以获得准确的大小
            NULL, 0,                             // 输入缓冲区和大小（此操作不需要）
            &diskGeometry, sizeof(diskGeometry), // 输出缓冲区和大小
            &bytesReturned, NULL                 // 返回的字节数和重叠结构
            );

    if (!ioctlResult) {
        errorCode = READ_DISK_SECTOR_ERROR_DISK_GET_GEOMETRY_FAILED;
        goto cleanup_and_return;
    }

    // 从几何结构中计算实际扇区大小
    const unsigned int actualSectorSize = (unsigned int)diskGeometry.Geometry.BytesPerSector;

    // 针对一个更大的、合理的范围验证检索到的扇区大小。
    // 现代驱动器通常有 512、4096 或新兴的更大物理扇区。
    // 设置一个像 1MB (1048576 字节) 这样极大的上限是非常慷慨且安全的。
    const unsigned int MIN_SECTOR_SIZE           = 512;
    const unsigned int MAX_PLAUSIBLE_SECTOR_SIZE = 1024 * 1024; // 1 MB

    // 检查扇区大小是否受支持
    if (actualSectorSize < MIN_SECTOR_SIZE) {
        errorCode = READ_DISK_SECTOR_ERROR_SECTOR_SIZE_TOO_SMALL;
        goto cleanup_and_return;
    }

    if (actualSectorSize > MAX_PLAUSIBLE_SECTOR_SIZE) {
        errorCode = READ_DISK_SECTOR_ERROR_SECTOR_SIZE_TOO_BIG;
        goto cleanup_and_return;
    }

    // 检查用户提供的缓冲区是否足够大
    if (bufferSize < actualSectorSize) {
        errorCode    = READ_DISK_SECTOR_ERROR_INSUFFICIENT_BUFFER;
        *pSectorSize = actualSectorSize; // 通知调用者所需的大小
        goto cleanup_and_return;
    }

    // 计算目标扇区的绝对字节偏移量
    LARGE_INTEGER liDistanceToMove = {0};
    // 在乘法之前检查潜在的溢出
    if (sectorIndex > LLONG_MAX / actualSectorSize) {
        errorCode = READ_DISK_SECTOR_ERROR_MULTIPLICATION_OVERFLOW;
        goto cleanup_and_return;
    }
    liDistanceToMove.QuadPart = sectorIndex * actualSectorSize;

    // 定位到计算出的偏移量
    // 注意：SetFilePointerEx 相较于 SetFilePointer 更适合 64 位偏移量
    LARGE_INTEGER liNewFilePointer = {0};
    if (!SetFilePointerEx(hDisk, liDistanceToMove, &liNewFilePointer, FILE_BEGIN)) {
        errorCode = READ_DISK_SECTOR_ERROR_SEEK_FAILED;
        goto cleanup_and_return;
    }

    // 分配一个与扇区大小完全相同的临时缓冲区
    uint8_t* tempBuffer = malloc(actualSectorSize);
    if (!tempBuffer) {
        // 分配失败
        errorCode = READ_DISK_SECTOR_ERROR_MULTIPLICATION_OVERFLOW;
        goto cleanup_and_return;
    }

    // 将扇区数据读入临时缓冲区
    DWORD bytesRead = 0;
    const BOOL readResult = ReadFile(hDisk, tempBuffer, actualSectorSize, &bytesRead, NULL);

    if (!readResult || bytesRead != actualSectorSize) {
        // 读取失败或未读取预期的字节数
        free(tempBuffer); // 在返回错误前释放已分配的缓冲区
        errorCode = READ_DISK_SECTOR_ERROR_READ_FAILED;
        goto cleanup_and_return;
    }

    // 将成功读取的数据从临时缓冲区复制到用户的缓冲区
    memcpy(pSectorBuffer, tempBuffer, actualSectorSize);
    free(tempBuffer); // 成功复制后释放临时缓冲区

    // 设置扇区大小的输出参数
    *pSectorSize = actualSectorSize;

cleanup_and_return:
    if (hDisk != INVALID_HANDLE_VALUE) {
        CloseHandle(hDisk); // 始终关闭句柄
    }
    return errorCode; // 返回之前确定的错误代码
}

// 辅助函数，用于获取扇区大小
int GetDiskSectorSize(const char* diskPath, unsigned int* pSectorSize) {
    HANDLE hDisk  = INVALID_HANDLE_VALUE;
    int errorCode = READ_DISK_SECTOR_SUCCESS;

    if (!diskPath || !pSectorSize) {
        return READ_DISK_SECTOR_ERROR_INVALID_PARAMETER;
    }

    hDisk = CreateFileA(
            diskPath,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
            );

    if (hDisk == INVALID_HANDLE_VALUE) {
        return READ_DISK_SECTOR_ERROR_DISK_OPEN_FAILED;
    }

    DISK_GEOMETRY_EX diskGeometry;
    DWORD bytesReturned = 0;
    const BOOL ioctlResult    = DeviceIoControl(
            hDisk,
            IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
            NULL, 0,
            &diskGeometry, sizeof(diskGeometry),
            &bytesReturned, NULL
            );

    if (!ioctlResult) {
        errorCode = READ_DISK_SECTOR_ERROR_DISK_GET_GEOMETRY_FAILED;
        goto cleanup_and_return_getsize;
    }

    const unsigned int actualSectorSize = (unsigned int)diskGeometry.Geometry.BytesPerSector;

    const unsigned int MIN_SECTOR_SIZE           = 512;
    const unsigned int MAX_PLAUSIBLE_SECTOR_SIZE = 1024 * 1024; // 1 MB

    if (actualSectorSize < MIN_SECTOR_SIZE || actualSectorSize > MAX_PLAUSIBLE_SECTOR_SIZE) {
        errorCode = READ_DISK_SECTOR_ERROR_INVALID_PARAMETER;
        goto cleanup_and_return_getsize;
    }

    *pSectorSize = actualSectorSize;

cleanup_and_return_getsize:
    if (hDisk != INVALID_HANDLE_VALUE) {
        CloseHandle(hDisk);
    }
    return errorCode;
}
