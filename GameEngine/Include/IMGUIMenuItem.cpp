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
    // 1) ������ ���ǰ� �־��� Render �Լ� �ڵ�
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


    // 2) ���Ӱ� �ݿ��� Render �Լ� �ڵ�
    // - Why ? Load Scene Menu Bar ������ ��� üũ ǥ�ð� �ȴ�.
    if (ImGui::MenuItem(m_Name.c_str()))
    {
        if (mCallBack)
        {
            mCallBack();
            mbSelect = false;
        }
    }
}
