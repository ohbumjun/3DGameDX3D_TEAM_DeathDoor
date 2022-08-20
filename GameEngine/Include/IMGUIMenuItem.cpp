#include "IMGUIMenuItem.h"

CIMGUIMenuItem::CIMGUIMenuItem()    :
    mbSelect(false),
    mbEnable(true),
    m_EnableCheck(false)
{
}

CIMGUIMenuItem::~CIMGUIMenuItem()
{
}

bool CIMGUIMenuItem::Init()
{
    return true;
}

void CIMGUIMenuItem::Render()
{
    // 1) 기존에 사용되고 있었던 Render 함수 코드
   // if (mShortCut.empty())
   // {
   //     ImGui::MenuItem(m_Name.c_str(), NULL, &mbSelect, mbEnable);
   // }
   // else
   // {
   //     ImGui::MenuItem(m_Name.c_str(), mShortCut.c_str(), &mbSelect, mbEnable);
   // }
   // 
   // if (mbSelect)
   // {
   //     if (mCallBack)
   //     {
   //         mCallBack();
   //         mbSelect = false;
   //     }
   // }


    // 2) 새롭게 반영한 Render 함수 코드
    // - Why ? Load Scene Menu Bar 옆에만 계속 체크 표시가 된다.
    if (ImGui::MenuItem(m_Name.c_str()))
    {
        if (mCallBack)
        {
            mCallBack();
            mbSelect = false;
        }
    }
}
