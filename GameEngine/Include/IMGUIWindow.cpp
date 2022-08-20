#include "IMGUIWindow.h"
#include "IMGUIWidget.h"
#include "IMGUIManager.h"

CIMGUIWindow::CIMGUIWindow() :
	m_Open(true),
	m_WindowFlag(ImGuiWindowFlags_NoCollapse),
	m_ModalPopup(false)
{
}

CIMGUIWindow::~CIMGUIWindow()
{
	size_t	Size = m_vecPopupWidget.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecPopupWidget[i]);
	}

	m_vecPopupWidget.clear();

	Size = m_vecWidget.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecWidget[i]);
	}

	m_vecWidget.clear();
}

void CIMGUIWindow::Open()
{
	m_Open = true;
}

void CIMGUIWindow::Close()
{
	m_Open = false;
}

void CIMGUIWindow::ClosePopup()
{
	ImGui::CloseCurrentPopup();

	size_t	Size = m_vecPopupWidget.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecPopupWidget[i]);
	}

	m_vecPopupWidget.clear();
}

CIMGUIWidget* CIMGUIWindow::FindWidget(const std::string& Name)
{
	size_t Size = m_vecWidget.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecWidget[i]->m_Name == Name)
			return m_vecWidget[i];
	}

	return nullptr;
}

void CIMGUIWindow::EraseWidget(const std::string& Name)
{
	size_t Count = m_vecWidget.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecWidget[i]->m_Name == Name)
		{
			auto iter = m_vecWidget.begin();
			std::advance(iter, i);

			m_vecWidget.erase(iter);
		}
	}
}

void CIMGUIWindow::DeleteWidget(const std::string& Name)
{
	size_t Count = m_vecWidget.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecWidget[i]->m_Name == Name)
		{
			SAFE_DELETE(m_vecWidget[i]);

			auto iter = m_vecWidget.begin();
			std::advance(iter, i);

			m_vecWidget.erase(iter);
		}
	}
}

bool CIMGUIWindow::Init()
{
	return true;
}

void CIMGUIWindow::Update(float DeltaTime)
{
	if (!m_Open)
		return;

	if (!ImGui::Begin(m_Name.c_str(), &m_Open, m_WindowFlag))
		m_Open = false;

	m_WindowStartPos.x = ImGui::GetCursorScreenPos().x;
	m_WindowStartPos.y = ImGui::GetCursorScreenPos().y;

	m_WindowSize.x = ImGui::GetContentRegionAvail().x;
	m_WindowSize.y = ImGui::GetContentRegionAvail().y;

	CIMGUIManager::GetInst()->SetCurrentFont();

	size_t Size = m_vecWidget.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (!m_vecWidget[i]->m_Render)
		{
			continue;
		}

		m_vecWidget[i]->Render();
	}

	CIMGUIManager::GetInst()->ResetCurrentFont();

	ImGui::End();
}

