#include "IMGUIText.h"
#include "IMGUIUtil.h"


CIMGUIText::CIMGUIText() :
    m_Text{},
    m_wText{},
    m_IsHelpMode(false),
    m_TextUTF8{}
{
}

CIMGUIText::~CIMGUIText()
{
}

bool CIMGUIText::Init()
{
    return true;
}

void CIMGUIText::Render()
{
    if (m_IsHelpMode) // ����� 
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(m_TextUTF8);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    else
    {
        ImGui::TextColored(m_Color.Value, m_TextUTF8);
    }

    ApplyDragEffect();
}

void CIMGUIText::ApplyDragEffect()
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        // ImGui::SetDragDropPayload("DND_DEMO_CELL", m_TextUTF8, sizeof(char) * strlen(m_TextUTF8));
        // ImGui::SetDragDropPayload(vIMGUIDragDropPayLoadName.TextDragDrop, m_wText, sizeof(TCHAR) * lstrlen(m_wText));
        ImGui::SetDragDropPayload(vIMGUIDragDropPayLoadName.TextDragDrop, m_wText, sizeof(m_wText));
        // - �Ʒ��� ���� Key ���� �ٲ�� �ϴ� DND_DEMO_CELL ����, �ٸ� Key ������ �ٲ�� �ϴ� ������
        // - IMGUIText �� �ƴ϶�, IMGUIChild �� Drag, Drop �ϴ� ��� ��
        // - ���� �ٸ� Widget ���� Drag Drop �ϴ� ��츦 ����ؾ� �ϱ� �����̴�.

        ImGui::EndDragDropSource();
    }
}
