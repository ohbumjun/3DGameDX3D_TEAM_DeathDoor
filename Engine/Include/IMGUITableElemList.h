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
    // Table �⺻ Height, Width
    const float TEXT_BASE_HEIGHT;
private:
    std::string m_TableTitle;
    float m_TableHeight;
    float m_TableWidth;
    int m_MaxElemSize;
    std::unordered_map<std::string, std::vector<std::string>> m_mapDataInfo;

    // m_mapDataInfo �� �ִ� ������ vector �������� ���� �Ǵ� ���̴�
    // �Ʒ� m_vecDataInfo �� ���� �ϳ� -> ��, std::vector<std::string> �� �� Column �� ����, �Ʒ��� ������ Info �̴�.
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
    // �Ϲ� const char* �� �޴� �Լ�
    void AddData(const std::string& DataName, char* Val);
    void MakeKey(const std::string& DataName);
    void ClearAll();
    void CalculateMaxElemCount();
    // ���� Key ���� ���ܵΰ�, Contents ����鸸 ���Ƴ���.
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

