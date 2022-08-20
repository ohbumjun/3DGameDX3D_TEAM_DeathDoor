#include "IMGUIChild.h"

CIMGUIChild::CIMGUIChild() :
	mbBorder(false),
	m_SameLine(false)
{
}

CIMGUIChild::~CIMGUIChild()
{
	size_t size = mVecChild.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(mVecChild[i]);
	}
}

bool CIMGUIChild::Init()
{
	return true;
}

void CIMGUIChild::Render()
{
	ImGui::BeginChild(m_Name.c_str(), m_Size, mbBorder);

	size_t size = mVecChild.size();
	for (size_t i = 0; i < size; ++i)
	{
		mVecChild[i]->Render();
	}

	ImGui::EndChild();

	if (m_SameLine)
		ImGui::SameLine();
}

void CIMGUIChild::ClearWidget()
{
	size_t size = mVecChild.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(mVecChild[i]);
	}

	mVecChild.clear();
}

