/////////////////////////////////////////////////////////////////////////////
/// Name:        src/CRC32_Cals.h
/// Purpose:     To calculate CRC32 checksum
/// Author:      Joson Williams<joson2025@outlook.com>
/// Created:     2026/2/15
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
unsigned int xcrc32(const unsigned char* buf, int length, unsigned int init);