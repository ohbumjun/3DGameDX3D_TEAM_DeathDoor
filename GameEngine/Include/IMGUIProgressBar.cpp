#include "IMGUIProgressBar.h"

CIMGUIProgressBar::CIMGUIProgressBar()
{
}

CIMGUIProgressBar::~CIMGUIProgressBar()
{
}

bool CIMGUIProgressBar::Init()
{
	if (!CIMGUIWidget::Init())
	{
		return false;
	}

	return true;
}

void CIMGUIProgressBar::Render()
{
	ImGui::ProgressBar(m_Percent, m_Size);
}
