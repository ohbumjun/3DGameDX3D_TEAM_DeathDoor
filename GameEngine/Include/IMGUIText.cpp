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
    if (m_IsHelpMode) // 설명글 
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
        // - 아래와 같이 Key 값을 바꿔야 하는 DND_DEMO_CELL 말고, 다른 Key 값으로 바꿔야 하는 이유는
        // - IMGUIText 가 아니라, IMGUIChild 를 Drag, Drop 하는 경우 등
        // - 서로 다른 Widget 들을 Drag Drop 하는 경우를 고려해야 하기 때문이다.

        ImGui::EndDragDropSource();
    }
}
