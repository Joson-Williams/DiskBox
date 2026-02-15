/////////////////////////////////////////////////////////////////////////////
/// Name:        src/MainWindow/DiskBar.cpp
/// Purpose:     To draw the disk info bar on main window.
/// Author:      Joson Williams
/// Created:     February 10, 2026
/// Copyright:   (c) 2024-2026 JOSON
/// Licence:     GNU GENERAL PUBLIC LICENSE v3
/////////////////////////////////////////////////////////////////////////////

//TODO 气泡。

#include "DiskBar.h"
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <array>
#include <algorithm>
#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/dcclient.h>
#include <wx/sizer.h>
#include <wx/dcgraph.h>
#include "../struct_definition.h"

#define JLEFT_ROUNDED_RECT 0x01
#define JRIGHT_ROUNDED_RECT 0x02
#define JROUNDED_RECT 0x03
#define JNO_ROUNDED_RECT 0x04

namespace {
    const std::array<std::pair<const char*, wxColour>, 16> COLOUR_ARRAY = {
        {
            {"FAT12", wxColour(206, 85, 84)},
            {"FAT16", wxColour(228, 92, 94)},
            {"FAT32", wxColour(255, 83, 84)},
            {"NTFS", wxColour(78, 170, 112)},
            {"exFAT", wxColour(53, 199, 115)},
            {"HFS+", wxColour(232, 233, 0)},
            {"APFS", wxColour(96, 201, 248)},
            {"ext2", wxColour(155, 112, 254)},
            {"ext3", wxColour(144, 102, 255)},
            {"ext4", wxColour(129, 83, 245)},
            {"XFS", wxColour(19, 194, 194)},
            {"Btrfs", wxColour(114, 46, 209)},
            {"ZFS", wxColour(235, 47, 150)},
            {"ReFS", wxColour(61, 197, 93)},
            {"Unknown", wxColour(191, 191, 191)},
            {"Unassigned", wxColour(229, 229, 229)}
        }
    };

    std::map<std::string, wxColour> createColourDict() {
        std::map<std::string, wxColour> dict;
        for (const auto& [key, value] : COLOUR_ARRAY) {
            dict[key] = value;
        }
        return dict;
    }

    const std::map<std::string, wxColour> colour_dict = createColourDict();

    constexpr int RADIUS = 15;

    /**
     * @brief 一个自定义的 wxToggleButton，用于在磁盘条中显示分区信息。
     *
     * 此按钮根据分区的文件系统格式显示不同的颜色，并支持不同类型的圆角边框。
     */
    class DiskBarToggleButton : public wxToggleButton {
    public:
        /**
         * @brief 构造一个新的 DiskBarToggleButton 对象。
         * @param parent 父窗口。
         * @param id 按钮 ID。
         * @param label 按钮标签。
         * @param partition 分区信息。
         * @param rectType 按钮的圆角类型。
         * @param pos 位置。
         * @param size 尺寸。
         * @param style 样式。
         * @param validator 验证器。
         * @param name 名称。
         */
        DiskBarToggleButton(
                wxWindow* parent,
                const wxWindowID id,
                const wxString& label,
                const PartitionInfo& partition,
                const int rectType           = JNO_ROUNDED_RECT,
                const wxPoint& pos           = wxDefaultPosition,
                const wxSize& size           = wxDefaultSize,
                long style                   = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name         = wxEmptyString
                )
            : wxToggleButton(parent, id, label, pos, size, style, validator, name),
              m_partition(partition),
              m_rectType(rectType) {
            Bind(wxEVT_PAINT, &DiskBarToggleButton::OnPaint, this);
            Bind(wxEVT_SIZE, &DiskBarToggleButton::OnSize, this);
            wxWindow::SetDoubleBuffered(true);
        }

    protected:
        void OnSize(wxSizeEvent& event) {
            Refresh(true);
            event.Skip();
        }

