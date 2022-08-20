#include "IMGUIButton.h"
#include "IMGUIWindow.h"

CIMGUIButton::CIMGUIButton() :
    m_Click(false)
{
}

CIMGUIButton::~CIMGUIButton()
{
}

bool CIMGUIButton::Init()
{
    CIMGUIWidget::Init();

    return true;
}

void CIMGUIButton::Render()
{
    ImGui::PushID(m_WidgetID);

    m_Click = ImGui::Button(m_Name.c_str(), m_Size);

    if (m_Click && m_ClickCallback)
        m_ClickCallback();

    ImGui::PopID();
}

