#include "IMGUIWidgetList.h"

CIMGUIWidgetList::CIMGUIWidgetList() :
	mSize(0),
	m_ApplyHideEffect(false)
{
}

CIMGUIWidgetList::~CIMGUIWidgetList()
{
	ClearWidget();
}

bool CIMGUIWidgetList::Init()
{
	return true;
}

void CIMGUIWidgetList::Render()
{
	if (m_ApplyHideEffect)
		RenderHide();
	else
		RenderNoHide();
}

void CIMGUIWidgetList::RenderHide()
{
	if (ImGui::CollapsingHeader(m_Name.c_str()))
	{
		RenderNoHide();
	}
}

void CIMGUIWidgetList::RenderNoHide()
{
	mSize = mVecChild.size();

	for (size_t i = 0; i < mSize; ++i)
	{
		if (!mVecChild[i]->IsRender())
		{
			continue;
		}
		mVecChild[i]->Render();
	}

}

void CIMGUIWidgetList::ClearWidget()
{
	size_t size = mVecChild.size();

	for (size_t i = 0; i < size; ++i)
	{
		SAFE_DELETE(mVecChild[i]);
	}

	mVecChild.clear();
}

void CIMGUIWidgetList::DeleteWidget(CIMGUIWidget* widget)
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

