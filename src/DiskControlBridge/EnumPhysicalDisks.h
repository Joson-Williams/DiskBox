/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/EnumPhysicalDisks.h
/// Purpose:     To enum physical disks
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/15
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <setupapi.h>

typedef struct {
    HANDLE hDevice;
    DWORD dwPropertySize; // 存储查询到的属性结构体的实际大小
    union {
        STORAGE_PROPERTY_QUERY query;         // 查询参数
        STORAGE_DEVICE_DESCRIPTOR descriptor; // 设备描述符 (基本属性)
    } u;

    PVOID pDescriptor;
    char devicePath[MAX_PATH]; // 易于读取的设备路径
} PhysicalDiskInfo;

int EnumeratePhysicalDisks(PhysicalDiskInfo** ppDiskArray, PDWORD pdwDiskCount);
