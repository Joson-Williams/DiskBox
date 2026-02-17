/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/DCB_ErrorDefinitions.h
/// Purpose:     (error definitions for DiskControlBridge)
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/14
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once

typedef enum {
    READ_DISK_SECTOR_SUCCESS                        = 0x00000000,  // 成功
    READ_DISK_SECTOR_ERROR_INVALID_PARAMETER        = 0x00000001,  // 不合法的参数
    READ_DISK_SECTOR_ERROR_DISK_OPEN_FAILED         = 0x00000002,  // 打开磁盘失败
    READ_DISK_SECTOR_ERROR_DISK_GET_GEOMETRY_FAILED = 0x00000003,  // 获取磁盘几何信息失败
    READ_DISK_SECTOR_ERROR_SEEK_FAILED              = 0x00000004,  // 无法定位到指定位置
    READ_DISK_SECTOR_ERROR_READ_FAILED              = 0x00000005,  // 无法读取
    READ_DISK_SECTOR_ERROR_INSUFFICIENT_BUFFER      = 0x00000006,  // 缓冲区不够
    READ_DISK_SECTOR_ERROR_SECTOR_SIZE_TOO_SMALL    = 0x00000007,  // 扇区字节数太小
    READ_DISK_SECTOR_ERROR_SECTOR_SIZE_TOO_BIG      = 0x00000008,  // 扇区字节数太大
    READ_DISK_SECTOR_ERROR_BUFFER_ASSIGNING_FAILED  = 0x00000009,  // 缓冲区分配失败
    READ_DISK_SECTOR_ERROR_MULTIPLICATION_OVERFLOW  = 0x0000000A,  // 乘法溢出
    READ_DISK_SECTOR_ERROR_UNKNOWN                  = 0x0000000B,  // 未知
} ReadDiskSectorResult;

typedef enum {
    ENUM_PHYSICAL_DISKS_SUCCESS                     = 0x00000000,  // 成功
    ENUM_PHYSICAL_DISKS_ERROR_GET_DISK_COUNT        = 0x00000001,  // 在尝试获取系统中物理磁盘的总数量时失败
    ENUM_PHYSICAL_DISKS_ERROR_MEMORY_ALLOCATION     = 0x00000002,  // 在程序尝试为存储设备信息的数组或临时缓冲区分配内存时失败
    ENUM_PHYSICAL_DISKS_ERROR_INVALID_DEVICE_HANDLE = 0x00000003,  // 在尝试打开某个物理磁盘设备以获取其句柄时失败
    ENUM_PHYSICAL_DISKS_ERROR_IOCTL_FAILED          = 0x00000004,  // 在使用 DeviceIoControl 函数向设备发送 IOCTL_STORAGE_QUERY_PROPERTY 请求以查询设备属性时失败
    ENUM_PHYSICAL_DISKS_ERROR_FINAL_PATH            = 0x00000005,  // 在将从 Windows API 获取的宽字符（Unicode）设备路径转换为 ANSI/MultiByte 字符串时失败
    ENUM_PHYSICAL_DISKS_ERROR_NO_DEVICES_FOUND      = 0x00000006,  // 程序成功执行了枚举操作，但在系统上没有发现任何符合物理磁盘接口的设备
    ENUM_PHYSICAL_DISKS_ERROR_QUERY_SIZE            = 0x00000007,  // 在查询设备属性大小或执行其他需要返回数据大小的操作时失败
    ENUM_PHYSICAL_DISKS_ERROR_INVALID_PARAMETER     = 0x00000008,  // 参数不合法
} EnumPhysicalDisksResult;