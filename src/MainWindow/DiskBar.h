/////////////////////////////////////////////////////////////////////////////
/// Name:        src/MainWindow/DiskBar.h
/// Purpose:     To draw the disk info bar on main window.
/// Author:      Joson Williams
/// Created:     February 10, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wx.h>
#include "../struct_definition.h"

void setDiskBar(
    wxWindow* parent,
    wxSizer* main_sizer,
    int diskBarHeight,
    int BUTTON_MINIMUM_WIDTH,
    const DiskInfo& diskInfo
);
