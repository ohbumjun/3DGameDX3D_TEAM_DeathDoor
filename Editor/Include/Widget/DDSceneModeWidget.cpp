#include "DDSceneModeWidget.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUIInputFloat3.h"
#include "IMGUITextInput.h"
#include "IMGUIButton.h"
#include "Scene/SceneManager.h"
#include "Component/ColliderBox3D.h"
#include "PathManager.h"
#include "Engine.h"
#include "../EditorUtil.h"
#include "EngineUtil.h"

#include "../DeathDoor/Scene/DDSceneMode.h"

CDDSceneModeWidget::CDDSceneModeWidget()
{
}

CDDSceneModeWidget::~CDDSceneModeWidget()
{
}

bool CDDSceneModeWidget::Init()
{
	// Widget
	m_InitialScene = AddWidget<CIMGUICheckBox>("InitialScene", 200.f);
	m_EntryPointNameFind = AddWidget<CIMGUITextInput>("Entry Point Object Name", 200.f);
	AddWidget<CIMGUISameLine>("Line");
	m_FindEntryPoint = AddWidget<CIMGUIButton>("Find", 0.f, 0.f);
	m_EntryPoint = AddWidget<CIMGUIInputFloat3>("Entry Point", 200.f);

	m_ExitPointObjNameFind = AddWidget<CIMGUITextInput>("Find Exit Point Component Name", 200.f);
	AddWidget<CIMGUISameLine>("Line");
	m_FindExitPointObj = AddWidget<CIMGUIButton>("Find", 0.f, 0.f);
	m_ExitPointObj = AddWidget<CIMGUITextInput>("Exit Point Collider Name", 200.f);

	m_NextSceneFileName = AddWidget<CIMGUITextInput>("Next Scene File Name", 200.f);
	AddWidget<CIMGUISameLine>("Line");
	m_NextSceneFileSet = AddWidget<CIMGUIButton>("Set", 0.f, 0.f);

	// Initial Setting
	m_InitialScene->AddCheckInfo("Initial Scene");
	m_EntryPointNameFind->SetHintText("Put Object Name Here");
	m_ExitPointObjNameFind->SetHintText("Put Object Name Here");
	m_ExitPointObj->ReadOnly(true);
	m_NextSceneFileName->ReadOnly(true);

	// CallBack
	m_InitialScene->SetCallBackIdx(this, &CDDSceneModeWidget::OnCheckIntialScene);
	m_FindEntryPoint->SetClickCallback(this, &CDDSceneModeWidget::OnClickFindEntry);
	m_EntryPoint->SetCallBack(this, &CDDSceneModeWidget::OnChangeEntryPoint);
	m_FindExitPointObj->SetClickCallback(this, &CDDSceneModeWidget::OnClickFindExitPointObj);
	m_NextSceneFileSet->SetClickCallback(this, &CDDSceneModeWidget::OnClickSetNextScene);

	return true;
}

void CDDSceneModeWidget::SetSceneMode(CDDSceneMode* Mode)
{
	m_SceneMode = Mode;

	RefreshWidgets();
}

void CDDSceneModeWidget::RefreshWidgets()
{
	bool IsInitial = m_SceneMode->IsInitialScene();
	Vector3 EntryPoint = m_SceneMode->GetEntryPoint();
	std::string ExitObjName = m_SceneMode->GetExitPointObjectName();
	std::string NextSceneFileName = m_SceneMode->GetNextSceneFileName();

	m_InitialScene->SetCheck(0, IsInitial);
	m_EntryPoint->SetVal(EntryPoint);
	m_ExitPointObj->SetText(ExitObjName.c_str());
	m_NextSceneFileName->SetText(NextSceneFileName.c_str());
}

void CDDSceneModeWidget::OnCheckIntialScene(int Idx, bool Check)
{
	CDDSceneMode* CurSceneMode = (CDDSceneMode*)CSceneManager::GetInst()->GetScene()->GetSceneMode();
	CurSceneMode->SetInitialScene(Check);
}

void CDDSceneModeWidget::OnChangeEntryPoint(const Vector3& Val)
{
	CDDSceneMode* CurSceneMode = (CDDSceneMode*)CSceneManager::GetInst()->GetScene()->GetSceneMode();
	CurSceneMode->SetEntryPoint(Val);
}

void CDDSceneModeWidget::OnClickFindEntry()
{
	if (m_EntryPointNameFind->Empty())
	{
		return;
	}

	std::string Input = m_EntryPointNameFind->GetTextMultibyte();

	CGameObject* Found = FindObj(Input);

	if (!Found)
	{
		return;
	}

	CSceneComponent* Root = Found->GetRootComponent();

	if (Root)
	{
		Vector3 Pos = Root->GetWorldPos();
		m_EntryPoint->SetVal(Pos);
		m_SceneMode->SetEntryPoint(Pos);
	}
}

void CDDSceneModeWidget::OnClickFindExitPointObj()
{
	if (m_ExitPointObjNameFind->Empty())
	{
		return;
	}

	std::string Input = m_ExitPointObjNameFind->GetTextMultibyte();

	CGameObject* Found = FindObj(Input);

	if (!Found)
	{
		return;
	}

	CDDSceneMode* CurSceneMode = (CDDSceneMode*)CSceneManager::GetInst()->GetScene()->GetSceneMode();

	bool IsValidObj = CurSceneMode->SetExitPointObj(Found);

	// 해당 SceneMode에 맞는 Exit Object 형식이 아닌 경우
	// 기본적으로 ColliderBox3D를 가지고 있어야 함
	if (!IsValidObj)
	{
		MessageBox(nullptr, TEXT("현재 씬모드의 Exit Point 오브젝트 형식과 맞지 않음"), TEXT("Error"), MB_OK);
		return;
	}

	m_ExitPointObj->SetText(Input.c_str());
}

void CDDSceneModeWidget::OnClickSetNextScene()
{
	TCHAR FileFullPath[MAX_PATH] = {};
	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(SCENE_PATH)->Path;
	OpenFile.lpstrFilter = TEXT("씬 파일\0 * .scn\0");
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char FilePathMultibyte[MAX_PATH] = {};
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		std::string SceneFileName = CEngineUtil::ExtractFilePathFromFullPath(FilePathMultibyte, SCENE_PATH);

		CDDSceneMode* CurSceneMode = (CDDSceneMode*)CSceneManager::GetInst()->GetScene()->GetSceneMode();
		CurSceneMode->SetNextSceneFileName(SceneFileName);
		m_NextSceneFileName->SetText(SceneFileName.c_str());
		MessageBox(nullptr, TEXT("Next Scene 경로 지정 완료"), TEXT("OK"), MB_OK);
	}
}

CGameObject* CDDSceneModeWidget::FindObj(const std::string& Name)
{
	CScene* CurScene = CSceneManager::GetInst()->GetScene();
	CGameObject* Found = CurScene->FindObject(Name);

	return Found;
}
