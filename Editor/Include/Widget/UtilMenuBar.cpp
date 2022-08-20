#include "UtilMenuBar.h"
#include "../Window/FBXConvertWindow.h"
#include "../Window/FileBrowser.h"
#include "../Window/FileBrowserTree.h"
#include "../Window/ToolWindow.h"
#include "IMGUIMenuItem.h"
#include "../EditorManager.h"

CUtilMenuBar::CUtilMenuBar()
{
}

CUtilMenuBar::~CUtilMenuBar()
{
}

bool CUtilMenuBar::Init()
{
	// Scene
	m_FBXConvertWindow = AddMenuItem("FBX Convert");
	m_FBXConvertWindow->SetClickCallBack(this, &CUtilMenuBar::OnToggleFBXConvertWindow);

	// File Browser
	m_FileBrowserTree = AddMenuItem("File Browse");
	m_FileBrowserTree->SetClickCallBack(this, &CUtilMenuBar::OnToggleFileBrowser);

	// Tool Window
	// m_ToolWindow = AddMenuItem("Tool");
	// m_ToolWindow->SetClickCallBack(this, &CUtilMenuBar::OnToggleToolWindow);

	return true;
}

void CUtilMenuBar::Render()
{
	CIMGUIBeginMenu::Render();
}

void CUtilMenuBar::OnToggleFBXConvertWindow()
{
	bool IsOpen = CEditorManager::GetInst()->GetFBXConvertWindow()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetFBXConvertWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetFBXConvertWindow()->Open();
	}
}

void CUtilMenuBar::OnToggleFileBrowser()
{
	bool IsOpen = CEditorManager::GetInst()->GetFileBrowser()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetFileBrowser()->Close();
		CEditorManager::GetInst()->GetFileBrowserTree()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetFileBrowser()->Open();
		CEditorManager::GetInst()->GetFileBrowserTree()->Open();
	}
}

/*
void CUtilMenuBar::OnToggleToolWindow()
{
	bool IsOpen = CEditorManager::GetInst()->GetToolWindow()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetToolWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetToolWindow()->Open();
	}
}

*/