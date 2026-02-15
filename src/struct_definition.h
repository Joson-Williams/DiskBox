/////////////////////////////////////////////////////////////////////////////
/// Name:        src/struct_definition.h
/// Purpose:     To define some structs
/// Author:      Joson Williams
/// Created:     February 5, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <tuple>
#include <vector>

// 分区数据结构体
struct PartitionInfo {

    std::string name;
    double size;
    std::string format;

    explicit PartitionInfo(const std::tuple<std::string,
                                            double,
                                            std::string>& data)
        : name(std::get<0>(data)),
          size(std::get<1>(data)),
          format(std::get<2>(data)) {

        // 兜底处理空值，避免空字符串
        if (name.empty()) {
            name = "Unknown";
        }

        if (size == 0.0) {
            size = 1.0;
        }

        if (format.empty()) {
            format = "Unknown";
        }
    }

    PartitionInfo() = default;
};


struct DiskInfo {

    std::string name;
    double total_size;
    std::string PartitionTabularFormat;
    std::vector<PartitionInfo> partitions;

    explicit DiskInfo(const std::tuple<std::string,
                                       double,
                                       std::string,
                                       std::vector<PartitionInfo>>& data)
        : name(std::get<0>(data)),
          total_size(std::get<1>(data)),
          PartitionTabularFormat(std::get<2>(data)),
          partitions(std::get<3>(data)) {

        // 兜底处理
        if (name.empty()) {
            name = "Unknown";
        }

        if (total_size == 0.0) {
            total_size = 1.0;
        }

        if (PartitionTabularFormat.empty()) {
            PartitionTabularFormat = "Unknown";
        }

        if (partitions.empty()) {
            // 新买的硬盘可能没分区，有时候分区表为空很正常
            // 此处不应该有兜底处理
        }
    }

    DiskInfo() = default;
};
