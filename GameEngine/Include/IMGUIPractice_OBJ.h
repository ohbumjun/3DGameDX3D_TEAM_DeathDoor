#pragma once
#include "IMGUIWidget.h"

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifndef IMGUI_CDECL
#ifdef _MSC_VER
#define IMGUI_CDECL __cdecl
#else
#define IMGUI_CDECL
#endif
#endif

// Dummy data structure that we use for the Table demo.
// (pre-C++11 doesn't allow us to instantiate ImVector<MyItem> template if this structure if defined inside the demo function)
namespace
{
    // We are passing our own identifier to TableSetupColumn() to facilitate identifying columns in the sorting code.
    // This identifier will be passed down into ImGuiTableSortSpec::ColumnUserID.
    // But it is possible to omit the user id parameter of TableSetupColumn() and just use the column index instead! (ImGuiTableSortSpec::ColumnIndex)
    // If you don't use sorting, you will generally never care about giving column an ID!
    enum MyItemColumnID
    {
        MyItemColumnID_ID,
        MyItemColumnID_Name,
        MyItemColumnID_Action,
        MyItemColumnID_Quantity,
        MyItemColumnID_Description
    };

    struct MyItem
    {
        int         ID;
        const char* Name;
        int         Quantity;

        // We have a problem which is affecting _only this demo_ and should not affect your code:
        // As we don't rely on std:: or other third-party library to compile dear imgui, we only have reliable access to qsort(),
        // however qsort doesn't allow passing user data to comparing function.
        // As a workaround, we are storing the sort specs in a static/global for the comparing function to access.
        // In your own use case you would probably pass the sort specs to your sorting/comparing functions directly and not use a global.
        // We could technically call ImGui::TableGetSortSpecs() in CompareWithSortSpecs(), but considering that this function is called
        // very often by the sorting algorithm it would be a little wasteful.
        static const ImGuiTableSortSpecs* s_current_sort_specs;

        // Compare function to be used by qsort()
        static int IMGUI_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
        {
            const MyItem* a = (const MyItem*)lhs;
            const MyItem* b = (const MyItem*)rhs;
            for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
            {
                // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
                // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
                const ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
                int delta = 0;
                switch (sort_spec->ColumnUserID)
                {
                case MyItemColumnID_ID:             delta = (a->ID - b->ID);                break;
                case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));     break;
                case MyItemColumnID_Quantity:       delta = (a->Quantity - b->Quantity);    break;
                case MyItemColumnID_Description:    delta = (strcmp(a->Name, b->Name));     break;
                default: IM_ASSERT(0); break;
                }
                if (delta > 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
                if (delta < 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
            };

            // qsort() is instable so always return a way to differenciate items.
            // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
            return (a->ID - b->ID);
        };
    };
};

const ImGuiTableSortSpecs* MyItem::s_current_sort_specs = NULL;

