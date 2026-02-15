/////////////////////////////////////////////////////////////////////////////
/// Name:        src/main.h
/// Purpose:     (App init)
/// Author:      Joson Williams
/// Created:     February 5, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#pragma once
#define WXUSE_GUI 1
#include <wx/wx.h>

class DiskBox : public wxApp {
public:
    virtual bool OnInit() override;
};
