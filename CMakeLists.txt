cmake_minimum_required(VERSION 4.1)
set(CMAKE_CXX_STANDARD 20)
project(DiskBox)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

set(RESOURCE_FILES resources/resource.rc)

set(wxWidgets_ROOT_DIR "${CMAKE_SOURCE_DIR}/libs/wxWidgets")
set(wxWidgets_USE_STATIC ON)

find_package(wxWidgets REQUIRED COMPONENTS
        core base # 必需的基础组件
        adv       # 高级控件 (Advanced)
        aui       # 高级用户界面 (Advanced User Interface)
)

file(GLOB_RECURSE SOURCES
        src/main.cpp
        src/UAC/UAC.cpp
        src/MainWindow/MainWindow.cpp
        src/MainWindow/DiskBar.cpp
        src/Font/font.cpp
        src/hDPI/hDPI.cpp
        src/struct_definition.h
        src/Hash/CRC32_Cals.c
        src/Hash/CRC32_Cals.h
#        resources/resource.rc
#        resources/resource.h
)

add_executable(${PROJECT_NAME} WIN32
        ${SOURCES})

add_subdirectory(src/DiskControlBridge)

target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        _UNICODE
        UNICODE
        wxUSE_UNICODE
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WX_WEBP_LIB ${wxWidgets_ROOT_DIR}/lib/vc_x64_lib/wxwebpd.lib)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(WX_WEBP_LIB ${wxWidgets_ROOT_DIR}/lib/vc_x64_lib/wxwebp.lib)
else()
    set(WX_WEBP_LIB "")  # 默认值
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${WX_WEBP_LIB}
        ${wxWidgets_LIBRARIES}
        ${CMAKE_SOURCE_DIR}/cmake-build-debug/src/DiskControlBridge/DiskControlBridge.lib

        # Windows 系统库
        comctl32
        rpcrt4
        wininet
        comdlg32
        oleaut32
        ole32
        uuid
        gdi32
        advapi32
        shell32
        setupapi
        version
        shlwapi
        winspool
        winmm
        ws2_32
)