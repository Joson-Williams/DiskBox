cmake_minimum_required(VERSION 4.1)
project(DiskControlBridge)

set(CMAKE_CXX_STANDARD 20)

add_library(DiskControlBridge STATIC
        ReadDiskSector.c
        ReadDiskSector.h
        EnumPhysicalDisks.c
        EnumPhysicalDisks.h
        DCB_ErrorDefinitions.h
)
