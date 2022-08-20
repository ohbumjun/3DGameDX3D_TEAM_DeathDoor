#include "IMGUIRadioButton.h"

CIMGUIRadioButton::CIMGUIRadioButton()    :
    mbMultiColumn(false),
    mbMultiCheck(false),
    mSpacingX(10.f),
    mCheckItemIdx(0)
{
}

CIMGUIRadioButton::~CIMGUIRadioButton()
{
    size_t size = mVecCheckInfo.size();
    
    for (size_t i = 0; i < size; ++i)
    {
        SAFE_DELETE(mVecCheckInfo[i]);
    }

    mVecCheckInfo.clear();
}

bool CIMGUIRadioButton::Init()
{
    return true;
}

void CIMGUIRadioButton::Render()
{
    size_t size = mVecCheckInfo.size();

    int count = 0;

    for (size_t i = 0; i < size; ++i)
    {
        if (ImGui::RadioButton(mVecCheckInfo[i]->LabelUTF8, mVecCheckInfo[i]->bCheck))
        {
            if (mVecCheckInfo[i]->bCheck)
            {
                if (!mAlwaysCheck)
                {
					mVecCheckInfo[i]->bCheck = false;
                }
            }
            else
            {
                if (!mbMultiCheck)
                {
					mVecCheckInfo[mCheckItemIdx]->bCheck = false;
					mCheckItemIdx = (int)i;
                }

                mVecCheckInfo[i]->bCheck = true;

                if (mCallBack)
                {
                    mCallBack(mVecCheckInfo[i]->LabelUTF8, mVecCheckInfo[i]->bCheck);
                }
            }
        }

        if (mbMultiColumn)
        {
            if (count == mColNum - 1)
            {
                count -= mColNum - 1;
            }
            else
            {
                ImGui::SameLine(0.f, mSpacingX);
                ++count;
            }
        }
    }
}
