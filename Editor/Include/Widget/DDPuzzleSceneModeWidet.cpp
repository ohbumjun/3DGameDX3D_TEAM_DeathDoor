#include "DDPuzzleSceneModeWidet.h"
#include "IMGUISameLine.h"
#include "IMGUITextInput.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUICheckBox.h"
#include "IMGUISeperator.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../DeathDoor/Scene/DDPuzzleSceneMode.h"

CDDPuzzleSceneModeWidet::CDDPuzzleSceneModeWidet()
{
}

CDDPuzzleSceneModeWidet::~CDDPuzzleSceneModeWidet()
{
}

bool CDDPuzzleSceneModeWidet::Init()
{
	CDDSceneModeWidget::Init();

	// Widget
	m_ClearCamMove = AddWidget<CIMGUICheckBox>("ClearCamMove", 200.f);

	m_BlockerObjName = AddWidget<CIMGUITextInput>("Blocker Obj Name", 200.f);
	m_BlockerObjNameInput = AddWidget<CIMGUITextInput>("Find Blocker Obj Name", 200.f);
	AddWidget<CIMGUISameLine>("Line");
	m_FindBlockerObj = AddWidget<CIMGUIButton>("Find", 0.f, 0.f);

	m_LadderObjName = AddWidget<CIMGUITextInput>("Ladder Obj Name", 200.f);
	m_LadderObjNameInput = AddWidget<CIMGUITextInput>("Find Ladder Obj Name", 200.f);
	AddWidget<CIMGUISameLine>("Line");
	m_FindLadderObj = AddWidget<CIMGUIButton>("Find", 0.f, 0.f);

	// Init
	m_ClearCamMove->AddCheckInfo("Clear Event Cam Move");
	m_BlockerObjName->ReadOnly(true);
	m_BlockerObjNameInput->SetHintText("Put Name");
	m_LadderObjName->ReadOnly(true);
	m_LadderObjNameInput->SetHintText("Put Name");

	// CallBack
	m_ClearCamMove->SetCallBackIdx(this, &CDDPuzzleSceneModeWidet::OnCheckClearCamMove);
	m_FindBlockerObj->SetClickCallback(this, &CDDPuzzleSceneModeWidet::OnClickFindBlokcerObj);
	m_FindLadderObj->SetClickCallback(this, &CDDPuzzleSceneModeWidet::OnClickFindLadderObj);

	return true;
}

void CDDPuzzleSceneModeWidet::RefreshWidgets()
{
	CDDSceneModeWidget::RefreshWidgets();

	CDDPuzzleSceneMode* SceneMode = dynamic_cast<CDDPuzzleSceneMode*>(m_SceneMode);

	bool ClearCamMove = SceneMode->IsClearCamMove();
	m_ClearCamMove->SetCheck(0, ClearCamMove);
	m_BlockerObjName->SetText(SceneMode->GetBlockerObjName().c_str());
	m_LadderObjName->SetText(SceneMode->GetLadderObjName().c_str());
}

void CDDPuzzleSceneModeWidet::OnCheckClearCamMove(int Idx, bool Enable)
{
	CDDPuzzleSceneMode* SceneMode = dynamic_cast<CDDPuzzleSceneMode*>(m_SceneMode);

	SceneMode->SetClearCamMove(Enable);
}

void CDDPuzzleSceneModeWidet::OnClickFindBlokcerObj()
{
	CDDPuzzleSceneMode* SceneMode = dynamic_cast<CDDPuzzleSceneMode*>(m_SceneMode);

	if (m_BlockerObjNameInput->Empty())
	{
		return;
	}

	std::string Input = m_BlockerObjNameInput->GetTextMultibyte();

	bool IsValidObj = SceneMode->SetBlockerObjectName(Input);

	// �ش� SceneMode�� �´� Exit Object ������ �ƴ� ���
	if (!IsValidObj)
	{
		MessageBox(nullptr, TEXT("���� ������� Blocker ������Ʈ ���İ� ���� ���� (ColliderBox3D, StaticMesh)"), TEXT("Error"), MB_OK);
		return;
	}

	m_BlockerObjName->SetText(Input.c_str());
}

void CDDPuzzleSceneModeWidet::OnClickFindLadderObj()
{
	CDDPuzzleSceneMode* SceneMode = dynamic_cast<CDDPuzzleSceneMode*>(m_SceneMode);

	if (m_LadderObjNameInput->Empty())
	{
		return;
	}

	std::string Input = m_LadderObjNameInput->GetTextMultibyte();

	bool IsValidObj = SceneMode->SetLadderObjectName(Input);

	// �ش� SceneMode�� �´� Exit Object ������ �ƴ� ���
	if (!IsValidObj)
	{
		MessageBox(nullptr, TEXT("���� ������� ��ٸ� ������Ʈ ���İ� ���� ���� (LadderCollider, StaticMesh, PaperBurn)"), TEXT("Error"), MB_OK);
		return;
	}

	m_LadderObjName->SetText(Input.c_str());
}
