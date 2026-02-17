/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/GetTableType.h
/// Purpose:     //TODO
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/17
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstdint>

enum class PartitionTableType : std::uint8_t;

/**
 * @brief 分析磁盘的第一个扇区以确定分区表类型
 * @param diskPath 要分析的磁盘路径 (例如 "\\\\.\\PhysicalDrive0")
 * @return PartitionTableType 枚举值表示磁盘分区表类型或错误状态
 */
PartitionTableType AnalyzeDiskPartitionTable(const char* diskPath);