        void OnPaint(wxPaintEvent& event) {
            wxPaintDC dc(this);
            wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
            if (!gc) return;

            const wxSize btnSize = GetClientSize();

            // 获取背景颜色
            const std::string format = m_partition.format;
            const wxColour bgColour = colour_dict.contains(format) ? colour_dict.at(format) : colour_dict.at("Unknown");

            // 清空背景
            dc.SetBackground(*wxWHITE_BRUSH); // 使用白色背景刷清空
            dc.Clear();

            // 设置绘图样式
            gc->SetBrush(wxBrush(bgColour));
            gc->SetPen(wxPen(bgColour, 1));

            const wxRect rect(0, 0, btnSize.GetWidth(), btnSize.GetHeight());
            const double radius = std::min(static_cast<double>(RADIUS), btnSize.GetHeight() / 2.0);

            // 根据类型绘制
            switch (m_rectType) {
                case JNO_ROUNDED_RECT: DrawNoRoundedRect(gc, rect);
                    break;
                case JLEFT_ROUNDED_RECT: DrawLeftRoundedRect(gc, rect, radius);
                    break;
                case JRIGHT_ROUNDED_RECT: DrawRightRoundedRect(gc, rect, radius);
                    break;
                case JROUNDED_RECT:
                default: DrawAllRoundedRect(gc, rect, radius);
                    break;
            }

            // 绘制文字
            // 判断分区卷标是否为空，为空则默认
            const wxString text = !m_partition.name.empty() ? m_partition.name : wxString::FromUTF8("Partition");
            // wxFont font(12,
            //     wxFONTFAMILY_DEFAULT,
            //     wxFONTSTYLE_NORMAL,
            //     wxFONTWEIGHT_NORMAL,
            //     false,
            //     "MiSans SemiBold");
            const wxFont font(12,
                              wxFONTFAMILY_DEFAULT,
                              wxFONTSTYLE_NORMAL,
                              wxFONTWEIGHT_BOLD,
                              false,
                              "Microsoft Yahei");
            dc.SetFont(font);
            dc.SetTextForeground(wxColour(0, 0, 0));
            const wxSize textSize = dc.GetTextExtent(text);

            // 如果按钮太窄就不画
            if (textSize.GetWidth() * 2 > btnSize.GetWidth()) {
                delete gc;
                return;
            };

            // 计算文字居中的位置
            const int x = (btnSize.GetWidth() - textSize.GetWidth()) / 2;
            const int y = (btnSize.GetHeight() - textSize.GetHeight()) / 2;
            dc.DrawText(text, x, y);

            delete gc;
        }

        // 绘制辅助函数
        static void DrawNoRoundedRect(wxGraphicsContext* gc, const wxRect& rect) {
            wxGraphicsPath path = gc->CreatePath();
            path.AddRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
            gc->FillPath(path);
        }

        static void DrawAllRoundedRect(wxGraphicsContext* gc, const wxRect& rect, const double radius) {
            wxGraphicsPath path = gc->CreatePath();
            path.AddRoundedRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), radius);
            gc->FillPath(path);
        }

        static void DrawLeftRoundedRect(wxGraphicsContext* gc, const wxRect& rect, const double radius) {
            wxGraphicsPath path = gc->CreatePath();
            const double x      = rect.GetX();
            const double y      = rect.GetY();
            const double w      = rect.GetWidth();
            const double h      = rect.GetHeight();

            path.MoveToPoint(x + radius, y);
            path.AddLineToPoint(x + w, y);
            path.AddLineToPoint(x + w, y + h);
            path.AddLineToPoint(x + radius, y + h);
            path.AddArcToPoint(x, y + h, x, y + h - radius, radius);
            path.AddLineToPoint(x, y + radius);
            path.AddArcToPoint(x, y, x + radius, y, radius);
            path.CloseSubpath();
            gc->FillPath(path);
        }

        static void DrawRightRoundedRect(wxGraphicsContext* gc, const wxRect& rect, const double radius) {
            wxGraphicsPath path = gc->CreatePath();
            const double x      = rect.GetX();
            const double y      = rect.GetY();
            const double w      = rect.GetWidth();
            const double h      = rect.GetHeight();

            path.MoveToPoint(x, y);
            path.AddLineToPoint(x, y + h);
            path.AddLineToPoint(x + w - radius, y + h);
            path.AddArcToPoint(x + w, y + h, x + w, y + h - radius, radius);
            path.AddLineToPoint(x + w, y + radius);
            path.AddArcToPoint(x + w, y, x + w - radius, y, radius);
            path.AddLineToPoint(x, y);
            path.CloseSubpath();
            gc->FillPath(path);
        }

    private:
        const PartitionInfo m_partition; // 使用成员变量替代 const 引用，避免潜在生命周期问题
        const int m_rectType;
    };

    // 私有辅助函数：计算单个分区项的规格
    struct PartitionItemSpecs {
        int proportion;
        int rectType;
    };

    PartitionItemSpecs _calculatePartitionItemSpecs(size_t index, size_t totalItems, const PartitionInfo& partition) {
        PartitionItemSpecs specs{};
        specs.proportion = static_cast<int>(partition.size);

        if (totalItems == 1) {
            specs.rectType = JROUNDED_RECT;
        } else if (index == 0) {
            specs.rectType = JLEFT_ROUNDED_RECT;
        } else if (index == totalItems - 1) {
            specs.rectType = JRIGHT_ROUNDED_RECT;
        } else {
            specs.rectType = JNO_ROUNDED_RECT;
        }
        return specs;
    }

} // namespace


