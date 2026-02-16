cmake_minimum_required(VERSION 4.1)
project(DiskControlBridge)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

add_executable(DiskControlBridge
        ReadDiskSector.c
        ReadDiskSector.h
        EnumPhysicalDisks.c
        EnumPhysicalDisks.h
        DCB_ErrorDefinitions.h
        test.cpp
)

target_link_libraries(DiskControlBridge PRIVATE setupapi.lib)
