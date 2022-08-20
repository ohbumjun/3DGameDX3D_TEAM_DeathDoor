#include "IMGUIColor4.h"

CIMGUIColor4::CIMGUIColor4() :
	mValue{ 0.f, 0.f, 0.f, 0.f }
{
}

CIMGUIColor4::~CIMGUIColor4()
{
}

bool CIMGUIColor4::Init()
{
	CIMGUIWidget::Init();

	return true;
}

void CIMGUIColor4::Render()
{
	float value[4] = { mValue.x, mValue.y, mValue.z, mValue.w };
	if (ImGui::ColorEdit4(m_Name.c_str(), value))
	{
		memcpy(&mValue, value, sizeof(float) * 4);

		if (mCallBack)
		{
			mCallBack(mValue);
		}
	}
}
