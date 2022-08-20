#include "IMGUIInputFloat3.h"

CIMGUIInputFloat3::CIMGUIInputFloat3() :
	mValue{ 0.f, 0.f, 0.f },
	mbInputRange(false),
	mMin(0.f),
	mMax(100.f)
{
}

CIMGUIInputFloat3::~CIMGUIInputFloat3()
{
}

bool CIMGUIInputFloat3::Init()
{
	CIMGUIWidget::Init();

	return true;
}

void CIMGUIInputFloat3::Render()
{
	ImGui::PushID(m_WidgetID);

	float value[3] = { mValue.x, mValue.y, mValue.z };

	if (ImGui::InputFloat3(m_Name.c_str(), value))
	{
		memcpy(&mValue, value, sizeof(float) * 3);

		// 최대, 최소값이 있을 경우
		if (mbInputRange)
		{
			for (int i = 0; i < 3; ++i)
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

