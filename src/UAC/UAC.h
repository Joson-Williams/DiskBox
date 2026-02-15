/////////////////////////////////////////////////////////////////////////////
/// Name:        src/UAC/UAC.h
/// Purpose:     To makesure the program is running as Administrator.
/// Author:      Joson Williams
/// Created:     February 5, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wx.h>

class UAC {
public:
    // 检查当前是否以管理员身份运行
    static bool IsRunningAsAdmin();

    // 请求管理员权限（如果当前不是管理员）
    static bool RequestAdminPrivileges(const wxString& reason = wxEmptyString);

    // 静默检查并请求管理员权限（无对话框）
    static bool CheckAndRequestAdmin();

    // 判断系统是否支持UAC
    static bool IsUACSupported();

    // 获取当前用户权限级别
    enum PrivilegeLevel {
        PL_UNKNOWN,
        PL_STANDARD,
        PL_ADMIN
    };

    static PrivilegeLevel GetCurrentPrivilegeLevel();
};
