/////////////////////////////////////////////////////////////////////////////
/// Name:        src/UAC/UAC.cpp
/// Purpose:     To makesure the program is running as Administrator.
/// Author:      Joson Williams
/// Created:     February 5, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include "UAC.h"
#include "shellapi.h"

bool UAC::IsRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroupSid = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    // 创建Administrators组的SID
    if (AllocateAndInitializeSid(&ntAuthority, 2,
                                 SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                                 0, 0, 0, 0, 0, 0, &adminGroupSid)) {
        // 检查令牌成员资格
        if (!CheckTokenMembership(nullptr, adminGroupSid, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(adminGroupSid);
    }

    return isAdmin == TRUE;
}

bool UAC::RequestAdminPrivileges(const wxString& reason) {
    if (IsRunningAsAdmin()) return true;

    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);

    // 如果需要显示原因
    if (!reason.IsEmpty()) {
        wxMessageBox(reason,
                     wxString::FromUTF8("需要管理员权限"),
                     wxOK | wxICON_INFORMATION);
    }

    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.lpVerb = L"runas"; // 请求管理员权限
    sei.lpFile = path;
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei)) {
        // 请求成功，当前程序应该退出
        return true;
    }

    DWORD err = GetLastError();
    if (err == ERROR_CANCELLED) {
        wxMessageBox(wxString::FromUTF8("用户取消了权限请求。"),
                     wxString::FromUTF8("操作已取消"),
                     wxOK | wxICON_WARNING);
    } else {
        wxString msg = wxString::FromUTF8("无法请求管理员权限。错误代码: ");
        msg << err;
        wxMessageBox(msg,
                     wxString::FromUTF8("错误"),
                     wxOK | wxICON_ERROR);
    }

    return false;
}

bool UAC::CheckAndRequestAdmin() {
    if (!IsRunningAsAdmin()) {
        return RequestAdminPrivileges(wxString::FromUTF8(
                "此操作需要管理员权限才能继续执行。\n"
                "请允许程序以管理员身份运行。"));
    }
    return true;
}

bool UAC::IsUACSupported() {
    OSVERSIONINFOEXW osvi = {sizeof(osvi)};
    DWORDLONG dwlConditionMask = 0;

    osvi.dwMajorVersion = 6; // Vista及以上版本支持UAC
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION, dwlConditionMask) != FALSE;
}

UAC::PrivilegeLevel UAC::GetCurrentPrivilegeLevel() {
    if (!IsUACSupported()) return PL_UNKNOWN;

    return IsRunningAsAdmin() ? PL_ADMIN : PL_STANDARD;
}
