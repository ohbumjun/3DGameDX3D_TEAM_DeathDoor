#include "IMGUIColor3.h"

CIMGUIColor3::CIMGUIColor3() :
	mValue{ 0.f, 0.f, 0.f }
{
}

CIMGUIColor3::~CIMGUIColor3()
{
}

bool CIMGUIColor3::Init()
{
	CIMGUIWidget::Init();

	return true;
}

void CIMGUIColor3::Render()
{
	ImGui::PushID(m_WidgetID);

	float value[3] = { mValue.x, mValue.y, mValue.z };

	if (ImGui::ColorEdit3(m_Name.c_str(), value))
	{
		memcpy(&mValue, value, sizeof(float) * 3);

		if (mCallBack)
		{
			mCallBack(mValue);
		}
	}

	ImGui::PopID();
}

