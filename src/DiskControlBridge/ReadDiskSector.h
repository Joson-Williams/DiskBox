/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/ReadDiskSector.h
/// Purpose:     To read sectors on hard disk
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/14
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <stdint.h>


/**
 * @brief 从原始磁盘设备读取单个逻辑扇区。
 *
 * 此函数打开一个指向物理磁盘（例如 \\\\.\\PhysicalDrive0）的句柄，
 * 查询其几何结构以确定扇区大小，分配一个正确大小的临时缓冲区，
 * 定位到指定的扇区索引，并读取数据。
 * 读取的数据和扇区大小随后会被复制到用户提供的缓冲区中。
 *
 * 注意：此函数访问物理磁盘需要管理员权限。
 *
 * @param[in] diskPath 磁盘设备的路径（例如 "\\\\.\\PhysicalDrive0"）。
 * @param[in] sectorIndex 要读取的扇区的零基索引。
 * @param[out] pSectorSize 指向一个变量的指针，用于存储扇区大小。
 * @param[out] pSectorBuffer 指向一个缓冲区的指针，用于复制扇区数据。
 *                           调用者必须确保此缓冲区的大小至少为 *pSectorSize 字节。
 * @param[in] bufferSize pSectorBuffer 指向的缓冲区的大小。
 * @return 成功时返回 READ_DISK_SECTOR_SUCCESS，否则返回定义的错误代码之一。
 */
unsigned int ReadDiskSector(const char* diskPath,
                   unsigned long long sectorIndex,
                   unsigned int* pSectorSize,
                   uint8_t* pSectorBuffer,
                   size_t bufferSize);

int GetDiskSectorSize(const char* diskPath, unsigned int* pSectorSize);