#include "IMGUIWidget.h"
#include "IMGUIWindow.h"

CIMGUIWidget::CIMGUIWidget() :
    m_Font(nullptr),
    m_Color{ 1.f, 1.f, 1.f, 1.f },
    m_Size{ 100.f, 100.f },
    m_Render(true)
{
}

CIMGUIWidget::~CIMGUIWidget()
{
}

bool CIMGUIWidget::Init()
{
    m_WidgetID = m_Owner->GetWidgetID();
    m_Owner->IncreaseWidgetID();

    return true;
}

void CIMGUIWidget::Render()
{
}

