#include "IMGUITableElemList.h"

CIMGUITableElemList::CIMGUITableElemList() :
    TEXT_BASE_HEIGHT(ImGui::GetTextLineHeightWithSpacing() * 10),
    m_TableHeight(TEXT_BASE_HEIGHT),
    m_TableWidth(0),
    m_TableTitle("Tables & Columns")
{
}

CIMGUITableElemList::~CIMGUITableElemList()
{
}

void CIMGUITableElemList::AddData(const std::string& DataName, int Val)
{
    int KeyIdx = FindMapKeyIdxInVector(DataName);

    m_mapDataInfo[DataName].push_back(std::to_string(Val));

    if (KeyIdx == -1)
    {
        // 새롭게 Vector 를 확장한다.
        size_t CurrentColumnSize = m_vecDataInfo.size();
        m_vecDataInfo.push_back(std::vector<std::string>());
        m_vecDataInfo[CurrentColumnSize].push_back(std::to_string(Val));
    }
    else
    {
        m_vecDataInfo[KeyIdx].push_back(std::to_string(Val));
    }

    CalculateMaxElemCount();
}

void CIMGUITableElemList::AddData(const std::string& DataName, float Val)
{
    int KeyIdx = FindMapKeyIdxInVector(DataName);

    m_mapDataInfo[DataName].push_back(std::to_string(Val));

    if (KeyIdx == -1)
    {
        // 새롭게 Vector 를 확장한다.
        size_t CurrentColumnSize = m_vecDataInfo.size();
        m_vecDataInfo.push_back(std::vector<std::string>());
        m_vecDataInfo[CurrentColumnSize].push_back(std::to_string(Val));
    }
    else
    {
        m_vecDataInfo[KeyIdx].push_back(std::to_string(Val));
    }
    CalculateMaxElemCount();
}

void CIMGUITableElemList::AddData(const std::string& DataName, bool Val)
{
    int KeyIdx = FindMapKeyIdxInVector(DataName);

    m_mapDataInfo[DataName].push_back(Val ? "True" : "False");

    if (KeyIdx == -1)
    {
        // 새롭게 Vector 를 확장한다.
        size_t CurrentColumnSize = m_vecDataInfo.size();
        m_vecDataInfo.push_back(std::vector<std::string>());
        m_vecDataInfo[CurrentColumnSize].push_back(Val ? "True" : "False");
    }
    else
    {
        m_vecDataInfo[KeyIdx].push_back(Val ? "True" : "False");
    }

    CalculateMaxElemCount();
}

void CIMGUITableElemList::AddData(const std::string& DataName, const std::string& Val)
{
    int KeyIdx = FindMapKeyIdxInVector(DataName);
  
    m_mapDataInfo[DataName].push_back(Val);

    if (KeyIdx == -1)
    {
        // 새롭게 Vector 를 확장한다.
        size_t CurrentColumnSize = m_vecDataInfo.size();
        m_vecDataInfo.push_back(std::vector<std::string>());
        m_vecDataInfo[CurrentColumnSize].push_back(Val);
    }
    else
    {
        m_vecDataInfo[KeyIdx].push_back(Val);
    }

    CalculateMaxElemCount();
}

// 일반 const char* 을 받는 함수
void CIMGUITableElemList::AddData(const std::string& DataName, char* Val)
{
    int KeyIdx = FindMapKeyIdxInVector(DataName);

    m_mapDataInfo[DataName].push_back(Val);

    if (KeyIdx == -1)
    {
        // 새롭게 Vector 를 확장한다.
        size_t CurrentColumnSize = m_vecDataInfo.size();
        m_vecDataInfo.push_back(std::vector<std::string>());
        m_vecDataInfo[CurrentColumnSize].push_back(Val);
    }
    else
    {
        m_vecDataInfo[KeyIdx].push_back(Val);
    }

    CalculateMaxElemCount();
}

void CIMGUITableElemList::MakeKey(const std::string& DataName)
{
    if (m_mapDataInfo.find(DataName) != m_mapDataInfo.end())
        return;

    m_mapDataInfo[DataName] = std::vector<std::string>{};
}

void CIMGUITableElemList::ClearAll()
{
    m_mapDataInfo.clear();
    m_vecDataInfo.clear();
}

void CIMGUITableElemList::CalculateMaxElemCount()
{
    auto iter = m_mapDataInfo.begin();
    auto iterEnd = m_mapDataInfo.end();

    int MaxElemCnt = 0;

    for (; iter != iterEnd; ++iter)
    {
        if (iter->second.size() > MaxElemCnt)
            MaxElemCnt = (int)iter->second.size();
    }

    m_MaxElemSize = MaxElemCnt;
}

void CIMGUITableElemList::ClearContents()
{
    auto iter = m_mapDataInfo.begin();
    auto iterEnd = m_mapDataInfo.end();

    for (; iter != iterEnd; ++iter)
    {
        iter->second.clear();
    }

    size_t vecSize = m_vecDataInfo.size();

    for (size_t i = 0; i < vecSize; ++i)
    {
        m_vecDataInfo[i].clear();
    }
}

int CIMGUITableElemList::FindMapKeyIdxInVector(const std::string& Key)
{
    int KeyIdx = 0;

    auto iter = m_mapDataInfo.begin();
    auto iterEnd = m_mapDataInfo.end();

    for (; iter != iterEnd; ++iter)
    {
        if (iter->first == Key)
            return KeyIdx;
        ++KeyIdx;
    }

    return -1;
}

bool CIMGUITableElemList::Init()
{
    return true;
}

void CIMGUITableElemList::Render()
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

    size_t ColumnSize = m_mapDataInfo.size();

    if (ColumnSize <= 0)
    {
        ImGui::PopID();
        return;
    }

    // m_MaxElemCnt + 1 을 해주는 이유는, Column 에 iter->first , 즉, Key 값도 들어가야 하기 때문이다.
    if (ImGui::BeginTable("table_scrolly", (int)ColumnSize, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

        auto iter = m_mapDataInfo.begin();
        auto iterEnd = m_mapDataInfo.end();

        // 첫번째 행 --> 각 열의 제목 세팅
        for (; iter != iterEnd; ++iter)
        {
            ImGui::TableSetupColumn(iter->first.c_str(), ImGuiTableColumnFlags_None);
        }

        ImGui::TableHeadersRow();

        // Demonstrate using clipper for large vertical lists
        ImGuiListClipper clipper;

        // 행 개수
        clipper.Begin((int)m_MaxElemSize);

        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();

                for (int column = 0; column < ColumnSize; column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    if (m_vecDataInfo[column].size() > row)
                    {
                        ImGui::TextColored(m_Color.Value, m_vecDataInfo[column][row].c_str());
                    }
                }
            }
        }
        ImGui::EndTable();
    }

    ImGui::PopID();

}
