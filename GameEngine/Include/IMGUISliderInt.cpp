#include "IMGUISliderInt.h"

CIMGUISliderInt::CIMGUISliderInt()  :
    mValue(0),
    mMin(0),
    mMax(100)
{
}

CIMGUISliderInt::~CIMGUISliderInt()
{
}

bool CIMGUISliderInt::Init()
{
    return true;
}

void CIMGUISliderInt::Render()
{
    if (ImGui::SliderInt(m_Name.c_str(), &mValue, mMin, mMax, "%d", 0))
    {
        if (mCallBack)
        {
            mCallBack(mValue);
        }
    }
}
