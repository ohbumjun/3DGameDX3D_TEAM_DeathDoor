#include "SceneMenu.h"
#include "IMGUIMenuItem.h"
#include "../EditorManager.h"
#include "../Window/SceneModeWindow.h"

CSceneMenu::CSceneMenu()
{
}

CSceneMenu::~CSceneMenu()
{
}

bool CSceneMenu::Init()
{
	m_SceneModeToggle = AddMenuItem("SceneMode Edit");
	m_SceneModeToggle->SetClickCallBack(this, &CSceneMenu::OnToggleSceneMode);

	return true;
}

void CSceneMenu::OnToggleSceneMode()
{
	bool IsOpen = CEditorManager::GetInst()->GetSceneModeWindow()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetSceneModeWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetSceneModeWindow()->Open();
	}
}
