#include "IMGUIInputInt2.h"

CIMGUIInputInt2::CIMGUIInputInt2()	:
	mValue{0, 0},
	mbInputRange(false),
	mMin(0),
	mMax(100000)
{
}

CIMGUIInputInt2::~CIMGUIInputInt2()
{
}

bool CIMGUIInputInt2::Init()
{
	CIMGUIWidget::Init();

	return true;
}

void CIMGUIInputInt2::Render()
{
	ImGui::PushID(m_WidgetID);

	if (ImGui::InputInt2(m_Name.c_str(), mValue))
	{
		// 최대, 최소값이 있을 경우
		if (mbInputRange)
		{
			for (int i = 0; i < 2; ++i)
			{
				if (mValue[i] < mMin)
				{
					mValue[i] = mMin;
				}
				else if (mValue[i] > mMax)
				{
					mValue[i] = mMax;
				}
			}
		}

		if (mCallBack)
		{
			mCallBack(mValue);
		}
	}

	ImGui::PopID();
}
