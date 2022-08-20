#include "IMGUIMainMenuBar.h"

CIMGUIMainMenuBar::CIMGUIMainMenuBar()
{
}

CIMGUIMainMenuBar::~CIMGUIMainMenuBar()
{
	size_t size = mVecChild.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(mVecChild[i]);
	}
}

bool CIMGUIMainMenuBar::Init()
{
	return true;
}

void CIMGUIMainMenuBar::Render()
{
	if (ImGui::BeginMainMenuBar())
	{
		mSize = mVecChild.size();
		for (size_t i = 0; i < mSize; ++i)
		{
			mVecChild[i]->Render();
		}

		ImGui::EndMainMenuBar();
	}
}
