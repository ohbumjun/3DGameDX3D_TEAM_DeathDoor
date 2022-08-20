#include "IMGUIPractice_OBJ.h"

CIMGUIPractice_OBJ::CIMGUIPractice_OBJ()
{
}

CIMGUIPractice_OBJ::~CIMGUIPractice_OBJ()
{
}

bool CIMGUIPractice_OBJ::Init()
{
	return true;
}

void CIMGUIPractice_OBJ::Render()
{
    //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    // // IMGUI_DEMO_MARKER("Tables");
    if (!ImGui::CollapsingHeader("Tables & Columns"))
        return;

    // Using those as a base value to create width/height that are factor of the size of our font
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    ImGui::PushID("Tables");

    int open_action = -1;
    if (ImGui::Button("Open all"))
        open_action = 1;
    ImGui::SameLine();
    if (ImGui::Button("Close all"))
        open_action = 0;
    ImGui::SameLine();

    // Options
    static bool disable_indent = false;
    ImGui::Checkbox("Disable tree indentation", &disable_indent);
    ImGui::SameLine();
    HelpMarker("Disable the indenting of tree nodes so demo tables can use the full window width.");
    ImGui::Separator();
    if (disable_indent)
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);

    // About Styling of tables
    // Most settings are configured on a per-table basis via the flags passed to BeginTable() and TableSetupColumns APIs.
    // There are however a few settings that a shared and part of the ImGuiStyle structure:
    //   style.CellPadding                          // Padding within each cell
    //   style.Colors[ImGuiCol_TableHeaderBg]       // Table header background
    //   style.Colors[ImGuiCol_TableBorderStrong]   // Table outer and header borders
    //   style.Colors[ImGuiCol_TableBorderLight]    // Table inner borders
    //   style.Colors[ImGuiCol_TableRowBg]          // Table row background when ImGuiTableFlags_RowBg is enabled (even rows)
    //   style.Colors[ImGuiCol_TableRowBgAlt]       // Table row background when ImGuiTableFlags_RowBg is enabled (odds rows)

    // Demos
    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Basic");
    if (ImGui::TreeNode("Basic"))
    {
        // Here we will showcase three different ways to output a table.
        // They are very simple variations of a same thing!

        // [Method 1] Using TableNextRow() to create a new row, and TableSetColumnIndex() to select the column.
        // In many situations, this is the most flexible and easy to use pattern.
        HelpMarker("Using TableNextRow() + calling TableSetColumnIndex() _before_ each cell, in a loop.");
        if (ImGui::BeginTable("table1", 3))
        {
            for (int row = 0; row < 4; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Row %d Column %d", row, column);
                }
            }
            ImGui::EndTable();
        }

        // [Method 2] Using TableNextColumn() called multiple times, instead of using a for loop + TableSetColumnIndex().
        // This is generally more convenient when you have code manually submitting the contents of each columns.
        HelpMarker("Using TableNextRow() + calling TableNextColumn() _before_ each cell, manually.");
        if (ImGui::BeginTable("table2", 3))
        {
            for (int row = 0; row < 4; row++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Row %d", row);
                ImGui::TableNextColumn();
                ImGui::Text("Some contents");
                ImGui::TableNextColumn();
                ImGui::Text("123.456");
            }
            ImGui::EndTable();
        }

        // [Method 3] We call TableNextColumn() _before_ each cell. We never call TableNextRow(),
        // as TableNextColumn() will automatically wrap around and create new roes as needed.
        // This is generally more convenient when your cells all contains the same type of data.
        HelpMarker(
            "Only using TableNextColumn(), which tends to be convenient for tables where every cells contains the same type of contents.\n"
            "This is also more similar to the old NextColumn() function of the Columns API, and provided to facilitate the Columns->Tables API transition.");
        if (ImGui::BeginTable("table3", 3))
        {
            for (int item = 0; item < 14; item++)
            {
                ImGui::TableNextColumn();
                ImGui::Text("Item %d", item);
            }
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Borders, background");
    if (ImGui::TreeNode("Borders, background"))
    {
        // Expose a few Borders related flags interactively
        enum ContentsType { CT_Text, CT_FillButton };
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        static bool display_headers = false;
        static int contents_type = CT_Text;

        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_RowBg", &flags, ImGuiTableFlags_RowBg);
        ImGui::CheckboxFlags("ImGuiTableFlags_Borders", &flags, ImGuiTableFlags_Borders);
        ImGui::SameLine(); HelpMarker("ImGuiTableFlags_Borders\n = ImGuiTableFlags_BordersInnerV\n | ImGuiTableFlags_BordersOuterV\n | ImGuiTableFlags_BordersInnerV\n | ImGuiTableFlags_BordersOuterH");
        ImGui::Indent();

        ImGui::CheckboxFlags("ImGuiTableFlags_BordersH", &flags, ImGuiTableFlags_BordersH);
        ImGui::Indent();
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuterH", &flags, ImGuiTableFlags_BordersOuterH);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersInnerH", &flags, ImGuiTableFlags_BordersInnerH);
        ImGui::Unindent();

        ImGui::CheckboxFlags("ImGuiTableFlags_BordersV", &flags, ImGuiTableFlags_BordersV);
        ImGui::Indent();
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuterV", &flags, ImGuiTableFlags_BordersOuterV);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersInnerV", &flags, ImGuiTableFlags_BordersInnerV);
        ImGui::Unindent();

        ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuter", &flags, ImGuiTableFlags_BordersOuter);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersInner", &flags, ImGuiTableFlags_BordersInner);
        ImGui::Unindent();

        ImGui::AlignTextToFramePadding(); ImGui::Text("Cell contents:");
        ImGui::SameLine(); ImGui::RadioButton("Text", &contents_type, CT_Text);
        ImGui::SameLine(); ImGui::RadioButton("FillButton", &contents_type, CT_FillButton);
        ImGui::Checkbox("Display headers", &display_headers);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBody", &flags, ImGuiTableFlags_NoBordersInBody); ImGui::SameLine(); HelpMarker("Disable vertical borders in columns Body (borders will always appears in Headers");
        PopStyleCompact();

        if (ImGui::BeginTable("table1", 3, flags))
        {
            // Display headers so we can inspect their interaction with borders.
            // (Headers are not the main purpose of this section of the demo, so we are not elaborating on them too much. See other sections for details)
            if (display_headers)
            {
                ImGui::TableSetupColumn("One");
                ImGui::TableSetupColumn("Two");
                ImGui::TableSetupColumn("Three");
                ImGui::TableHeadersRow();
            }

            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    char buf[32];
                    sprintf_s(buf, "Hello %d,%d", column, row);
                    if (contents_type == CT_Text)
                        ImGui::TextUnformatted(buf);
                    else if (contents_type == CT_FillButton)
                        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Resizable, stretch");
    if (ImGui::TreeNode("Resizable, stretch"))
    {
        // By default, if we don't enable ScrollX the sizing policy for each columns is "Stretch"
        // Each columns maintain a sizing weight, and they will occupy all available width.
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody;
        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags, ImGuiTableFlags_Resizable);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersV", &flags, ImGuiTableFlags_BordersV);
        ImGui::SameLine(); HelpMarker("Using the _Resizable flag automatically enables the _BordersInnerV flag as well, this is why the resize borders are still showing when unchecking this.");
        PopStyleCompact();

        if (ImGui::BeginTable("table1", 3, flags))
        {
            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Hello %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Resizable, fixed");
    if (ImGui::TreeNode("Resizable, fixed"))
    {
        // Here we use ImGuiTableFlags_SizingFixedFit (even though _ScrollX is not set)
        // So columns will adopt the "Fixed" policy and will maintain a fixed width regardless of the whole available width (unless table is small)
        // If there is not enough available width to fit all columns, they will however be resized down.
        // FIXME-TABLE: Providing a stretch-on-init would make sense especially for tables which don't have saved settings
        HelpMarker(
            "Using _Resizable + _SizingFixedFit flags.\n"
            "Fixed-width columns generally makes more sense if you want to use horizontal scrolling.\n\n"
            "Double-click a column border to auto-fit the column to its contents.");
        PushStyleCompact();
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody;
        ImGui::CheckboxFlags("ImGuiTableFlags_NoHostExtendX", &flags, ImGuiTableFlags_NoHostExtendX);
        PopStyleCompact();

        if (ImGui::BeginTable("table1", 3, flags))
        {
            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Hello %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Resizable, mixed");
    if (ImGui::TreeNode("Resizable, mixed"))
    {
        HelpMarker(
            "Using TableSetupColumn() to alter resizing policy on a per-column basis.\n\n"
            "When combining Fixed and Stretch columns, generally you only want one, maybe two trailing columns to use _WidthStretch.");
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table1", 3, flags))
        {
            ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("CCC", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("%s %d,%d", (column == 2) ? "Stretch" : "Fixed", column, row);
                }
            }
            ImGui::EndTable();
        }
        if (ImGui::BeginTable("table2", 6, flags))
        {
            ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("CCC", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
            ImGui::TableSetupColumn("DDD", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("EEE", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("FFF", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultHide);
            ImGui::TableHeadersRow();
            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 6; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("%s %d,%d", (column >= 3) ? "Stretch" : "Fixed", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Reorderable, hideable, with headers");
    if (ImGui::TreeNode("Reorderable, hideable, with headers"))
    {
        HelpMarker(
            "Click and drag column headers to reorder columns.\n\n"
            "Right-click on a header to open a context menu.");
        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags, ImGuiTableFlags_Resizable);
        ImGui::CheckboxFlags("ImGuiTableFlags_Reorderable", &flags, ImGuiTableFlags_Reorderable);
        ImGui::CheckboxFlags("ImGuiTableFlags_Hideable", &flags, ImGuiTableFlags_Hideable);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBody", &flags, ImGuiTableFlags_NoBordersInBody);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBodyUntilResize", &flags, ImGuiTableFlags_NoBordersInBodyUntilResize); ImGui::SameLine(); HelpMarker("Disable vertical borders in columns Body until hovered for resize (borders will always appears in Headers)");
        PopStyleCompact();

        if (ImGui::BeginTable("table1", 3, flags))
        {
            // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
            // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
            ImGui::TableSetupColumn("One");
            ImGui::TableSetupColumn("Two");
            ImGui::TableSetupColumn("Three");
            ImGui::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Hello %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }

        // Use outer_size.x == 0.0f instead of default to make the table as tight as possible (only valid when no scrolling and no stretch column)
        if (ImGui::BeginTable("table2", 3, flags | ImGuiTableFlags_SizingFixedFit, ImVec2(0.0f, 0.0f)))
        {
            ImGui::TableSetupColumn("One");
            ImGui::TableSetupColumn("Two");
            ImGui::TableSetupColumn("Three");
            ImGui::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Fixed %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Padding");
    if (ImGui::TreeNode("Padding"))
    {
        // First example: showcase use of padding flags and effect of BorderOuterV/BorderInnerV on X padding.
        // We don't expose BorderOuterH/BorderInnerH here because they have no effect on X padding.
        HelpMarker(
            "We often want outer padding activated when any using features which makes the edges of a column visible:\n"
            "e.g.:\n"
            "- BorderOuterV\n"
            "- any form of row selection\n"
            "Because of this, activating BorderOuterV sets the default to PadOuterX. Using PadOuterX or NoPadOuterX you can override the default.\n\n"
            "Actual padding values are using style.CellPadding.\n\n"
            "In this demo we don't show horizontal borders to emphasis how they don't affect default horizontal padding.");

        static ImGuiTableFlags flags1 = ImGuiTableFlags_BordersV;
        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_PadOuterX", &flags1, ImGuiTableFlags_PadOuterX);
        ImGui::SameLine(); HelpMarker("Enable outer-most padding (default if ImGuiTableFlags_BordersOuterV is set)");
        ImGui::CheckboxFlags("ImGuiTableFlags_NoPadOuterX", &flags1, ImGuiTableFlags_NoPadOuterX);
        ImGui::SameLine(); HelpMarker("Disable outer-most padding (default if ImGuiTableFlags_BordersOuterV is not set)");
        ImGui::CheckboxFlags("ImGuiTableFlags_NoPadInnerX", &flags1, ImGuiTableFlags_NoPadInnerX);
        ImGui::SameLine(); HelpMarker("Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off)");
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuterV", &flags1, ImGuiTableFlags_BordersOuterV);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersInnerV", &flags1, ImGuiTableFlags_BordersInnerV);
        static bool show_headers = false;
        ImGui::Checkbox("show_headers", &show_headers);
        PopStyleCompact();

        if (ImGui::BeginTable("table_padding", 3, flags1))
        {
            if (show_headers)
            {
                ImGui::TableSetupColumn("One");
                ImGui::TableSetupColumn("Two");
                ImGui::TableSetupColumn("Three");
                ImGui::TableHeadersRow();
            }

            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    if (row == 0)
                    {
                        ImGui::Text("Avail %.2f", ImGui::GetContentRegionAvail().x);
                    }
                    else
                    {
                        char buf[32];
                        sprintf_s(buf, "Hello %d,%d", column, row);
                        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                    }
                    //if (ImGui::TableGetColumnFlags() & ImGuiTableColumnFlags_IsHovered)
                    //    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 100, 0, 255));
                }
            }
            ImGui::EndTable();
        }

        // Second example: set style.CellPadding to (0.0) or a custom value.
        // FIXME-TABLE: Vertical border effectively not displayed the same way as horizontal one...
        HelpMarker("Setting style.CellPadding to (0,0) or a custom value.");
        static ImGuiTableFlags flags2 = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        static ImVec2 cell_padding(0.0f, 0.0f);
        static bool show_widget_frame_bg = true;

        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_Borders", &flags2, ImGuiTableFlags_Borders);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersH", &flags2, ImGuiTableFlags_BordersH);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersV", &flags2, ImGuiTableFlags_BordersV);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersInner", &flags2, ImGuiTableFlags_BordersInner);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuter", &flags2, ImGuiTableFlags_BordersOuter);
        ImGui::CheckboxFlags("ImGuiTableFlags_RowBg", &flags2, ImGuiTableFlags_RowBg);
        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags2, ImGuiTableFlags_Resizable);
        ImGui::Checkbox("show_widget_frame_bg", &show_widget_frame_bg);
        ImGui::SliderFloat2("CellPadding", &cell_padding.x, 0.0f, 10.0f, "%.0f");
        PopStyleCompact();

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
        if (ImGui::BeginTable("table_padding_2", 3, flags2))
        {
            static char text_bufs[3 * 5][16]; // Mini text storage for 3x5 cells
            static bool init = true;
            if (!show_widget_frame_bg)
                ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
            for (int cell = 0; cell < 3 * 5; cell++)
            {
                ImGui::TableNextColumn();
                if (init)
                    strcpy_s(text_bufs[cell], "edit me");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::PushID(cell);
                ImGui::InputText("##cell", text_bufs[cell], IM_ARRAYSIZE(text_bufs[cell]));
                ImGui::PopID();
            }
            if (!show_widget_frame_bg)
                ImGui::PopStyleColor();
            init = false;
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Explicit widths");
    if (ImGui::TreeNode("Sizing policies"))
    {
        static ImGuiTableFlags flags1 = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_ContextMenuInBody;
        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags1, ImGuiTableFlags_Resizable);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoHostExtendX", &flags1, ImGuiTableFlags_NoHostExtendX);
        PopStyleCompact();

        static ImGuiTableFlags sizing_policy_flags[4] = { ImGuiTableFlags_SizingFixedFit, ImGuiTableFlags_SizingFixedSame, ImGuiTableFlags_SizingStretchProp, ImGuiTableFlags_SizingStretchSame };
        for (int table_n = 0; table_n < 4; table_n++)
        {
            ImGui::PushID(table_n);
            ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 30);
            EditTableSizingFlags(&sizing_policy_flags[table_n]);

            // To make it easier to understand the different sizing policy,
            // For each policy: we display one table where the columns have equal contents width, and one where the columns have different contents width.
            if (ImGui::BeginTable("table1", 3, sizing_policy_flags[table_n] | flags1))
            {
                for (int row = 0; row < 3; row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("Oh dear");
                    ImGui::TableNextColumn(); ImGui::Text("Oh dear");
                    ImGui::TableNextColumn(); ImGui::Text("Oh dear");
                }
                ImGui::EndTable();
            }
            if (ImGui::BeginTable("table2", 3, sizing_policy_flags[table_n] | flags1))
            {
                for (int row = 0; row < 3; row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("AAAA");
                    ImGui::TableNextColumn(); ImGui::Text("BBBBBBBB");
                    ImGui::TableNextColumn(); ImGui::Text("CCCCCCCCCCCC");
                }
                ImGui::EndTable();
            }
            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Advanced");
        ImGui::SameLine();
        HelpMarker("This section allows you to interact and see the effect of various sizing policies depending on whether Scroll is enabled and the contents of your columns.");

        enum ContentsType { CT_ShowWidth, CT_ShortText, CT_LongText, CT_Button, CT_FillButton, CT_InputText };
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
        static int contents_type = CT_ShowWidth;
        static int column_count = 3;

        PushStyleCompact();
        ImGui::PushID("Advanced");
        ImGui::PushItemWidth(TEXT_BASE_WIDTH * 30);
        EditTableSizingFlags(&flags);
        ImGui::Combo("Contents", &contents_type, "Show width\0Short Text\0Long Text\0Button\0Fill Button\0InputText\0");
        if (contents_type == CT_FillButton)
        {
            ImGui::SameLine();
            HelpMarker("Be mindful that using right-alignment (e.g. size.x = -FLT_MIN) creates a feedback loop where contents width can feed into auto-column width can feed into contents width.");
        }
        ImGui::DragInt("Columns", &column_count, 0.1f, 1, 64, "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags, ImGuiTableFlags_Resizable);
        ImGui::CheckboxFlags("ImGuiTableFlags_PreciseWidths", &flags, ImGuiTableFlags_PreciseWidths);
        ImGui::SameLine(); HelpMarker("Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.");
        ImGui::CheckboxFlags("ImGuiTableFlags_ScrollX", &flags, ImGuiTableFlags_ScrollX);
        ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &flags, ImGuiTableFlags_ScrollY);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoClip", &flags, ImGuiTableFlags_NoClip);
        ImGui::PopItemWidth();
        ImGui::PopID();
        PopStyleCompact();

        if (ImGui::BeginTable("table2", column_count, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 7)))
        {
            for (int cell = 0; cell < 10 * column_count; cell++)
            {
                ImGui::TableNextColumn();
                int column = ImGui::TableGetColumnIndex();
                int row = ImGui::TableGetRowIndex();

                ImGui::PushID(cell);
                char label[32];
                static char text_buf[32] = "";
                sprintf_s(label, "Hello %d,%d", column, row);
                switch (contents_type)
                {
                case CT_ShortText:  ImGui::TextUnformatted(label); break;
                case CT_LongText:   ImGui::Text("Some %s text %d,%d\nOver two lines..", column == 0 ? "long" : "longeeer", column, row); break;
                case CT_ShowWidth:  ImGui::Text("W: %.1f", ImGui::GetContentRegionAvail().x); break;
                case CT_Button:     ImGui::Button(label); break;
                case CT_FillButton: ImGui::Button(label, ImVec2(-FLT_MIN, 0.0f)); break;
                case CT_InputText:  ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputText("##", text_buf, IM_ARRAYSIZE(text_buf)); break;
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Vertical scrolling, with clipping");
    if (ImGui::TreeNode("Vertical scrolling, with clipping"))
    {
        HelpMarker("Here we activate ScrollY, which will create a child window container to allow hosting scrollable contents.\n\nWe also demonstrate using ImGuiListClipper to virtualize the submission of many items.");
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &flags, ImGuiTableFlags_ScrollY);
        PopStyleCompact();

        // When using ScrollX or ScrollY we need to specify a size for our table container!
        // Otherwise by default the table will fit all available space, like a BeginChild() call.
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
        if (ImGui::BeginTable("table_scrolly", 3, flags, outer_size))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn("One", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Two", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Three", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            // Demonstrate using clipper for large vertical lists
            ImGuiListClipper clipper;
            clipper.Begin(1000);
            while (clipper.Step())
            {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        ImGui::Text("Hello %d,%d", column, row);
                    }
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Horizontal scrolling");
    if (ImGui::TreeNode("Horizontal scrolling"))
    {
        HelpMarker(
            "When ScrollX is enabled, the default sizing policy becomes ImGuiTableFlags_SizingFixedFit, "
            "as automatically stretching columns doesn't make much sense with horizontal scrolling.\n\n"
            "Also note that as of the current version, you will almost always want to enable ScrollY along with ScrollX,"
            "because the container window won't automatically extend vertically to fix contents (this may be improved in future versions).");
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
        static int freeze_cols = 1;
        static int freeze_rows = 1;

        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags, ImGuiTableFlags_Resizable);
        ImGui::CheckboxFlags("ImGuiTableFlags_ScrollX", &flags, ImGuiTableFlags_ScrollX);
        ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &flags, ImGuiTableFlags_ScrollY);
        ImGui::SetNextItemWidth(ImGui::GetFrameHeight());
        ImGui::DragInt("freeze_cols", &freeze_cols, 0.2f, 0, 9, NULL, ImGuiSliderFlags_NoInput);
        ImGui::SetNextItemWidth(ImGui::GetFrameHeight());
        ImGui::DragInt("freeze_rows", &freeze_rows, 0.2f, 0, 9, NULL, ImGuiSliderFlags_NoInput);
        PopStyleCompact();

        // When using ScrollX or ScrollY we need to specify a size for our table container!
        // Otherwise by default the table will fit all available space, like a BeginChild() call.
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
        if (ImGui::BeginTable("table_scrollx", 7, flags, outer_size))
        {
            ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
            ImGui::TableSetupColumn("Line #", ImGuiTableColumnFlags_NoHide); // Make the first column not hideable to match our use of TableSetupScrollFreeze()
            ImGui::TableSetupColumn("One");
            ImGui::TableSetupColumn("Two");
            ImGui::TableSetupColumn("Three");
            ImGui::TableSetupColumn("Four");
            ImGui::TableSetupColumn("Five");
            ImGui::TableSetupColumn("Six");
            ImGui::TableHeadersRow();
            for (int row = 0; row < 20; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 7; column++)
                {
                    // Both TableNextColumn() and TableSetColumnIndex() return true when a column is visible or performing width measurement.
                    // Because here we know that:
                    // - A) all our columns are contributing the same to row height
                    // - B) column 0 is always visible,
                    // We only always submit this one column and can skip others.
                    // More advanced per-column clipping behaviors may benefit from polling the status flags via TableGetColumnFlags().
                    if (!ImGui::TableSetColumnIndex(column) && column > 0)
                        continue;
                    if (column == 0)
                        ImGui::Text("Line %d", row);
                    else
                        ImGui::Text("Hello world %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Stretch + ScrollX");
        ImGui::SameLine();
        HelpMarker(
            "Showcase using Stretch columns + ScrollX together: "
            "this is rather unusual and only makes sense when specifying an 'inner_width' for the table!\n"
            "Without an explicit value, inner_width is == outer_size.x and therefore using Stretch columns + ScrollX together doesn't make sense.");
        static ImGuiTableFlags flags2 = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg | ImGuiTableFlags_ContextMenuInBody;
        static float inner_width = 1000.0f;
        PushStyleCompact();
        ImGui::PushID("flags3");
        ImGui::PushItemWidth(TEXT_BASE_WIDTH * 30);
        ImGui::CheckboxFlags("ImGuiTableFlags_ScrollX", &flags2, ImGuiTableFlags_ScrollX);
        ImGui::DragFloat("inner_width", &inner_width, 1.0f, 0.0f, FLT_MAX, "%.1f");
        ImGui::PopItemWidth();
        ImGui::PopID();
        PopStyleCompact();
        if (ImGui::BeginTable("table2", 7, flags2, outer_size, inner_width))
        {
            for (int cell = 0; cell < 20 * 7; cell++)
            {
                ImGui::TableNextColumn();
                ImGui::Text("Hello world %d,%d", ImGui::TableGetColumnIndex(), ImGui::TableGetRowIndex());
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Columns flags");
    if (ImGui::TreeNode("Columns flags"))
    {
        // Create a first table just to show all the options/flags we want to make visible in our example!
        const int column_count = 3;
        const char* column_names[column_count] = { "One", "Two", "Three" };
        static ImGuiTableColumnFlags column_flags[column_count] = { ImGuiTableColumnFlags_DefaultSort, ImGuiTableColumnFlags_None, ImGuiTableColumnFlags_DefaultHide };
        static ImGuiTableColumnFlags column_flags_out[column_count] = { 0, 0, 0 }; // Output from TableGetColumnFlags()

        if (ImGui::BeginTable("table_columns_flags_checkboxes", column_count, ImGuiTableFlags_None))
        {
            PushStyleCompact();
            for (int column = 0; column < column_count; column++)
            {
                ImGui::TableNextColumn();
                ImGui::PushID(column);
                ImGui::AlignTextToFramePadding(); // FIXME-TABLE: Workaround for wrong text baseline propagation across columns
                ImGui::Text("'%s'", column_names[column]);
                ImGui::Spacing();
                ImGui::Text("Input flags:");
                EditTableColumnsFlags(&column_flags[column]);
                ImGui::Spacing();
                ImGui::Text("Output flags:");
                ImGui::BeginDisabled();
                ShowTableColumnsStatusFlags(column_flags_out[column]);
                ImGui::EndDisabled();
                ImGui::PopID();
            }
            PopStyleCompact();
            ImGui::EndTable();
        }

        // Create the real table we care about for the example!
        // We use a scrolling table to be able to showcase the difference between the _IsEnabled and _IsVisible flags above, otherwise in
        // a non-scrolling table columns are always visible (unless using ImGuiTableFlags_NoKeepColumnsVisible + resizing the parent window down)
        const ImGuiTableFlags flags
            = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV
            | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable;
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 9);
        if (ImGui::BeginTable("table_columns_flags", column_count, flags, outer_size))
        {
            for (int column = 0; column < column_count; column++)
                ImGui::TableSetupColumn(column_names[column], column_flags[column]);
            ImGui::TableHeadersRow();
            for (int column = 0; column < column_count; column++)
                column_flags_out[column] = ImGui::TableGetColumnFlags(column);
            float indent_step = (float)((int)TEXT_BASE_WIDTH / 2);
            for (int row = 0; row < 8; row++)
            {
                ImGui::Indent(indent_step); // Add some indentation to demonstrate usage of per-column IndentEnable/IndentDisable flags.
                ImGui::TableNextRow();
                for (int column = 0; column < column_count; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("%s %s", (column == 0) ? "Indented" : "Hello", ImGui::TableGetColumnName(column));
                }
            }
            ImGui::Unindent(indent_step * 8.0f);

            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Columns widths");
    if (ImGui::TreeNode("Columns widths"))
    {
        HelpMarker("Using TableSetupColumn() to setup default width.");

        static ImGuiTableFlags flags1 = ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBodyUntilResize;
        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags1, ImGuiTableFlags_Resizable);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBodyUntilResize", &flags1, ImGuiTableFlags_NoBordersInBodyUntilResize);
        PopStyleCompact();
        if (ImGui::BeginTable("table1", 3, flags1))
        {
            // We could also set ImGuiTableFlags_SizingFixedFit on the table and all columns will default to ImGuiTableColumnFlags_WidthFixed.
            ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
            ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed, 200.0f); // Default to 200.0f
            ImGui::TableSetupColumn("three", ImGuiTableColumnFlags_WidthFixed);       // Default to auto
            ImGui::TableHeadersRow();
            for (int row = 0; row < 4; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    if (row == 0)
                        ImGui::Text("(w: %5.1f)", ImGui::GetContentRegionAvail().x);
                    else
                        ImGui::Text("Hello %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }

        HelpMarker("Using TableSetupColumn() to setup explicit width.\n\nUnless _NoKeepColumnsVisible is set, fixed columns with set width may still be shrunk down if there's not enough space in the host.");

        static ImGuiTableFlags flags2 = ImGuiTableFlags_None;
        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_NoKeepColumnsVisible", &flags2, ImGuiTableFlags_NoKeepColumnsVisible);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersInnerV", &flags2, ImGuiTableFlags_BordersInnerV);
        ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuterV", &flags2, ImGuiTableFlags_BordersOuterV);
        PopStyleCompact();
        if (ImGui::BeginTable("table2", 4, flags2))
        {
            // We could also set ImGuiTableFlags_SizingFixedFit on the table and all columns will default to ImGuiTableColumnFlags_WidthFixed.
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 30.0f);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 4; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    if (row == 0)
                        ImGui::Text("(w: %5.1f)", ImGui::GetContentRegionAvail().x);
                    else
                        ImGui::Text("Hello %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Nested tables");
    if (ImGui::TreeNode("Nested tables"))
    {
        HelpMarker("This demonstrate embedding a table into another table cell.");

        if (ImGui::BeginTable("table_nested1", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
        {
            ImGui::TableSetupColumn("A0");
            ImGui::TableSetupColumn("A1");
            ImGui::TableHeadersRow();

            ImGui::TableNextColumn();
            ImGui::Text("A0 Row 0");
            {
                float rows_height = TEXT_BASE_HEIGHT * 2;
                if (ImGui::BeginTable("table_nested2", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
                {
                    ImGui::TableSetupColumn("B0");
                    ImGui::TableSetupColumn("B1");
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow(ImGuiTableRowFlags_None, rows_height);
                    ImGui::TableNextColumn();
                    ImGui::Text("B0 Row 0");
                    ImGui::TableNextColumn();
                    ImGui::Text("B1 Row 0");
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, rows_height);
                    ImGui::TableNextColumn();
                    ImGui::Text("B0 Row 1");
                    ImGui::TableNextColumn();
                    ImGui::Text("B1 Row 1");

                    ImGui::EndTable();
                }
            }
            ImGui::TableNextColumn(); ImGui::Text("A1 Row 0");
            ImGui::TableNextColumn(); ImGui::Text("A0 Row 1");
            ImGui::TableNextColumn(); ImGui::Text("A1 Row 1");
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Row height");
    if (ImGui::TreeNode("Row height"))
    {
        HelpMarker("You can pass a 'min_row_height' to TableNextRow().\n\nRows are padded with 'style.CellPadding.y' on top and bottom, so effectively the minimum row height will always be >= 'style.CellPadding.y * 2.0f'.\n\nWe cannot honor a _maximum_ row height as that would requires a unique clipping rectangle per row.");
        if (ImGui::BeginTable("table_row_height", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV))
        {
            for (int row = 0; row < 10; row++)
            {
                float min_row_height = (float)(int)(TEXT_BASE_HEIGHT * 0.30f * row);
                ImGui::TableNextRow(ImGuiTableRowFlags_None, min_row_height);
                ImGui::TableNextColumn();
                ImGui::Text("min_row_height = %.2f", min_row_height);
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Outer size");
    if (ImGui::TreeNode("Outer size"))
    {
        // Showcasing use of ImGuiTableFlags_NoHostExtendX and ImGuiTableFlags_NoHostExtendY
        // Important to that note how the two flags have slightly different behaviors!
        ImGui::Text("Using NoHostExtendX and NoHostExtendY:");
        PushStyleCompact();
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
        ImGui::CheckboxFlags("ImGuiTableFlags_NoHostExtendX", &flags, ImGuiTableFlags_NoHostExtendX);
        ImGui::SameLine(); HelpMarker("Make outer width auto-fit to columns, overriding outer_size.x value.\n\nOnly available when ScrollX/ScrollY are disabled and Stretch columns are not used.");
        ImGui::CheckboxFlags("ImGuiTableFlags_NoHostExtendY", &flags, ImGuiTableFlags_NoHostExtendY);
        ImGui::SameLine(); HelpMarker("Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit).\n\nOnly available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.");
        PopStyleCompact();

        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5.5f);
        if (ImGui::BeginTable("table1", 3, flags, outer_size))
        {
            for (int row = 0; row < 10; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("Cell %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::SameLine();
        ImGui::Text("Hello!");

        ImGui::Spacing();

        ImGui::Text("Using explicit size:");
        if (ImGui::BeginTable("table2", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(TEXT_BASE_WIDTH * 30, 0.0f)))
        {
            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("Cell %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::SameLine();
        if (ImGui::BeginTable("table3", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(TEXT_BASE_WIDTH * 30, 0.0f)))
        {
            for (int row = 0; row < 3; row++)
            {
                ImGui::TableNextRow(0, TEXT_BASE_HEIGHT * 1.5f);
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("Cell %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Background color");
    if (ImGui::TreeNode("Background color"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
        static int row_bg_type = 1;
        static int row_bg_target = 1;
        static int cell_bg_type = 1;

        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_Borders", &flags, ImGuiTableFlags_Borders);
        ImGui::CheckboxFlags("ImGuiTableFlags_RowBg", &flags, ImGuiTableFlags_RowBg);
        ImGui::SameLine(); HelpMarker("ImGuiTableFlags_RowBg automatically sets RowBg0 to alternative colors pulled from the Style.");
        ImGui::Combo("row bg type", (int*)&row_bg_type, "None\0Red\0Gradient\0");
        ImGui::Combo("row bg target", (int*)&row_bg_target, "RowBg0\0RowBg1\0"); ImGui::SameLine(); HelpMarker("Target RowBg0 to override the alternating odd/even colors,\nTarget RowBg1 to blend with them.");
        ImGui::Combo("cell bg type", (int*)&cell_bg_type, "None\0Blue\0"); ImGui::SameLine(); HelpMarker("We are colorizing cells to B1->C2 here.");
        IM_ASSERT(row_bg_type >= 0 && row_bg_type <= 2);
        IM_ASSERT(row_bg_target >= 0 && row_bg_target <= 1);
        IM_ASSERT(cell_bg_type >= 0 && cell_bg_type <= 1);
        PopStyleCompact();

        if (ImGui::BeginTable("table1", 5, flags))
        {
            for (int row = 0; row < 6; row++)
            {
                ImGui::TableNextRow();

                // Demonstrate setting a row background color with 'ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBgX, ...)'
                // We use a transparent color so we can see the one behind in case our target is RowBg1 and RowBg0 was already targeted by the ImGuiTableFlags_RowBg flag.
                if (row_bg_type != 0)
                {
                    ImU32 row_bg_color = ImGui::GetColorU32(row_bg_type == 1 ? ImVec4(0.7f, 0.3f, 0.3f, 0.65f) : ImVec4(0.2f + row * 0.1f, 0.2f, 0.2f, 0.65f)); // Flat or Gradient?
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0 + row_bg_target, row_bg_color);
                }

                // Fill cells
                for (int column = 0; column < 5; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("%c%c", 'A' + row, '0' + column);

                    // Change background of Cells B1->C2
                    // Demonstrate setting a cell background color with 'ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ...)'
                    // (the CellBg color will be blended over the RowBg and ColumnBg colors)
                    // We can also pass a column number as a third parameter to TableSetBgColor() and do this outside the column loop.
                    if (row >= 1 && row <= 2 && column >= 1 && column <= 2 && cell_bg_type == 1)
                    {
                        ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                    }
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Tree view");
    if (ImGui::TreeNode("Tree view"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

        if (ImGui::BeginTable("3ways", 3, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableHeadersRow();

            // Simple storage to output a dummy file-system.
            struct MyTreeNode
            {
                const char* Name;
                const char* Type;
                int             Size;
                int             ChildIdx;
                int             ChildCount;
                static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    const bool is_folder = (node->ChildCount > 0);
                    if (is_folder)
                    {
                        bool open = ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_SpanFullWidth);
                        ImGui::TableNextColumn();
                        ImGui::TextDisabled("--");
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(node->Type);
                        if (open)
                        {
                            for (int child_n = 0; child_n < node->ChildCount; child_n++)
                                DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", node->Size);
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(node->Type);
                    }
                }
            };
            static const MyTreeNode nodes[] =
            {
                { "Root",                         "Folder",       -1,       1, 3    }, // 0
                { "Music",                        "Folder",       -1,       4, 2    }, // 1
                { "Textures",                     "Folder",       -1,       6, 3    }, // 2
                { "desktop.ini",                  "System file",  1024,    -1,-1    }, // 3
                { "File1_a.wav",                  "Audio file",   123000,  -1,-1    }, // 4
                { "File1_b.wav",                  "Audio file",   456000,  -1,-1    }, // 5
                { "Image001.png",                 "Image file",   203128,  -1,-1    }, // 6
                { "Copy of Image001.png",         "Image file",   203256,  -1,-1    }, // 7
                { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
            };

            MyTreeNode::DisplayNode(&nodes[0], nodes);

            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Item width");
    if (ImGui::TreeNode("Item width"))
    {
        HelpMarker(
            "Showcase using PushItemWidth() and how it is preserved on a per-column basis.\n\n"
            "Note that on auto-resizing non-resizable fixed columns, querying the content width for e.g. right-alignment doesn't make sense.");
        if (ImGui::BeginTable("table_item_width", 3, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("small");
            ImGui::TableSetupColumn("half");
            ImGui::TableSetupColumn("right-align");
            ImGui::TableHeadersRow();

            for (int row = 0; row < 3; row++)
            {
                ImGui::TableNextRow();
                if (row == 0)
                {
                    // Setup ItemWidth once (instead of setting up every time, which is also possible but less efficient)
                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushItemWidth(TEXT_BASE_WIDTH * 3.0f); // Small
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::PushItemWidth(-FLT_MIN); // Right-aligned
                }

                // Draw our contents
                static float dummy_f = 0.0f;
                ImGui::PushID(row);
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("float0", &dummy_f, 0.0f, 1.0f);
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("float1", &dummy_f, 0.0f, 1.0f);
                ImGui::TableSetColumnIndex(2);
                ImGui::SliderFloat("float2", &dummy_f, 0.0f, 1.0f);
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    // Demonstrate using TableHeader() calls instead of TableHeadersRow()
    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Custom headers");
    if (ImGui::TreeNode("Custom headers"))
    {
        const int COLUMNS_COUNT = 3;
        if (ImGui::BeginTable("table_custom_headers", COLUMNS_COUNT, ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
        {
            ImGui::TableSetupColumn("Apricot");
            ImGui::TableSetupColumn("Banana");
            ImGui::TableSetupColumn("Cherry");

            // Dummy entire-column selection storage
            // FIXME: It would be nice to actually demonstrate full-featured selection using those checkbox.
            static bool column_selected[3] = {};

            // Instead of calling TableHeadersRow() we'll submit custom headers ourselves
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            for (int column = 0; column < COLUMNS_COUNT; column++)
            {
                ImGui::TableSetColumnIndex(column);
                const char* column_name = ImGui::TableGetColumnName(column); // Retrieve name passed to TableSetupColumn()
                ImGui::PushID(column);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                ImGui::Checkbox("##checkall", &column_selected[column]);
                ImGui::PopStyleVar();
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::TableHeader(column_name);
                ImGui::PopID();
            }

            for (int row = 0; row < 5; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    char buf[32];
                    sprintf_s(buf, "Cell %d,%d", column, row);
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Selectable(buf, column_selected[column]);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    // Demonstrate creating custom context menus inside columns, while playing it nice with context menus provided by TableHeadersRow()/TableHeader()
    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
   //  // IMGUI_DEMO_MARKER("Tables/Context menus");
    if (ImGui::TreeNode("Context menus"))
    {
        HelpMarker("By default, right-clicking over a TableHeadersRow()/TableHeader() line will open the default context-menu.\nUsing ImGuiTableFlags_ContextMenuInBody we also allow right-clicking over columns body.");
        static ImGuiTableFlags flags1 = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_ContextMenuInBody;

        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_ContextMenuInBody", &flags1, ImGuiTableFlags_ContextMenuInBody);
        PopStyleCompact();

        // Context Menus: first example
        // [1.1] Right-click on the TableHeadersRow() line to open the default table context menu.
        // [1.2] Right-click in columns also open the default table context menu (if ImGuiTableFlags_ContextMenuInBody is set)
        const int COLUMNS_COUNT = 3;
        if (ImGui::BeginTable("table_context_menu", COLUMNS_COUNT, flags1))
        {
            ImGui::TableSetupColumn("One");
            ImGui::TableSetupColumn("Two");
            ImGui::TableSetupColumn("Three");

            // [1.1]] Right-click on the TableHeadersRow() line to open the default table context menu.
            ImGui::TableHeadersRow();

            // Submit dummy contents
            for (int row = 0; row < 4; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < COLUMNS_COUNT; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Cell %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }

        // Context Menus: second example
        // [2.1] Right-click on the TableHeadersRow() line to open the default table context menu.
        // [2.2] Right-click on the ".." to open a custom popup
        // [2.3] Right-click in columns to open another custom popup
        HelpMarker("Demonstrate mixing table context menu (over header), item context button (over button) and custom per-colum context menu (over column body).");
        ImGuiTableFlags flags2 = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders;
        if (ImGui::BeginTable("table_context_menu_2", COLUMNS_COUNT, flags2))
        {
            ImGui::TableSetupColumn("One");
            ImGui::TableSetupColumn("Two");
            ImGui::TableSetupColumn("Three");

            // [2.1] Right-click on the TableHeadersRow() line to open the default table context menu.
            ImGui::TableHeadersRow();
            for (int row = 0; row < 4; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < COLUMNS_COUNT; column++)
                {
                    // Submit dummy contents
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Cell %d,%d", column, row);
                    ImGui::SameLine();

                    // [2.2] Right-click on the ".." to open a custom popup
                    ImGui::PushID(row * COLUMNS_COUNT + column);
                    ImGui::SmallButton("..");
                    if (ImGui::BeginPopupContextItem())
                    {
                        ImGui::Text("This is the popup for Button(\"..\") in Cell %d,%d", column, row);
                        if (ImGui::Button("Close"))
                            ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
            }

            // [2.3] Right-click anywhere in columns to open another custom popup
            // (instead of testing for !IsAnyItemHovered() we could also call OpenPopup() with ImGuiPopupFlags_NoOpenOverExistingPopup
            // to manage popup priority as the popups triggers, here "are we hovering a column" are overlapping)
            int hovered_column = -1;
            for (int column = 0; column < COLUMNS_COUNT + 1; column++)
            {
                ImGui::PushID(column);
                if (ImGui::TableGetColumnFlags(column) & ImGuiTableColumnFlags_IsHovered)
                    hovered_column = column;
                if (hovered_column == column && !ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(1))
                    ImGui::OpenPopup("MyPopup");
                if (ImGui::BeginPopup("MyPopup"))
                {
                    if (column == COLUMNS_COUNT)
                        ImGui::Text("This is a custom popup for unused space after the last column.");
                    else
                        ImGui::Text("This is a custom popup for Column %d", column);
                    if (ImGui::Button("Close"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }

            ImGui::EndTable();
            ImGui::Text("Hovered column: %d", hovered_column);
        }
        ImGui::TreePop();
    }

    // Demonstrate creating multiple tables with the same ID
    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // // IMGUI_DEMO_MARKER("Tables/Synced instances");
    if (ImGui::TreeNode("Synced instances"))
    {
        HelpMarker("Multiple tables with the same identifier will share their settings, width, visibility, order etc.");
        for (int n = 0; n < 3; n++)
        {
            char buf[32];
            sprintf_s(buf, "Synced Table %d", n);
            bool open = ImGui::CollapsingHeader(buf, ImGuiTreeNodeFlags_DefaultOpen);
            if (open && ImGui::BeginTable("Table", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings))
            {
                ImGui::TableSetupColumn("One");
                ImGui::TableSetupColumn("Two");
                ImGui::TableSetupColumn("Three");
                ImGui::TableHeadersRow();
                for (int cell = 0; cell < 9; cell++)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("this cell %d", cell);
                }
                ImGui::EndTable();
            }
        }
        ImGui::TreePop();
    }

    // Demonstrate using Sorting facilities
    // This is a simplified version of the "Advanced" example, where we mostly focus on the code necessary to handle sorting.
    // Note that the "Advanced" example also showcase manually triggering a sort (e.g. if item quantities have been modified)
    static const char* template_items_names[] =
    {
        "Banana", "Apple", "Cherry", "Watermelon", "Grapefruit", "Strawberry", "Mango",
        "Kiwi", "Orange", "Pineapple", "Blueberry", "Plum", "Coconut", "Pear", "Apricot"
    };
    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // IMGUI_DEMO_MARKER("Tables/Sorting");
    if (ImGui::TreeNode("Sorting"))
    {
        // Create item list
        static ImVector<MyItem> items;
        if (items.Size == 0)
        {
            items.resize(50, MyItem());
            for (int n = 0; n < items.Size; n++)
            {
                const int template_n = n % IM_ARRAYSIZE(template_items_names);
                MyItem& item = items[n];
                item.ID = n;
                item.Name = template_items_names[template_n];
                item.Quantity = (n * n - n) % 20; // Assign default quantities
            }
        }

        // Options
        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollY;
        PushStyleCompact();
        ImGui::CheckboxFlags("ImGuiTableFlags_SortMulti", &flags, ImGuiTableFlags_SortMulti);
        ImGui::SameLine(); HelpMarker("When sorting is enabled: hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).");
        ImGui::CheckboxFlags("ImGuiTableFlags_SortTristate", &flags, ImGuiTableFlags_SortTristate);
        ImGui::SameLine(); HelpMarker("When sorting is enabled: allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).");
        PopStyleCompact();

        if (ImGui::BeginTable("table_sorting", 4, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            // Demonstrate using a mixture of flags among available sort-related flags:
            // - ImGuiTableColumnFlags_DefaultSort
            // - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
            // - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_ID);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
            ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Quantity);
            ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
            ImGui::TableHeadersRow();

            // Sort our data if sort specs have been changed!
            if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
                if (sorts_specs->SpecsDirty)
                {
                    MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                    if (items.Size > 1)
                        qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                    MyItem::s_current_sort_specs = NULL;
                    sorts_specs->SpecsDirty = false;
                }

            // Demonstrate using clipper for large vertical lists
            ImGuiListClipper clipper;
            clipper.Begin(items.Size);
            while (clipper.Step())
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    // Display a data item
                    MyItem* item = &items[row_n];
                    ImGui::PushID(item->ID);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%04d", item->ID);
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(item->Name);
                    ImGui::TableNextColumn();
                    ImGui::SmallButton("None");
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", item->Quantity);
                    ImGui::PopID();
                }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    // In this example we'll expose most table flags and settings.
    // For specific flags and settings refer to the corresponding section for more detailed explanation.
    // This section is mostly useful to experiment with combining certain flags or settings with each others.
    //ImGui::SetNextItemOpen(true, ImGuiCond_Once); // [DEBUG]
    if (open_action != -1)
        ImGui::SetNextItemOpen(open_action != 0);
    // IMGUI_DEMO_MARKER("Tables/Advanced");
    if (ImGui::TreeNode("Advanced"))
    {
        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_SizingFixedFit;

        enum ContentsType { CT_Text, CT_Button, CT_SmallButton, CT_FillButton, CT_Selectable, CT_SelectableSpanRow };
        static int contents_type = CT_SelectableSpanRow;
        const char* contents_type_names[] = { "Text", "Button", "SmallButton", "FillButton", "Selectable", "Selectable (span row)" };
        static int freeze_cols = 1;
        static int freeze_rows = 1;
        static int items_count = IM_ARRAYSIZE(template_items_names) * 2;
        static ImVec2 outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 12);
        static float row_min_height = 0.0f; // Auto
        static float inner_width_with_scroll = 0.0f; // Auto-extend
        static bool outer_size_enabled = true;
        static bool show_headers = true;
        static bool show_wrapped_text = false;
        //static ImGuiTextFilter filter;
        //ImGui::SetNextItemOpen(true, ImGuiCond_Once); // FIXME-TABLE: Enabling this results in initial clipped first pass on table which tend to affects column sizing
        if (ImGui::TreeNode("Options"))
        {
            // Make the UI compact because there are so many fields
            PushStyleCompact();
            ImGui::PushItemWidth(TEXT_BASE_WIDTH * 28.0f);

            if (ImGui::TreeNodeEx("Features:", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags, ImGuiTableFlags_Resizable);
                ImGui::CheckboxFlags("ImGuiTableFlags_Reorderable", &flags, ImGuiTableFlags_Reorderable);
                ImGui::CheckboxFlags("ImGuiTableFlags_Hideable", &flags, ImGuiTableFlags_Hideable);
                ImGui::CheckboxFlags("ImGuiTableFlags_Sortable", &flags, ImGuiTableFlags_Sortable);
                ImGui::CheckboxFlags("ImGuiTableFlags_NoSavedSettings", &flags, ImGuiTableFlags_NoSavedSettings);
                ImGui::CheckboxFlags("ImGuiTableFlags_ContextMenuInBody", &flags, ImGuiTableFlags_ContextMenuInBody);
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Decorations:", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::CheckboxFlags("ImGuiTableFlags_RowBg", &flags, ImGuiTableFlags_RowBg);
                ImGui::CheckboxFlags("ImGuiTableFlags_BordersV", &flags, ImGuiTableFlags_BordersV);
                ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuterV", &flags, ImGuiTableFlags_BordersOuterV);
                ImGui::CheckboxFlags("ImGuiTableFlags_BordersInnerV", &flags, ImGuiTableFlags_BordersInnerV);
                ImGui::CheckboxFlags("ImGuiTableFlags_BordersH", &flags, ImGuiTableFlags_BordersH);
                ImGui::CheckboxFlags("ImGuiTableFlags_BordersOuterH", &flags, ImGuiTableFlags_BordersOuterH);
                ImGui::CheckboxFlags("ImGuiTableFlags_BordersInnerH", &flags, ImGuiTableFlags_BordersInnerH);
                ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBody", &flags, ImGuiTableFlags_NoBordersInBody); ImGui::SameLine(); HelpMarker("Disable vertical borders in columns Body (borders will always appears in Headers");
                ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBodyUntilResize", &flags, ImGuiTableFlags_NoBordersInBodyUntilResize); ImGui::SameLine(); HelpMarker("Disable vertical borders in columns Body until hovered for resize (borders will always appears in Headers)");
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Sizing:", ImGuiTreeNodeFlags_DefaultOpen))
            {
                EditTableSizingFlags(&flags);
                ImGui::SameLine(); HelpMarker("In the Advanced demo we override the policy of each column so those table-wide settings have less effect that typical.");
                ImGui::CheckboxFlags("ImGuiTableFlags_NoHostExtendX", &flags, ImGuiTableFlags_NoHostExtendX);
                ImGui::SameLine(); HelpMarker("Make outer width auto-fit to columns, overriding outer_size.x value.\n\nOnly available when ScrollX/ScrollY are disabled and Stretch columns are not used.");
                ImGui::CheckboxFlags("ImGuiTableFlags_NoHostExtendY", &flags, ImGuiTableFlags_NoHostExtendY);
                ImGui::SameLine(); HelpMarker("Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit).\n\nOnly available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.");
                ImGui::CheckboxFlags("ImGuiTableFlags_NoKeepColumnsVisible", &flags, ImGuiTableFlags_NoKeepColumnsVisible);
                ImGui::SameLine(); HelpMarker("Only available if ScrollX is disabled.");
                ImGui::CheckboxFlags("ImGuiTableFlags_PreciseWidths", &flags, ImGuiTableFlags_PreciseWidths);
                ImGui::SameLine(); HelpMarker("Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.");
                ImGui::CheckboxFlags("ImGuiTableFlags_NoClip", &flags, ImGuiTableFlags_NoClip);
                ImGui::SameLine(); HelpMarker("Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with ScrollFreeze options.");
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Padding:", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::CheckboxFlags("ImGuiTableFlags_PadOuterX", &flags, ImGuiTableFlags_PadOuterX);
                ImGui::CheckboxFlags("ImGuiTableFlags_NoPadOuterX", &flags, ImGuiTableFlags_NoPadOuterX);
                ImGui::CheckboxFlags("ImGuiTableFlags_NoPadInnerX", &flags, ImGuiTableFlags_NoPadInnerX);
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Scrolling:", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::CheckboxFlags("ImGuiTableFlags_ScrollX", &flags, ImGuiTableFlags_ScrollX);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetFrameHeight());
                ImGui::DragInt("freeze_cols", &freeze_cols, 0.2f, 0, 9, NULL, ImGuiSliderFlags_NoInput);
                ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &flags, ImGuiTableFlags_ScrollY);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetFrameHeight());
                ImGui::DragInt("freeze_rows", &freeze_rows, 0.2f, 0, 9, NULL, ImGuiSliderFlags_NoInput);
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Sorting:", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::CheckboxFlags("ImGuiTableFlags_SortMulti", &flags, ImGuiTableFlags_SortMulti);
                ImGui::SameLine(); HelpMarker("When sorting is enabled: hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).");
                ImGui::CheckboxFlags("ImGuiTableFlags_SortTristate", &flags, ImGuiTableFlags_SortTristate);
                ImGui::SameLine(); HelpMarker("When sorting is enabled: allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).");
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Other:", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("show_headers", &show_headers);
                ImGui::Checkbox("show_wrapped_text", &show_wrapped_text);

                ImGui::DragFloat2("##OuterSize", &outer_size_value.x);
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::Checkbox("outer_size", &outer_size_enabled);
                ImGui::SameLine();
                HelpMarker("If scrolling is disabled (ScrollX and ScrollY not set):\n"
                    "- The table is output directly in the parent window.\n"
                    "- OuterSize.x < 0.0f will right-align the table.\n"
                    "- OuterSize.x = 0.0f will narrow fit the table unless there are any Stretch column.\n"
                    "- OuterSize.y then becomes the minimum size for the table, which will extend vertically if there are more rows (unless NoHostExtendY is set).");

                // From a user point of view we will tend to use 'inner_width' differently depending on whether our table is embedding scrolling.
                // To facilitate toying with this demo we will actually pass 0.0f to the BeginTable() when ScrollX is disabled.
                ImGui::DragFloat("inner_width (when ScrollX active)", &inner_width_with_scroll, 1.0f, 0.0f, FLT_MAX);

                ImGui::DragFloat("row_min_height", &row_min_height, 1.0f, 0.0f, FLT_MAX);
                ImGui::SameLine(); HelpMarker("Specify height of the Selectable item.");

                ImGui::DragInt("items_count", &items_count, 0.1f, 0, 9999);
                ImGui::Combo("items_type (first column)", &contents_type, contents_type_names, IM_ARRAYSIZE(contents_type_names));
                //filter.Draw("filter");
                ImGui::TreePop();
            }

            ImGui::PopItemWidth();
            PopStyleCompact();
            ImGui::Spacing();
            ImGui::TreePop();
        }

        // Update item list if we changed the number of items
        static ImVector<MyItem> items;
        static ImVector<int> selection;
        static bool items_need_sort = false;
        if (items.Size != items_count)
        {
            items.resize(items_count, MyItem());
            for (int n = 0; n < items_count; n++)
            {
                const int template_n = n % IM_ARRAYSIZE(template_items_names);
                MyItem& item = items[n];
                item.ID = n;
                item.Name = template_items_names[template_n];
                item.Quantity = (template_n == 3) ? 10 : (template_n == 4) ? 20 : 0; // Assign default quantities
            }
        }

        const ImDrawList* parent_draw_list = ImGui::GetWindowDrawList();
        const int parent_draw_list_draw_cmd_count = parent_draw_list->CmdBuffer.Size;
        ImVec2 table_scroll_cur, table_scroll_max; // For debug display
        const ImDrawList* table_draw_list = NULL;  // "

        // Submit table
        const float inner_width_to_use = (flags & ImGuiTableFlags_ScrollX) ? inner_width_with_scroll : 0.0f;
        if (ImGui::BeginTable("table_advanced", 6, flags, outer_size_enabled ? outer_size_value : ImVec2(0, 0), inner_width_to_use))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, MyItemColumnID_ID);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
            ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_PreferSortDescending, 0.0f, MyItemColumnID_Quantity);
            ImGui::TableSetupColumn("Description", (flags & ImGuiTableFlags_NoHostExtendX) ? 0 : ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Description);
            ImGui::TableSetupColumn("Hidden", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);

            // Sort our data if sort specs have been changed!
            ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs();
            if (sorts_specs && sorts_specs->SpecsDirty)
                items_need_sort = true;
            if (sorts_specs && items_need_sort && items.Size > 1)
            {
                MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                MyItem::s_current_sort_specs = NULL;
                sorts_specs->SpecsDirty = false;
            }
            items_need_sort = false;

            // Take note of whether we are currently sorting based on the Quantity field,
            // we will use this to trigger sorting when we know the data of this column has been modified.
            const bool sorts_specs_using_quantity = (ImGui::TableGetColumnFlags(3) & ImGuiTableColumnFlags_IsSorted) != 0;

            // Show headers
            if (show_headers)
                ImGui::TableHeadersRow();

            // Show data
            // FIXME-TABLE FIXME-NAV: How we can get decent up/down even though we have the buttons here?
            ImGui::PushButtonRepeat(true);
#if 1
            // Demonstrate using clipper for large vertical lists
            ImGuiListClipper clipper;
            clipper.Begin(items.Size);
            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
#else
            // Without clipper
                {
                    for (int row_n = 0; row_n < items.Size; row_n++)
#endif
                    {
                        MyItem* item = &items[row_n];
                        //if (!filter.PassFilter(item->Name))
                        //    continue;

                        const bool item_is_selected = selection.contains(item->ID);
                        ImGui::PushID(item->ID);
                        ImGui::TableNextRow(ImGuiTableRowFlags_None, row_min_height);

                        // For the demo purpose we can select among different type of items submitted in the first column
                        ImGui::TableSetColumnIndex(0);
                        char label[32];
                        sprintf_s(label, "%04d", item->ID);
                        if (contents_type == CT_Text)
                            ImGui::TextUnformatted(label);
                        else if (contents_type == CT_Button)
                            ImGui::Button(label);
                        else if (contents_type == CT_SmallButton)
                            ImGui::SmallButton(label);
                        else if (contents_type == CT_FillButton)
                            ImGui::Button(label, ImVec2(-FLT_MIN, 0.0f));
                        else if (contents_type == CT_Selectable || contents_type == CT_SelectableSpanRow)
                        {
                            ImGuiSelectableFlags selectable_flags = (contents_type == CT_SelectableSpanRow) ? ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap : ImGuiSelectableFlags_None;
                            if (ImGui::Selectable(label, item_is_selected, selectable_flags, ImVec2(0, row_min_height)))
                            {
                                if (ImGui::GetIO().KeyCtrl)
                                {
                                    if (item_is_selected)
                                        selection.find_erase_unsorted(item->ID);
                                    else
                                        selection.push_back(item->ID);
                                }
                                else
                                {
                                    selection.clear();
                                    selection.push_back(item->ID);
                                }
                            }
                        }

                        if (ImGui::TableSetColumnIndex(1))
                            ImGui::TextUnformatted(item->Name);

                        // Here we demonstrate marking our data set as needing to be sorted again if we modified a quantity,
                        // and we are currently sorting on the column showing the Quantity.
                        // To avoid triggering a sort while holding the button, we only trigger it when the button has been released.
                        // You will probably need a more advanced system in your code if you want to automatically sort when a specific entry changes.
                        if (ImGui::TableSetColumnIndex(2))
                        {
                            if (ImGui::SmallButton("Chop")) { item->Quantity += 1; }
                            if (sorts_specs_using_quantity && ImGui::IsItemDeactivated()) { items_need_sort = true; }
                            ImGui::SameLine();
                            if (ImGui::SmallButton("Eat")) { item->Quantity -= 1; }
                            if (sorts_specs_using_quantity && ImGui::IsItemDeactivated()) { items_need_sort = true; }
                        }

                        if (ImGui::TableSetColumnIndex(3))
                            ImGui::Text("%d", item->Quantity);

                        ImGui::TableSetColumnIndex(4);
                        if (show_wrapped_text)
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet");
                        else
                            ImGui::Text("Lorem ipsum dolor sit amet");

                        if (ImGui::TableSetColumnIndex(5))
                            ImGui::Text("1234");

                        ImGui::PopID();
                    }
                }
                ImGui::PopButtonRepeat();

                // Store some info to display debug details below
                table_scroll_cur = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
                table_scroll_max = ImVec2(ImGui::GetScrollMaxX(), ImGui::GetScrollMaxY());
                table_draw_list = ImGui::GetWindowDrawList();
                ImGui::EndTable();
            }
        static bool show_debug_details = false;
        ImGui::Checkbox("Debug details", &show_debug_details);
        if (show_debug_details && table_draw_list)
        {
            ImGui::SameLine(0.0f, 0.0f);
            const int table_draw_list_draw_cmd_count = table_draw_list->CmdBuffer.Size;
            if (table_draw_list == parent_draw_list)
                ImGui::Text(": DrawCmd: +%d (in same window)",
                    table_draw_list_draw_cmd_count - parent_draw_list_draw_cmd_count);
            else
                ImGui::Text(": DrawCmd: +%d (in child window), Scroll: (%.f/%.f) (%.f/%.f)",
                    table_draw_list_draw_cmd_count - 1, table_scroll_cur.x, table_scroll_max.x, table_scroll_cur.y, table_scroll_max.y);
        }
        ImGui::TreePop();
        }

    ImGui::PopID();

    ShowDemoWindowColumns();

    if (disable_indent)
        ImGui::PopStyleVar();
}
