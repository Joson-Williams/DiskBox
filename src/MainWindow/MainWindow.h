/////////////////////////////////////////////////////////////////////////////
/// Name:        src/MainWindow/MainWindow.h
/// Purpose:     To draw the main window.
/// Author:      Joson Williams
/// Created:     February 7, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wx.h>

/**
 * @class MainWindow
 * @brief 程序主窗口类，继承自wxFrame
 * @details 负责磁盘信息展示、磁盘工具按钮布局、分区可视化等核心UI功能
 */
class MainWindow : public wxFrame {
public:
    /**
     * @brief 构造函数
     * @param title 窗口标题字符串
     */
    explicit MainWindow(const wxString& title);
};

/**
 * @brief 初始化应用程序主窗口
 * @return 初始化成功返回true，失败返回false
 */
bool InitializeApp();