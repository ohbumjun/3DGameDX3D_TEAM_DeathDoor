#include "DataMenu.h"
#include "IMGUIMenuItem.h"
#include "../EditorManager.h"
#include "../Window/CollisionProfileEditor.h"
#include "../Window/InspectorWindow.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "../Window/ObjectComponentWindow.h"
#include "../Window/SceneComponentHierarchyWindow.h"

CDataMenu::CDataMenu()	:
	m_CollisionProfileToggle(nullptr)
{
}

CDataMenu::~CDataMenu()
{
}

bool CDataMenu::Init()
{
	m_CollisionProfileToggle = AddMenuItem("Collision Profile");
	m_CollisionProfileToggle->SetClickCallBack(this, &CDataMenu::OnToggleCollisionProfileToggle);

	m_InspectorToggle = AddMenuItem("Inspector");
	m_InspectorToggle->SetClickCallBack(this, &CDataMenu::OnToggleInspectorToggle);

	m_OverAllObjectDataToggle = AddMenuItem("Object");
	m_OverAllObjectDataToggle->SetClickCallBack(this, &CDataMenu::OnToggleOverAllObjectDataToggle);


	return true;
}

void CDataMenu::OnToggleCollisionProfileToggle()
{
	bool IsOpen = CEditorManager::GetInst()->GetCollisionProfileEditor()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetCollisionProfileEditor()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetCollisionProfileEditor()->Open();
	}
}

void CDataMenu::OnToggleInspectorToggle()
{
	bool IsOpen = CEditorManager::GetInst()->GetInspectorWindow()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetInspectorWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetInspectorWindow()->Open();
	}
}

void CDataMenu::OnToggleOverAllObjectDataToggle()
{
	bool IsOpen = CEditorManager::GetInst()->GetObjectHierarchyWindow()->IsOpen();

	if (IsOpen)
	{
		CEditorManager::GetInst()->GetObjectHierarchyWindow()->Close();
		CEditorManager::GetInst()->GetObjectComponentWindow()->Close();
		CEditorManager::GetInst()->GetComponentHierarchyWindow()->Close();
	}
	else
	{
		CEditorManager::GetInst()->GetObjectHierarchyWindow()->Open();
		CEditorManager::GetInst()->GetObjectComponentWindow()->Open();
		CEditorManager::GetInst()->GetComponentHierarchyWindow()->Open();
	}
}
