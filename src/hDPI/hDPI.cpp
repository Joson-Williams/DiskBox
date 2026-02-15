/////////////////////////////////////////////////////////////////////////////
/// Name:        src/hDPI/hDPI.cpp
/// Purpose:     To setup high DPI support.
/// Author:      Joson Williams
/// Created:     February 7, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include "hdpi.h"
#include <tuple>

// 配置高DPI
void WindowUtils::SetupHighDPI() {
    // 初始化图片处理器（通用）
    wxInitAllImageHandlers();

    // Windows平台DPI感知设置
    #ifdef __WXMSW__
    // 设置为每监视器DPI感知
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    // 优化Windows渲染质量
    wxSystemOptions::SetOption(wxT("msw.window.no-maskblt"), 1);
    wxSystemOptions::SetOption(wxT("msw.font.no-proof-quality"), 1);
    #endif

    // 通用DPI缩放配置
    wxSystemOptions::SetOption(wxT("dpi.scale"), 1);
    wxSystemOptions::SetOption(wxT("font.default_face"), wxEmptyString);
    wxSystemOptions::SetOption(wxT("font.default_size"), -1);
}

// 计算并设置窗口尺寸
void WindowUtils::SetupWindowSize(wxFrame* frame, int offsetW, int offsetH, int minW, int minH) {
    if (!frame) return;

    const wxDisplay display;
    wxRect screenRect = display.GetGeometry();

    // 屏幕尺寸有效性检查
    if (screenRect.GetWidth() <= 0 || screenRect.GetHeight() <= 0) {
        screenRect = wxRect(0, 0, 1920, 1080);
    }

    // 计算最终尺寸（保证不小于最小尺寸）
    const int newWidth = std::max(screenRect.GetWidth() - offsetW, minW);
    const int newHeight = std::max(screenRect.GetHeight() - offsetH, minH);

    frame->SetSize(newWidth, newHeight);
}

// 初始化窗口基础样式
void WindowUtils::SetupWindowStyle(wxFrame* frame) {
    if (!frame) return; // 空指针保护

    // 设置白色背景
    frame->SetBackgroundColour(wxColour(255, 255, 255));
    // 窗口居中显示
    frame->Centre();
}
