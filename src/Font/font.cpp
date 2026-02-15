/////////////////////////////////////////////////////////////////////////////
/// Name:        src/Font/font.cpp
/// Purpose:     To import fonts from rosource file
/// Author:      Joson Williams
/// Created:     2026/2/12
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <windows.h>
#include <vector>

bool LoadEmbeddedFont() {

    for (int i = 0x01; i <= 0x02; i++) {
        // 获取资源句柄
        HRSRC hRes = FindResource(GetModuleHandle(nullptr), MAKEINTRESOURCE(i), RT_RCDATA);
        if (!hRes) {
            return false;
        }

        // 加载资源信息
        HGLOBAL hData = LoadResource(GetModuleHandle(nullptr), hRes);
        if (!hData) {
            return false;
        }

        // 锁定资源以获取指向数据的指针
        const void* pFontData = LockResource(hData);
        if (!pFontData) {
            FreeResource(hData);
            return false;
        }

        DWORD fontSize = SizeofResource(GetModuleHandle(nullptr), hRes);
        if (fontSize == 0) {
            FreeResource(hData);
            return false;
        }

        // 将字体数据复制到可写内存 (AddFontMemResourceEx 需要)
        std::vector<char> fontBuffer(fontSize);
        memcpy(fontBuffer.data(), pFontData, fontSize);

        // 将字体添加到系统内存中
        DWORD numFontsAdded = 0;
        HANDLE hFont        = AddFontMemResourceEx(
                fontBuffer.data(), // 指向字体数据的指针
                fontSize,          // 数据大小
                nullptr,           // 保留参数，通常为 NULL
                &numFontsAdded     // 输出参数，返回成功添加的字体数量
                );

        if (!hFont || numFontsAdded == 0) {
            FreeResource(hData);
            return false;
        }
    }

    return true;
}
