#include "IMGUICheckBox.h"

CIMGUICheckBox::CIMGUICheckBox()    :
    mbMultiColumn(false),
    mSpacingX(10.f),
    mColNum(0)
{
}

CIMGUICheckBox::~CIMGUICheckBox()
{
    size_t size = mVecCheckInfo.size();
    
    for (size_t i = 0; i < size; ++i)
    {
        SAFE_DELETE(mVecCheckInfo[i]);
    }

    mVecCheckInfo.clear();
}

bool CIMGUICheckBox::Init()
{
    CIMGUIWidget::Init();

    return true;
}

void CIMGUICheckBox::Render()
{
    ImGui::PushID(m_WidgetID);

    size_t size = mVecCheckInfo.size();

    int count = 0;

    for (size_t i = 0; i < size; ++i)
    {
        if (ImGui::Checkbox(mVecCheckInfo[i]->LabelUTF8, &mVecCheckInfo[i]->bCheck))
        {
            if (mCallBackLabel)
            {
                mCallBackLabel(mVecCheckInfo[i]->LabelUTF8, mVecCheckInfo[i]->bCheck);
            }
            else if (mCallBackIdx) 
            {
                mCallBackIdx((int)i, mVecCheckInfo[i]->bCheck);
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

    ImGui::PopID();
}
