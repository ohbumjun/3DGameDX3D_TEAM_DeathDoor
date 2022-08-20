#include "IMGUIInputInt.h"

CIMGUIInputInt::CIMGUIInputInt()    :
    mVal(0),
    mStep(1)
{
}

CIMGUIInputInt::~CIMGUIInputInt()
{
}

bool CIMGUIInputInt::Init()
{
    return true;
}

void CIMGUIInputInt::Render()
{
    ImGui::PushItemWidth(m_Size.x);
    if (ImGui::InputInt(m_Name.c_str(), &mVal, mStep))
    {
        if (mCallBack)
        {
            mCallBack(mVal);
        }
    }
    ImGui::PopItemWidth();
}
