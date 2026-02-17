/////////////////////////////////////////////////////////////////////////////
/// Name:        src/DiskControlBridge/ParseGPTTable.cpp
/// Purpose:     Reads and parses the GPT partition table from PhysicalDrive0
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/16
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <format>
#include <string>
#include <optional>
#include <algorithm> // For std::find_if, std::all_of

// 引入 C 语言编写的磁盘读取函数和错误定义
extern "C" {
    #include "ReadDiskSector.h"
    #include "ErrorDefinitions.h"
}

struct PartitionInfo {
    std::string type_guid_str;    // 分区类型的 GUID 字符串 (例如 Microsoft Basic Data, EFI System Partition)
    std::string unique_guid_str;  // 分区的唯一 GUID 字符串 (每个分区都有唯一的 ID)
    uint64_t first_lba;           // 分区起始的逻辑块地址 (LBA)
    uint64_t last_lba;            // 分区结束的逻辑块地址 (LBA)
    uint64_t attributes;          // 分区属性标志 (Bitmask)
    std::string name;             // 分区名称 (由用户在创建时指定，存储在 GPT 表中)
    std::string filesystem_label; // 文件系统卷标或占位符 (GPT 中没有真正的文件系统类型，这里用分区名或预留字段)
};

struct DiskInfo {
    bool has_error = false;                       // 指示解析过程中是否发生错误
    int error_code = 0;                           // 如果 has_error 为 true，则此字段包含具体的错误代码
    std::string disk_guid_str;                    // 磁盘的全局唯一 GUID 字符串
    uint64_t gpt_header_lba                  = 0; // GPT 主头部所在的 LBA 地址 (通常为 1)
    uint64_t backup_gpt_header_lba           = 0; // GPT 备份头部所在的 LBA 地址 (通常在磁盘末尾)
    uint64_t first_usable_lba                = 0; // 第一个可用作分区的 LBA 地址
    uint64_t last_usable_lba                 = 0; // 最后一个可用作分区的 LBA 地址
    uint32_t number_of_partition_entries     = 0; // GPT 分区条目数组中总条目的数量
    uint32_t size_of_partition_entry         = 0; // 单个分区条目所占用的字节数
    uint64_t partition_entry_array_start_lba = 0; // GPT 分区条目数组起始的 LBA 地址
    int partition_count                      = 0; // 计算出的非空分区数量
    std::vector<PartitionInfo> partitions;        // 包含所有有效分区信息的向量
};

// 将 GUID 数组转换为字符串格式
std::string guidToString(const uint8_t* guid) {
    return std::format(
            "{:02X}{:02X}{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
            guid[3], guid[2], guid[1], guid[0],
            guid[5], guid[4],
            guid[7], guid[6],
            guid[8], guid[9],
            guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);
}


