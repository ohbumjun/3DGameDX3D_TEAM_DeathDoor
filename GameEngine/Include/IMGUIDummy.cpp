#include "IMGUIDummy.h"

CIMGUIDummy::CIMGUIDummy()
{
}

CIMGUIDummy::~CIMGUIDummy()
{
}

bool CIMGUIDummy::Init()
{
	return true;
}

void CIMGUIDummy::Render()
{
	ImGui::Dummy(m_Size);
}
