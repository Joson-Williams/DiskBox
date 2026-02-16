/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/DCB_ErrorDefinitions.h
/// Purpose:     (error definitions for DiskControlBridge)
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/14
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define READ_DISK_SECTOR_SUCCESS                        0xE0000000  // 成功
#define READ_DISK_SECTOR_ERROR_INVALID_PARAMETER        0xEA000001  // 不合法的参数
#define READ_DISK_SECTOR_ERROR_DISK_OPEN_FAILED         0xEA000002  // 打开磁盘失败
#define READ_DISK_SECTOR_ERROR_DISK_GET_GEOMETRY_FAILED 0xEA000003  // 获取磁盘几何信息失败
#define READ_DISK_SECTOR_ERROR_SEEK_FAILED              0xEA000004  // 无法定位到指定位置
#define READ_DISK_SECTOR_ERROR_READ_FAILED              0xEA000005  // 无法读取
#define READ_DISK_SECTOR_ERROR_INSUFFICIENT_BUFFER      0xEA000006  // 缓冲区不够
#define READ_DISK_SECTOR_ERROR_SECTOR_SIZE_TOO_SMALL    0xEA000007  // 扇区字节数太小
#define READ_DISK_SECTOR_ERROR_SECTOR_SIZE_TOO_BIG      0xEA000008  // 扇区字节数太大
#define READ_DISK_SECTOR_ERROR_BUFFER_ASSIGNING_FAILED  0xEA000009  // 缓冲区分配失败
#define READ_DISK_SECTOR_ERROR_MULTIPLICATION_OVERFLOW  0xEA00000A  // 乘法溢出
#define ENUM_PHYSICAL_DISKS_SUCCESS                     0xE0000000  // 成功
#define ENUM_PHYSICAL_DISKS_ERROR_GET_DISK_COUNT        0xEB000001  // 在尝试获取系统中物理磁盘的总数量时失败
#define ENUM_PHYSICAL_DISKS_ERROR_MEMORY_ALLOCATION     0xEB000002  // 在程序尝试为存储设备信息的数组或临时缓冲区分配内存时失败。
#define ENUM_PHYSICAL_DISKS_ERROR_INVALID_DEVICE_HANDLE 0xEB000003  // 在尝试打开某个物理磁盘设备以获取其句柄时失败。
#define ENUM_PHYSICAL_DISKS_ERROR_IOCTL_FAILED          0xEB000004  // 在使用 DeviceIoControl 函数向设备发送 IOCTL_STORAGE_QUERY_PROPERTY 请求以查询设备属性时失败。
#define ENUM_PHYSICAL_DISKS_ERROR_FINAL_PATH            0xEB000005  // 在将从 Windows API 获取的宽字符（Unicode）设备路径转换为 ANSI/MultiByte 字符串以便存储到 PhysicalDiskInfo 结构体中时失败。
#define ENUM_PHYSICAL_DISKS_ERROR_NO_DEVICES_FOUND      0xEB000006  // 程序成功执行了枚举操作，但在系统上没有发现任何符合物理磁盘接口（GUID_DEVINTERFACE_DISK）的设备。
#define ENUM_PHYSICAL_DISKS_ERROR_QUERY_SIZE            0xEB000007  // 在查询设备属性大小或执行其他需要返回数据大小的操作时失败。
#define ENUM_PHYSICAL_DISKS_ERROR_INVALID_PARAMETER     0xEB000008  // 参数不合法