DiskInfo ParseGPTDisk(const char* diskPath) {
    DiskInfo disk_info{};
    unsigned int sectorSize = 0;
    int result              = 0;

    // 获取扇区大小
    result = GetDiskSectorSize(diskPath, &sectorSize);
    if (result != READ_DISK_SECTOR_SUCCESS) {
        disk_info.has_error  = true;
        disk_info.error_code = result;
        return disk_info;
    }

    // 读取 MBR 扇区 (LBA 0)
    std::vector<uint8_t> mbrBuffer(sectorSize);
    result = ReadDiskSector(diskPath, 0, &sectorSize, mbrBuffer.data(), mbrBuffer.size());

    // MBR 中的分区类型 0xEE 表示 GPT 保护性 MBR
    if (const uint8_t gpt_test_type = mbrBuffer[0x1BE + 4]; gpt_test_type != 0xEE) {
        disk_info.has_error  = true;
        disk_info.error_code = READ_DISK_SECTOR_ERROR_UNKNOWN;
        return disk_info;
    }

    // 3. 读取 GPT 头部 (LBA 1)
    std::vector<uint8_t> gptHeaderBuffer(sectorSize);
    result = ReadDiskSector(diskPath, 1, &sectorSize, gptHeaderBuffer.data(), gptHeaderBuffer.size());
    if (result != READ_DISK_SECTOR_SUCCESS) {
        disk_info.has_error  = true;
        disk_info.error_code = result;
        return disk_info;
    }

    // 4. 解析 GPT 头部信息
    struct GPTHeader {
        char signature[8];
        uint32_t revision;
        uint32_t header_size;
        uint32_t crc32_header;
        uint32_t reserved_zero;
        uint64_t current_lba;
        uint64_t backup_lba;
        uint64_t first_usable_lba;
        uint64_t last_usable_lba;
        uint8_t disk_guid[16];
        uint64_t partition_entry_lba;
        uint32_t num_partition_entries;
        uint32_t size_of_partition_entry;
        uint32_t crc32_partition_array;
        uint8_t reserved[420];
    };

    const auto* gptHeader = reinterpret_cast<const GPTHeader*>(gptHeaderBuffer.data());

    // 验证 GPT 签名
    constexpr char expected_signature[] = "EFI PART";
    if (memcmp(gptHeader->signature, expected_signature, 8) != 0) {
        disk_info.has_error  = true;
        disk_info.error_code = READ_DISK_SECTOR_ERROR_UNKNOWN;
        return disk_info;
    }

    disk_info.disk_guid_str                   = guidToString(gptHeader->disk_guid);
    disk_info.gpt_header_lba                  = gptHeader->current_lba;
    disk_info.backup_gpt_header_lba           = gptHeader->backup_lba;
    disk_info.first_usable_lba                = gptHeader->first_usable_lba;
    disk_info.last_usable_lba                 = gptHeader->last_usable_lba;
    disk_info.partition_entry_array_start_lba = gptHeader->partition_entry_lba;
    disk_info.number_of_partition_entries     = gptHeader->num_partition_entries;
    disk_info.size_of_partition_entry         = gptHeader->size_of_partition_entry;

    // 5. 读取并解析分区条目
    const uint64_t entry_start_lba = gptHeader->partition_entry_lba;
    const uint32_t num_entries     = gptHeader->num_partition_entries;
    const uint32_t entry_size      = gptHeader->size_of_partition_entry;

    const uint64_t total_entries_bytes = static_cast<uint64_t>(num_entries) * entry_size;
    const auto sectors_needed          = static_cast<uint32_t>((total_entries_bytes + sectorSize - 1) / sectorSize);

    std::vector<uint8_t> entries_buffer(total_entries_bytes);

    uint64_t current_lba_to_read = entry_start_lba;
    size_t buffer_offset         = 0;

    for (uint32_t i = 0; i < sectors_needed; ++i) {
        std::vector<uint8_t> sector_buffer(sectorSize);
        result = ReadDiskSector(diskPath, current_lba_to_read, &sectorSize, sector_buffer.data(), sector_buffer.size());
        if (result != READ_DISK_SECTOR_SUCCESS) {
            disk_info.has_error  = true;
            disk_info.error_code = result;
            return disk_info;
        }

        const size_t bytes_to_copy = std::min(static_cast<size_t>(sectorSize), entries_buffer.size() - buffer_offset);
        memcpy(entries_buffer.data() + buffer_offset, sector_buffer.data(), bytes_to_copy);
        buffer_offset += bytes_to_copy;
        current_lba_to_read++;
    }

    // 解析每个分区条目
    for (uint32_t i = 0; i < num_entries; ++i) {
        const size_t entry_offset = i * entry_size;
        const uint8_t* entry_data = entries_buffer.data() + entry_offset;

        // 提取分区条目各字段
        uint8_t type_guid[16];
        uint8_t unique_guid[16];
        uint64_t first_lba, last_lba;
        uint64_t attributes;
        uint16_t name_utf16[36];

        memcpy(type_guid, entry_data, 16);
        memcpy(unique_guid, entry_data + 16, 16);
        memcpy(&first_lba, entry_data + 32, 8);
        memcpy(&last_lba, entry_data + 40, 8);
        memcpy(&attributes, entry_data + 48, 8);
        memcpy(name_utf16, entry_data + 56, 72); // 72 bytes = 36 uint16_t

        // 检查分区类型 GUID 是否全零，表示未使用
        if (bool is_unused = std::ranges::all_of(type_guid, [](const uint8_t b) { return b == 0; }); !is_unused) {
            PartitionInfo part_info{};
            part_info.type_guid_str   = guidToString(type_guid);
            part_info.unique_guid_str = guidToString(unique_guid);
            part_info.first_lba       = first_lba;
            part_info.last_lba        = last_lba;
            part_info.attributes      = attributes;

            // 解析分区名称 (UTF-16 to string)
            std::wstring w_name;
            for (int j = 0; j < 36 && name_utf16[j] != 0; ++j) {
                w_name += static_cast<wchar_t>(name_utf16[j]);
            }
            // 简单转换为 std::string (注意：对于非ASCII字符可能有问题，但满足基本需求)
            part_info.name = std::string(w_name.begin(), w_name.end());

            // GPT 条目中没有文件系统类型，只有分区名称。可以将其作为标签。
            part_info.filesystem_label = part_info.name.empty() ? "<No Name>" : part_info.name;

            disk_info.partitions.push_back(part_info);
            disk_info.partition_count++;
        }
    }

    return disk_info;
}