/**
 * @brief 根据提供的磁盘信息，在指定的父窗口和 Sizer 中创建并填充一个磁盘条。
 * @param parent 父窗口指针。
 * @param main_sizer 主要的垂直 Sizer，用于添加新的磁盘条面板。
 * @param diskBarHeight 磁盘条的高度。
 * @param BUTTON_MINIMUM_WIDTH 按钮的最小宽度。
 * @param diskInfo 包含磁盘及其分区信息的数据结构。
 */
void setDiskBar(
        wxWindow* parent,
        wxSizer* main_sizer,
        const int diskBarHeight,
        const int BUTTON_MINIMUM_WIDTH,
        const DiskInfo& diskInfo
        ) {
    // 解析磁盘信息
    const std::string diskName = diskInfo.name;
    const auto& partitionList  = diskInfo.partitions;

    // 创建顶层面板和水平布局
    auto* rowPanel     = new wxPanel(parent, wxID_ANY);
    auto* diskRowSizer = new wxBoxSizer(wxHORIZONTAL);
    rowPanel->SetSizer(diskRowSizer);

    // 添加顶层面板到主 sizer
    main_sizer->Add(rowPanel, 1, wxEXPAND); // 恢复为 1

    // 计算面板的最大高度
    const int panelHeight = parent->GetSize().GetHeight() / 25; // 已恢复此行

    // 计算左侧标签面板
    const int label_width = parent->GetSize().GetWidth() / 6;
    auto* labelPanel      = new wxPanel(rowPanel, wxID_ANY);
    auto* labelSizer      = new wxBoxSizer(wxHORIZONTAL);
    auto* diskLabel       = new wxStaticText(labelPanel, wxID_ANY, wxString::FromUTF8(diskName.c_str()));
    labelSizer->Add(diskLabel, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    labelPanel->SetSizer(labelSizer);
    labelPanel->SetMinSize(wxSize(label_width, wxDefaultCoord));
    labelPanel->SetMaxSize(wxSize(label_width, panelHeight)); // 应用最大高度

    // 计算右侧分区容器面板
    auto* partitionContainerPanel = new wxPanel(rowPanel, wxID_ANY);
    auto* partitionBarSizer       = new wxBoxSizer(wxHORIZONTAL);
    partitionContainerPanel->SetSizer(partitionBarSizer);
    partitionContainerPanel->SetMaxSize(wxSize(-1, panelHeight)); // 应用最大高度

    // 将标签和容器面板添加到行布局
    diskRowSizer->Add(labelPanel, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    diskRowSizer->Add(partitionContainerPanel, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    // 遍历分区列表，创建按钮
    int id_counter          = 10000;
    const size_t totalItems = partitionList.size();

    for (size_t i = 0; i < totalItems; ++i) {
        const auto& partition = partitionList[i];

        // 使用辅助函数计算规格
        auto specs = _calculatePartitionItemSpecs(i, totalItems, partition);

        // 创建按钮
        auto* btn = new DiskBarToggleButton(
                partitionContainerPanel,
                id_counter,
                wxString::FromUTF8(partition.name.c_str()),
                partition, // 传递 const ref 到构造函数，由内部拷贝
                specs.rectType
                );
        btn->SetMinSize(wxSize(BUTTON_MINIMUM_WIDTH, diskBarHeight));

        // 存储分区数据副本供外部回调使用
        auto* partData = new PartitionInfo(partition);
        btn->SetClientData(partData);

        // 添加按钮到分区条的布局中
        partitionBarSizer->Add(btn, specs.proportion, wxEXPAND);

        id_counter++;
    }
}
