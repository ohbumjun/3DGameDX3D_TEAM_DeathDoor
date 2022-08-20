#include "IMGUISliderFloat.h"

CIMGUISliderFloat::CIMGUISliderFloat() :
    mValue(0.f),
    mMin(0.f),
    mMax(1.f)
{
}

CIMGUISliderFloat::~CIMGUISliderFloat()
{
}

bool CIMGUISliderFloat::Init()
{
    CIMGUIWidget::Init();
    return true;
}

void CIMGUISliderFloat::Render()
{
    ImGui::PushID(m_WidgetID);

    if (ImGui::SliderFloat(m_Name.c_str(), &mValue, mMin, mMax, "%.1f", 0))
    {
        if (mCallBack)
        {
            mCallBack(mValue);
        }
    }

    ImGui::PopID();
}
