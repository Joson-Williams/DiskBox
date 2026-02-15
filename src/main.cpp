/////////////////////////////////////////////////////////////////////////////
/// Name:        src/main.cpp
/// Purpose:     (App init)
/// Author:      Joson Williams
/// Created:     February 5, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/msw/private.h>
#include "Font/font.h"
#include "MainWindow/MainWindow.h"
#include "UAC/UAC.h"
#include "../resources/resource.h"

bool DiskBox::OnInit() {

    if (!UAC::CheckAndRequestAdmin()) return false;

    if (!UAC::IsRunningAsAdmin()) return false;

    // if (!LoadEmbeddedFont()) return false;

    return InitializeApp();
}

bool InitializeApp() {
    // 创建并显示主窗口
    auto* mainFrame = new MainWindow("DiskBox");
    mainFrame->Show(true);
    mainFrame->Raise(); // 确保窗口在最前面

    // 刷新显示
    // NOLINTNEXTLINE
    wxTheApp->GetTopWindow()->Update();
    // NOLINTNEXTLINE
    wxTheApp->GetTopWindow()->Refresh();
    return true;
}

wxIMPLEMENT_APP(DiskBox);
