#pragma once
#include "IMGUIWidget.h"


#pragma once
#include "IMGUIWidget.h"

class CIMGUITableElemList :
    public CIMGUIWidget
{
public:
    CIMGUITableElemList();
    virtual ~CIMGUITableElemList() override;
private:
    // Table 기본 Height, Width
    const float TEXT_BASE_HEIGHT;
private:
    std::string m_TableTitle;
    float m_TableHeight;
    float m_TableWidth;
    int m_MaxElemSize;
    std::unordered_map<std::string, std::vector<std::string>> m_mapDataInfo;

    // m_mapDataInfo 에 있는 내용이 vector 형식으로 들어가게 되는 것이다
    // 아래 m_vecDataInfo 의 원소 하나 -> 즉, std::vector<std::string> 는 한 Column 을 따라, 아래로 나열될 Info 이다.
    std::vector<std::vector<std::string>> m_vecDataInfo; 
public:
    void SetTableTitle(const std::string& Title)
    {
        m_TableTitle = Title;
    }
    void SetTableHeight(float Height)
    {
        m_TableHeight = Height;
    }
    void SetTableWidth(float Width)
    {
        m_TableWidth = Width;
    }
public:
    void AddData(const std::string& DataName, int Val);
    void AddData(const std::string& DataName, float Val);
    void AddData(const std::string& DataName, bool Val);
    void AddData(const std::string& DataName, const std::string& Val);
    // 일반 const char* 을 받는 함수
    void AddData(const std::string& DataName, char* Val);
    void MakeKey(const std::string& DataName);
    void ClearAll();
    void CalculateMaxElemCount();
    // 기존 Key 값은 남겨두고, Contents 내용들만 갈아낀다.
    void ClearContents();
    int FindMapKeyIdxInVector(const std::string& Key);
public:
    virtual bool Init();
    virtual void Render();

private:
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
};