class CIMGUIPractice_OBJ :
    public CIMGUIWidget
{
	friend class CIMGUIPopUpModal;
    friend class CIMGUIWindow;
    friend class CIMGUIWidgetList;
    friend class CIMGUIChild;
protected:
	CIMGUIPractice_OBJ();
	virtual ~CIMGUIPractice_OBJ() override;
    // Demonstrate the various window flags. Typically you would just use the default!
    bool no_titlebar;
    bool no_scrollbar;
    bool no_menu;
    bool no_move;
    bool no_resize;
    bool no_collapse;
    bool no_close;
    bool no_nav;
    bool no_background;
    bool no_bring_to_front;
    bool unsaved_document;
public:
	virtual bool Init();
	virtual void Render();
    static void PushStyleCompact()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
    }
    static void PopStyleCompact()
    {
        ImGui::PopStyleVar(2);
    }
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    static void EditTableSizingFlags(ImGuiTableFlags* p_flags)
    {
        struct EnumDesc { ImGuiTableFlags Value; const char* Name; const char* Tooltip; };
        static const EnumDesc policies[] =
        {
            { ImGuiTableFlags_None,               "Default",                            "Use default sizing policy:\n- ImGuiTableFlags_SizingFixedFit if ScrollX is on or if host window has ImGuiWindowFlags_AlwaysAutoResize.\n- ImGuiTableFlags_SizingStretchSame otherwise." },
            { ImGuiTableFlags_SizingFixedFit,     "ImGuiTableFlags_SizingFixedFit",     "Columns default to _WidthFixed (if resizable) or _WidthAuto (if not resizable), matching contents width." },
            { ImGuiTableFlags_SizingFixedSame,    "ImGuiTableFlags_SizingFixedSame",    "Columns are all the same width, matching the maximum contents width.\nImplicitly disable ImGuiTableFlags_Resizable and enable ImGuiTableFlags_NoKeepColumnsVisible." },
            { ImGuiTableFlags_SizingStretchProp,  "ImGuiTableFlags_SizingStretchProp",  "Columns default to _WidthStretch with weights proportional to their widths." },
            { ImGuiTableFlags_SizingStretchSame,  "ImGuiTableFlags_SizingStretchSame",  "Columns default to _WidthStretch with same weights." }
        };
        int idx;
        for (idx = 0; idx < IM_ARRAYSIZE(policies); idx++)
            if (policies[idx].Value == (*p_flags & ImGuiTableFlags_SizingMask_))
                break;
        const char* preview_text = (idx < IM_ARRAYSIZE(policies)) ? policies[idx].Name + (idx > 0 ? strlen("ImGuiTableFlags") : 0) : "";
        if (ImGui::BeginCombo("Sizing Policy", preview_text))
        {
            for (int n = 0; n < IM_ARRAYSIZE(policies); n++)
                if (ImGui::Selectable(policies[n].Name, idx == n))
                    *p_flags = (*p_flags & ~ImGuiTableFlags_SizingMask_) | policies[n].Value;
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);
            for (int m = 0; m < IM_ARRAYSIZE(policies); m++)
            {
                ImGui::Separator();
                ImGui::Text("%s:", policies[m].Name);
                ImGui::Separator();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().IndentSpacing * 0.5f);
                ImGui::TextUnformatted(policies[m].Tooltip);
            }
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    static void EditTableColumnsFlags(ImGuiTableColumnFlags* p_flags)
    {
        ImGui::CheckboxFlags("_Disabled", p_flags, ImGuiTableColumnFlags_Disabled); ImGui::SameLine(); HelpMarker("Master disable flag (also hide from context menu)");
        ImGui::CheckboxFlags("_DefaultHide", p_flags, ImGuiTableColumnFlags_DefaultHide);
        ImGui::CheckboxFlags("_DefaultSort", p_flags, ImGuiTableColumnFlags_DefaultSort);
        if (ImGui::CheckboxFlags("_WidthStretch", p_flags, ImGuiTableColumnFlags_WidthStretch))
            *p_flags &= ~(ImGuiTableColumnFlags_WidthMask_ ^ ImGuiTableColumnFlags_WidthStretch);
        if (ImGui::CheckboxFlags("_WidthFixed", p_flags, ImGuiTableColumnFlags_WidthFixed))
            *p_flags &= ~(ImGuiTableColumnFlags_WidthMask_ ^ ImGuiTableColumnFlags_WidthFixed);
        ImGui::CheckboxFlags("_NoResize", p_flags, ImGuiTableColumnFlags_NoResize);
        ImGui::CheckboxFlags("_NoReorder", p_flags, ImGuiTableColumnFlags_NoReorder);
        ImGui::CheckboxFlags("_NoHide", p_flags, ImGuiTableColumnFlags_NoHide);
        ImGui::CheckboxFlags("_NoClip", p_flags, ImGuiTableColumnFlags_NoClip);
        ImGui::CheckboxFlags("_NoSort", p_flags, ImGuiTableColumnFlags_NoSort);
        ImGui::CheckboxFlags("_NoSortAscending", p_flags, ImGuiTableColumnFlags_NoSortAscending);
        ImGui::CheckboxFlags("_NoSortDescending", p_flags, ImGuiTableColumnFlags_NoSortDescending);
        ImGui::CheckboxFlags("_NoHeaderLabel", p_flags, ImGuiTableColumnFlags_NoHeaderLabel);
        ImGui::CheckboxFlags("_NoHeaderWidth", p_flags, ImGuiTableColumnFlags_NoHeaderWidth);
        ImGui::CheckboxFlags("_PreferSortAscending", p_flags, ImGuiTableColumnFlags_PreferSortAscending);
        ImGui::CheckboxFlags("_PreferSortDescending", p_flags, ImGuiTableColumnFlags_PreferSortDescending);
        ImGui::CheckboxFlags("_IndentEnable", p_flags, ImGuiTableColumnFlags_IndentEnable); ImGui::SameLine(); HelpMarker("Default for column 0");
        ImGui::CheckboxFlags("_IndentDisable", p_flags, ImGuiTableColumnFlags_IndentDisable); ImGui::SameLine(); HelpMarker("Default for column >0");
    }

    static void ShowTableColumnsStatusFlags(ImGuiTableColumnFlags flags)
    {
        ImGui::CheckboxFlags("_IsEnabled", &flags, ImGuiTableColumnFlags_IsEnabled);
        ImGui::CheckboxFlags("_IsVisible", &flags, ImGuiTableColumnFlags_IsVisible);
        ImGui::CheckboxFlags("_IsSorted", &flags, ImGuiTableColumnFlags_IsSorted);
        ImGui::CheckboxFlags("_IsHovered", &flags, ImGuiTableColumnFlags_IsHovered);
    }


    // Demonstrate old/legacy Columns API!
    // [2020: Columns are under-featured and not maintained. Prefer using the more flexible and powerful BeginTable() API!]
    static void ShowDemoWindowColumns()
    {
        // IMGUI_DEMO_MARKER("Columns (legacy API)");
        bool open = ImGui::TreeNode("Legacy Columns API");
        ImGui::SameLine();
        HelpMarker("Columns() is an old API! Prefer using the more flexible and powerful BeginTable() API!");
        if (!open)
            return;

        // Basic columns
        // IMGUI_DEMO_MARKER("Columns (legacy API)/Basic");
        if (ImGui::TreeNode("Basic"))
        {
            ImGui::Text("Without border:");
            ImGui::Columns(3, "mycolumns3", false);  // 3-ways, no border
            ImGui::Separator();
            for (int n = 0; n < 14; n++)
            {
                char label[32];
                sprintf_s(label, "Item %d", n);
                if (ImGui::Selectable(label)) {}
                //if (ImGui::Button(label, ImVec2(-FLT_MIN,0.0f))) {}
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::Separator();

            ImGui::Text("With border:");
            ImGui::Columns(4, "mycolumns"); // 4-ways, with border
            ImGui::Separator();
            ImGui::Text("ID"); ImGui::NextColumn();
            ImGui::Text("Name"); ImGui::NextColumn();
            ImGui::Text("Path"); ImGui::NextColumn();
            ImGui::Text("Hovered"); ImGui::NextColumn();
            ImGui::Separator();
            const char* names[3] = { "One", "Two", "Three" };
            const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
            static int selected = -1;
            for (int i = 0; i < 3; i++)
            {
                char label[32];
                sprintf_s(label, "%04d", i);
                if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                    selected = i;
                bool hovered = ImGui::IsItemHovered();
                ImGui::NextColumn();
                ImGui::Text(names[i]); ImGui::NextColumn();
                ImGui::Text(paths[i]); ImGui::NextColumn();
                ImGui::Text("%d", hovered); ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::TreePop();
        }

        // IMGUI_DEMO_MARKER("Columns (legacy API)/Borders");
        if (ImGui::TreeNode("Borders"))
        {
            // NB: Future columns API should allow automatic horizontal borders.
            static bool h_borders = true;
            static bool v_borders = true;
            static int columns_count = 4;
            const int lines_count = 3;
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragInt("##columns_count", &columns_count, 0.1f, 2, 10, "%d columns");
            if (columns_count < 2)
                columns_count = 2;
            ImGui::SameLine();
            ImGui::Checkbox("horizontal", &h_borders);
            ImGui::SameLine();
            ImGui::Checkbox("vertical", &v_borders);
            ImGui::Columns(columns_count, NULL, v_borders);
            for (int i = 0; i < columns_count * lines_count; i++)
            {
                if (h_borders && ImGui::GetColumnIndex() == 0)
                    ImGui::Separator();
                ImGui::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
                ImGui::Text("Width %.2f", ImGui::GetColumnWidth());
                ImGui::Text("Avail %.2f", ImGui::GetContentRegionAvail().x);
                ImGui::Text("Offset %.2f", ImGui::GetColumnOffset());
                ImGui::Text("Long text that is likely to clip");
                ImGui::Button("Button", ImVec2(-FLT_MIN, 0.0f));
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            if (h_borders)
                ImGui::Separator();
            ImGui::TreePop();
        }

        // Create multiple items in a same cell before switching to next column
        // IMGUI_DEMO_MARKER("Columns (legacy API)/Mixed items");
        if (ImGui::TreeNode("Mixed items"))
        {
            ImGui::Columns(3, "mixed");
            ImGui::Separator();

            ImGui::Text("Hello");
            ImGui::Button("Banana");
            ImGui::NextColumn();

            ImGui::Text("ImGui");
            ImGui::Button("Apple");
            static float foo = 1.0f;
            ImGui::InputFloat("red", &foo, 0.05f, 0, "%.3f");
            ImGui::Text("An extra line here.");
            ImGui::NextColumn();

            ImGui::Text("Sailor");
            ImGui::Button("Corniflower");
            static float bar = 1.0f;
            ImGui::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Category A")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
            if (ImGui::CollapsingHeader("Category B")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
            if (ImGui::CollapsingHeader("Category C")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::TreePop();
        }

        // Word wrapping
        // IMGUI_DEMO_MARKER("Columns (legacy API)/Word-wrapping");
        if (ImGui::TreeNode("Word-wrapping"))
        {
            ImGui::Columns(2, "word-wrapping");
            ImGui::Separator();
            ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
            ImGui::TextWrapped("Hello Left");
            ImGui::NextColumn();
            ImGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
            ImGui::TextWrapped("Hello Right");
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::TreePop();
        }

        // IMGUI_DEMO_MARKER("Columns (legacy API)/Horizontal Scrolling");
        if (ImGui::TreeNode("Horizontal Scrolling"))
        {
            ImGui::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
            ImVec2 child_size = ImVec2(0, ImGui::GetFontSize() * 20.0f);
            ImGui::BeginChild("##ScrollingRegion", child_size, false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::Columns(10);

            // Also demonstrate using clipper for large vertical lists
            int ITEMS_COUNT = 2000;
            ImGuiListClipper clipper;
            clipper.Begin(ITEMS_COUNT);
            while (clipper.Step())
            {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                    for (int j = 0; j < 10; j++)
                    {
                        ImGui::Text("Line %d Column %d...", i, j);
                        ImGui::NextColumn();
                    }
            }
            ImGui::Columns(1);
            ImGui::EndChild();
            ImGui::TreePop();
        }

        // IMGUI_DEMO_MARKER("Columns (legacy API)/Tree");
        if (ImGui::TreeNode("Tree"))
        {
            ImGui::Columns(2, "tree", true);
            for (int x = 0; x < 3; x++)
            {
                bool open1 = ImGui::TreeNode((void*)(intptr_t)x, "Node%d", x);
                ImGui::NextColumn();
                ImGui::Text("Node contents");
                ImGui::NextColumn();
                if (open1)
                {
                    for (int y = 0; y < 3; y++)
                    {
                        bool open2 = ImGui::TreeNode((void*)(intptr_t)y, "Node%d.%d", x, y);
                        ImGui::NextColumn();
                        ImGui::Text("Node contents");
                        if (open2)
                        {
                            ImGui::Text("Even more contents");
                            if (ImGui::TreeNode("Tree in column"))
                            {
                                ImGui::Text("The quick brown fox jumps over the lazy dog");
                                ImGui::TreePop();
                            }
                        }
                        ImGui::NextColumn();
                        if (open2)
                            ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::Columns(1);
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

};

