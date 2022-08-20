#include "IMGUIInputFloat2.h"

CIMGUIInputFloat2::CIMGUIInputFloat2() :
	mValue{ 0.f, 0.f },
	mbInputRange(false),
	mMin(0.f),
	mMax(100.f)
{
}

CIMGUIInputFloat2::~CIMGUIInputFloat2()
{
}

bool CIMGUIInputFloat2::Init()
{
	CIMGUIWidget::Init();
	return true;
}

void CIMGUIInputFloat2::Render()
{
	ImGui::PushID(m_WidgetID);

	float value[2] = { mValue.x, mValue.y };

	if (ImGui::InputFloat2(m_Name.c_str(), value))
	{
		memcpy(&mValue, value, sizeof(float) * 2);

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

