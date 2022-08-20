#pragma once
#include "IMGUIWidget.h"

class CIMGUITable :
    public CIMGUIWidget
{
public:
    CIMGUITable();
    virtual ~CIMGUITable() override;
private :
    // Table 기본 Height, Width
    const float TEXT_BASE_HEIGHT;
private:
    size_t m_MaxElemCnt;
    std::string m_TableTitle;
    float m_TableHeight;
    float m_TableWidth;
    std::unordered_map<std::string, std::vector<std::string>> m_mapDataInfo;
public :
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
    void MakeKey(const std::string& DataName);
    void ClearAll();
    // 기존 Key 값은 남겨두고, Contents 내용들만 갈아낀다.
    void ClearContents();

public :
    virtual bool Init();
    virtual void Render();

private :
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

