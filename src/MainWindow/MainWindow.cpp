/////////////////////////////////////////////////////////////////////////////
/// Name:        src/MainWindow/MainWindow.cpp
/// Purpose:     To draw the main window.
/// Author:      Joson Williams
/// Created:     February 7, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <tuple>
#include <string>
#include <wx/wx.h>
#include <wx/display.h>
#include "MainWindow.h"
#include "DiskBar.h"
#include "../hDPI/hDPI.h"
#include "../struct_definition.h"

constexpr int DISKROWBAR_HEIGHT = 60;
constexpr int BTN_MIN_WIDTH     = 30;

MainWindow::MainWindow(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

    WindowUtils::SetupHighDPI();
    WindowUtils::SetupWindowSize(this);
    WindowUtils::SetupWindowStyle(this);
    wxWindowBase::SetBackgroundColour(wxColour(255, 255, 255));
    Centre();

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    std::vector<PartitionInfo> partitionlist;
    partitionlist.emplace_back(std::make_tuple("Windows", 200.0, "NTFS"));
    partitionlist.emplace_back(std::make_tuple("UU", 3.0, "FAT12"));
    partitionlist.emplace_back(std::make_tuple("Files", 40, "NTFS"));
    partitionlist.emplace_back(std::make_tuple("ll", 40, "ext4"));

    DiskInfo dsk;
    dsk.name                   = "Sandisk";
    dsk.total_size             = 231.0;
    dsk.PartitionTabularFormat = "GPT";
    dsk.partitions             = partitionlist;

    setDiskBar(this,
               mainSizer,
               DISKROWBAR_HEIGHT,
               BTN_MIN_WIDTH,
               dsk);

    SetSizer(mainSizer);
    wxTopLevelWindowBase::Layout();
}
