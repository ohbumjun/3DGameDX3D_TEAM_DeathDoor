
#include "IMGUIPopUpModal.h"

CIMGUIPopUpModal::CIMGUIPopUpModal()    :
    mSize(0),
    m_State(PopUpModalState::Open)
{
}

CIMGUIPopUpModal::~CIMGUIPopUpModal()
{
    size_t size = mVecChild.size();

    for (size_t i = 0; i < size; ++i)
    {
        SAFE_DELETE(mVecChild[i]);
    }
}

bool CIMGUIPopUpModal::Init()
{
    return true;
}

void CIMGUIPopUpModal::Render()
{
    if (m_State == PopUpModalState::Closed)
        return;

    ImGui::OpenPopup(m_Name.c_str());

    if (ImGui::BeginPopupModal(m_Name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        m_Render = true;
		size_t size = mVecChild.size();
		for (size_t i = 0; i < size; ++i)
		{
            mVecChild[i]->Render();
		}

        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
            m_State = PopUpModalState::Closed;
            m_Render = false;
        }

        ImGui::EndPopup();
    }
}
