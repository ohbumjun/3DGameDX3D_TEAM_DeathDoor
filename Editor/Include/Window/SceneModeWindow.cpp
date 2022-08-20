#include "SceneModeWindow.h"
#include "IMGUIText.h"
#include "IMGUIComboBox.h"
#include "../DeathDoor/DDUtil.h"
#include "Scene/SceneManager.h"
#include "../EditorManager.h"
#include "../Window/ToolWindow.h"
#include "../DeathDoor/Scene/DDSceneMode.h"
#include "../DeathDoor/Scene/DDBossSceneMode.h"
#include "../DeathDoor/Scene/DDInstanceSceneMode.h"
#include "../DeathDoor/Scene/DDPuzzleSceneMode.h"

#include "../Widget/DDSceneModeWidget.h"
#include "../Widget/DDInstanceSceneModeWidget.h"
#include "../Widget/DDPuzzleSceneModeWidet.h"

CSceneModeWindow::CSceneModeWindow()
{
}

CSceneModeWindow::~CSceneModeWindow()
{
}

bool CSceneModeWindow::Init()
{
	CIMGUIText* Text = AddWidget<CIMGUIText>("Text");
	Text->SetText("SceneMode Window");

	m_DDSceneTypeCombo = AddWidget<CIMGUIComboBox>("DeathDoor SceneMode Type", 200.f);

	for (int i = 0; i < (int)DDSceneModeType::Max; ++i)
	{
		m_DDSceneTypeCombo->AddItem(CDDUtil::DDSceneModeTypeToString((DDSceneModeType)i));
		m_DDSceneTypeCombo->SetSelectCallback(this, &CSceneModeWindow::OnSelectSceneType);
	}

	return true;
}

void CSceneModeWindow::OnRefreshSceneModeWidget(class CSceneMode* Mode)
{
	if (!m_SceneModeWidget)
	{
		CreateModeWidget(Mode);
	}
}

void CSceneModeWindow::OnSelectSceneType(int Idx, const char* Label)
{
	size_t SceneTypeID = CDDUtil::DDSceneModeTypeToTypeID((DDSceneModeType)Idx);

	CScene* CurScene = CSceneManager::GetInst()->GetScene();

	bool Success = CurScene->SetSceneMode(SceneTypeID);

	if (!Success)
	{
		MessageBox(nullptr, TEXT("Scene Mode 생성 실패"), TEXT("Error"), MB_OK);
		return;
	}
	
	DeleteWidget("SceneModeWidget");

	CSceneMode* Mode = CurScene->GetSceneMode();
	CreateModeWidget(Mode);

	// Tool Window를 통해 Pause
	CEditorManager::GetInst()->GetToolWindow()->OnClickPause();
}

void CSceneModeWindow::CreateModeWidget(class CSceneMode* Mode)
{
	size_t TypeID = Mode->GetTypeID();

	if (TypeID == typeid(CDDSceneMode).hash_code())
	{
		m_SceneModeWidget = AddWidget<CDDSceneModeWidget>("SceneModeWidget", 200.f);
	}
	else if (TypeID == typeid(CDDBossSceneMode).hash_code())
	{
		m_SceneModeWidget = AddWidget<CDDSceneModeWidget>("SceneModeWidget", 200.f);
	}
	else if (TypeID == typeid(CDDInstanceSceneMode).hash_code())
	{
		m_SceneModeWidget = AddWidget<CDDInstanceSceneModeWidget>("SceneModeWidget", 200.f);
	}
	else if (TypeID == typeid(CDDPuzzleSceneMode).hash_code())
	{
		m_SceneModeWidget = AddWidget<CDDPuzzleSceneModeWidet>("SceneModeWidget", 200.f);
	}

	if (m_SceneModeWidget)
	{
		m_SceneModeWidget->SetSceneMode((CDDSceneMode*)Mode);
	}
}
