#include "IMGUISeperator.h"

CIMGUISeperator::CIMGUISeperator()
{
}

CIMGUISeperator::~CIMGUISeperator()
{
}

bool CIMGUISeperator::Init()
{
	return true;
}

void CIMGUISeperator::Render()
{
	ImGui::Separator();
}
