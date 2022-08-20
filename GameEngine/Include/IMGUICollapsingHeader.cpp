#include "IMGUICollapsingHeader.h"

CIMGUICollapsingHeader::CIMGUICollapsingHeader() :
	mSize(0)
{
}

CIMGUICollapsingHeader::~CIMGUICollapsingHeader()
{
	ClearWidget();
}

bool CIMGUICollapsingHeader::Init()
{
	CIMGUIWidget::Init();
	return true;
}

void CIMGUICollapsingHeader::Render()
{
    ImGui::PushID(m_WidgetID);

	if (ImGui::CollapsingHeader(m_Name.c_str()))
	{
		mSize = mVecChild.size();
		for (size_t i = 0; i < mSize; ++i)
		{
			mVecChild[i]->Render();
		}
	}

    ImGui::PopID();
}

void CIMGUICollapsingHeader::ClearWidget()
{
	size_t size = mVecChild.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(mVecChild[i]);
	}

	mVecChild.clear();
}

void CIMGUICollapsingHeader::DeleteWidget(CIMGUIWidget* widget)
{
	auto iter = mVecChild.begin();
	auto iterEnd = mVecChild.end();

	for (; iter != iterEnd; ++iter)
	{
		if (widget == (*iter))
		{
			SAFE_DELETE(*iter);
			mVecChild.erase(iter);
			mSize = mVecChild.size();
			return;
		}
	}
}

