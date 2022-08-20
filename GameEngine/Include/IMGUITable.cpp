#include "IMGUITable.h"

CIMGUITable::CIMGUITable() :
    m_MaxElemCnt(1) ,
    TEXT_BASE_HEIGHT(ImGui::GetTextLineHeightWithSpacing() * 20),
    m_TableHeight(TEXT_BASE_HEIGHT),
    m_TableWidth(0),
    m_TableTitle("Tables & Columns")
{
}

CIMGUITable::~CIMGUITable()
{
}

void CIMGUITable::AddData(const std::string& DataName, int Val)
{
    m_mapDataInfo[DataName].push_back(std::to_string(Val));

    size_t VecSize = m_mapDataInfo[DataName].size();
    if (VecSize > m_MaxElemCnt)
        m_MaxElemCnt = VecSize;
}

void CIMGUITable::AddData(const std::string& DataName, float Val)
{
    m_mapDataInfo[DataName].push_back(std::to_string(Val));

    size_t VecSize = m_mapDataInfo[DataName].size();
    if (VecSize > m_MaxElemCnt)
        m_MaxElemCnt = VecSize;
}

void CIMGUITable::AddData(const std::string& DataName, bool Val)
{
    m_mapDataInfo[DataName].push_back(Val ? "True" : "False");

    size_t VecSize = m_mapDataInfo[DataName].size();
    if (VecSize > m_MaxElemCnt)
        m_MaxElemCnt = VecSize;
}

void CIMGUITable::AddData(const std::string& DataName, const std::string& Val)
{
    m_mapDataInfo[DataName].push_back(Val);

    std::vector<std::string> Data = m_mapDataInfo[DataName];

    // size_t VecSize = m_mapDataInfo[DataName].size();
    size_t VecSize = Data.size();

    if (VecSize > m_MaxElemCnt)
        m_MaxElemCnt = VecSize;
}

void CIMGUITable::MakeKey(const std::string& DataName)
{
    if (m_mapDataInfo.find(DataName) != m_mapDataInfo.end())
        return;

    m_mapDataInfo[DataName] = std::vector<std::string>{};
}

void CIMGUITable::ClearAll()
{
    m_mapDataInfo.clear();
}

void CIMGUITable::ClearContents()
{
    auto iter = m_mapDataInfo.begin();
    auto iterEnd = m_mapDataInfo.end();

    for (; iter != iterEnd; ++iter)
    {
        iter->second.clear();
    }
}

bool CIMGUITable::Init()
{
    return true;
}

void CIMGUITable::Render()
{
    if (!ImGui::CollapsingHeader(m_TableTitle.c_str()))
        return;

    // Using those as a base value to create width/height that are factor of the size of our font
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    ImGui::PushID("Tables");

        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY 
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter 
            | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable 
            | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        PushStyleCompact();
        ImGui::CheckboxFlags("Limit Display", &flags, ImGuiTableFlags_ScrollY);
        PopStyleCompact();

        // When using ScrollX or ScrollY we need to specify a size for our table container!
        // Otherwise by default the table will fit all available space, like a BeginChild() call.
        ImVec2 outer_size = ImVec2(0.0f, m_TableHeight);

        // "table_scrolly" 는 해당 Table Widget 의 Key
        // - 하나의 Window 안에서, Table Widget 을 여러개 사용할 때
        // - 아래의 Table ID 는 달라야 한다.
        // m_MaxElemCnt + 1 을 해주는 이유는, Column 에 iter->first , 즉, Key 값도 들어가야 하기 때문이다.
        // if (ImGui::BeginTable("table_scrolly", (int)m_MaxElemCnt + 1, flags, outer_size))
        if (ImGui::BeginTable(m_TableTitle.c_str(), (int)m_MaxElemCnt + 1, flags, outer_size))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

            // 첫번째 행 --> 각 열의 제목 세팅
            ImGui::TableSetupColumn("Element Name", ImGuiTableColumnFlags_None);

            /*
            auto iter = m_mapDataInfo.begin();
            auto iterEnd = m_mapDataInfo.end();

            for (; iter != iterEnd; ++iter)
            {
                ImGui::TableSetupColumn(iter->first.c_str(), ImGuiTableColumnFlags_None);
            }
            */
           
            ImGui::TableHeadersRow();

            // Demonstrate using clipper for large vertical lists
            ImGuiListClipper clipper;

            // 행 개수
            clipper.Begin((int)m_mapDataInfo.size());

            auto iter = m_mapDataInfo.begin();
            auto iterEnd = m_mapDataInfo.end();

            while (clipper.Step())
            {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < m_MaxElemCnt + 1; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        // 첫번째 Column 은 Row 를 넣는다.
                        if (column == 0)
                            ImGui::TextColored(m_Color.Value, iter->first.c_str());
                        else if (iter->second.size() >= column)
                            ImGui::TextColored(m_Color.Value, iter->second[column - 1].c_str());
                        else
                            continue;
                    }
                    ++iter;
                }
            }
            ImGui::EndTable();
        }

       ImGui::PopID();

   }
