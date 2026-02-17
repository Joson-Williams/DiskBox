/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/GetTableType.cpp
/// Purpose:     To determine the partition table format.
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/17
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include "ReadDiskSector.h"
#include "ErrorDefinitions.h"
#include <cstdint>         // for std::uint8_t, std::uint32_t, etc.
#include <cstring>         // for std::memcmp
#include <memory>          // for std::make_unique

// 定义返回结果的枚举
enum class PartitionTableType : std::uint8_t {
    GPT                       = 0x00,
    MBR                       = 0x01,
    NO_PARTITION_TABLE        = 0x02,
    ERROR_BASE                = 0x10, // 错误代码的基础值，错误码高位均为1
    ERROR_GETTING_SECTOR_SIZE = 0x11,
    ERROR_READING_LBA0        = 0x12,
    ERROR_READING_LBA1        = 0x13,
    ERROR_MEMORY_ALLOCATION   = 0x14,
    ERROR_UNKNOWN             = 0x1F
};

PartitionTableType AnalyzeDiskPartitionTable(const char* diskPath) {

    // 获取扇区大小
    unsigned int sectorSize = 0;
    int readResult          = GetDiskSectorSize(diskPath, &sectorSize);
    if (readResult != READ_DISK_SECTOR_SUCCESS || sectorSize == 0) {
        return PartitionTableType::ERROR_GETTING_SECTOR_SIZE;
    }

    // 读取 LBA 0
    auto buffer = std::make_unique<std::uint8_t[]>(sectorSize);
    if (!buffer) {
        return PartitionTableType::ERROR_MEMORY_ALLOCATION;
    }

    readResult = ReadDiskSector(diskPath, 0, &sectorSize, buffer.get(), sectorSize);
    if (readResult != READ_DISK_SECTOR_SUCCESS) {
        return PartitionTableType::ERROR_READING_LBA0;
    }

    // 检查 LBA 0 是否为 MBR
    bool isMBR = false;
    if (sectorSize >= 512) {
        constexpr std::uint16_t mbrSignature = 0xAA55;
        const std::uint16_t lba0_sig         = *reinterpret_cast<const std::uint16_t*>(buffer.get() + 510);
        isMBR                                = (lba0_sig == mbrSignature);
    }

    // 如果 LBA 0 是 MBR，进一步检查 LBA 1 以区分 MBR/GPT
    if (isMBR) {
        // 重新填充 buffer 用于 LBA 1
        readResult = ReadDiskSector(diskPath, 1, &sectorSize, buffer.get(), sectorSize);
        if (readResult != READ_DISK_SECTOR_SUCCESS) {
            return PartitionTableType::ERROR_READING_LBA1;
        }

        // GPT 头部的魔数 "EFI PART"
        const std::uint8_t gpt_signature[] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54}; // "EFI PART"
        if (sectorSize >= 8 && std::memcmp(buffer.get(), gpt_signature, 8) == 0) {
            return PartitionTableType::GPT; // LBA 1 是 GPT 头，确认为 GPT
        }
        return PartitionTableType::MBR; // LBA 0 是 MBR，LBA 1 不是 GPT，确认为 MBR
    }

    // 如果 LBA 0 不是 MBR，检查它是否可能是 GPT
    // 或者判定为无分区表
    const std::uint8_t gpt_signature_start[] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};
    if (sectorSize >= 8 && std::memcmp(buffer.get(), gpt_signature_start, 8) == 0) {

        readResult = ReadDiskSector(diskPath, 1, &sectorSize, buffer.get(), sectorSize);
        if (readResult != READ_DISK_SECTOR_SUCCESS) {
            return PartitionTableType::ERROR_READING_LBA1;
        }

        if (std::memcmp(buffer.get(), gpt_signature_start, 8) == 0) {
            return PartitionTableType::GPT;
        }

        // LBA 0 是 GPT，LBA 1 不是，这是一个无效/损坏的结构
        return PartitionTableType::NO_PARTITION_TABLE;
    }

    // LBA 0 既不是 MBR 签名也不是 GPT 签名
    return PartitionTableType::NO_PARTITION_TABLE;
}
