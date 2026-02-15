/////////////////////////////////////////////////////////////////////////////
/// Name:        src/hDPI/hDPI.h
/// Purpose:     To setup high DPI support.
/// Author:      Joson Williams
/// Created:     February 7, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wx.h>
#include <wx/display.h>
#include <wx/sysopt.h>

/**
 * @class WindowUtils
 * @brief 窗口工具类 - 封装高DPI配置和窗口通用设置
 * @details 提供跨平台的高DPI适配、窗口尺寸计算、基础样式配置等通用功能，
 *          可在所有wxFrame派生类中复用，简化窗口初始化逻辑
 */
class WindowUtils {
public:
    /**
     * @brief 配置高DPI相关设置（跨平台兼容）
     * @details 初始化图片处理器，针对不同平台设置DPI感知模式，
     *          优化字体和窗口渲染质量，适配高分辨率屏幕
     * @note Windows平台启用每监视器DPI感知V2，其他平台使用wxWidgets默认适配
     */
    static void SetupHighDPI();

    /**
     * @brief 计算并设置窗口的最佳尺寸（基于屏幕大小，带最小尺寸限制）
     * @param frame 要设置尺寸的wxFrame窗口指针（不能为空）
     * @param offsetW 窗口宽度相对屏幕宽度的偏移量（默认100px，即屏幕宽度-100）
     * @param offsetH 窗口高度相对屏幕高度的偏移量（默认80px，即屏幕高度-80）
     * @param minW 窗口最小宽度限制（默认800px，防止窗口过小）
     * @param minH 窗口最小高度限制（默认600px，防止窗口过小）
     * @note 若屏幕尺寸获取失败，默认使用1920x1080作为基准计算
     */
    static void SetupWindowSize(wxFrame* frame,
                                int offsetW = 100,
                                int offsetH = 80,
                                int minW = 800,
                                int minH = 600);

    /**
     * @brief 初始化窗口基础样式（背景色、居中、图片处理器）
     * @param frame 要配置样式的wxFrame窗口指针（不能为空）
     * @details 设置窗口背景为白色，将窗口居中显示，确保基础视觉效果统一
     */
    static void SetupWindowStyle(wxFrame* frame);

    /**
     * @brief 分区颜色常量（抽离到工具类中，方便全局使用）
     * @details 定义8种不同的分区颜色，用于磁盘分区可视化展示，
     *          顺序依次为：蓝色、绿色、紫色、黄色、橙色、红色、灰色、青色
     */
    static const wxColour PARTITION_COLORS[8];
};