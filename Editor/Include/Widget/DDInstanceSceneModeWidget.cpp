#include "DDInstanceSceneModeWidget.h"
#include "IMGUISameLine.h"
#include "IMGUIInputFloat3.h"
#include "IMGUITextInput.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUISeperator.h"
#include "IMGUIListBox.h"
#include "IMGUIInputFloat.h"
#include "IMGUIChild.h"
#include "IMGUIComboBox.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Component/ColliderBox3D.h"
#include "../EditorManager.h"
#include "../Window/ToolWindow.h"
#include "ObjectPool.h"
#include "Component/AnimationMeshComponent.h"

CDDInstanceSceneModeWidget::CDDInstanceSceneModeWidget()	:
	m_CurSelectPhaseIdx(-1)
{
}

CDDInstanceSceneModeWidget::~CDDInstanceSceneModeWidget()
{
}

bool CDDInstanceSceneModeWidget::Init()
{
	CDDSceneModeWidget::Init();

	// Widget
	m_EnterColliderObjName = AddWidget<CIMGUITextInput>("Enter Collider Obj Name", 200.f);
	m_EnterColliderObjNameInput = AddWidget<CIMGUITextInput>("Find Obj Name", 200.f);
	AddWidget<CIMGUISameLine>("Line");
	m_FindEnterColliderObj = AddWidget<CIMGUIButton>("Find", 0.f, 0.f);

	m_BlockerObjName = AddWidget<CIMGUITextInput>("Blocker Obj Name", 200.f);
	m_BlockerObjNameInput = AddWidget<CIMGUITextInput>("Find Obj Name", 200.f);
	AddWidget<CIMGUISameLine>("Line");
	m_FindBlockerObj = AddWidget<CIMGUIButton>("Find", 0.f, 0.f);

	CIMGUIText* Text = AddWidget<CIMGUIText>("Text");
	Text->SetText("End Event ObjList (PaperBurn)");
	m_EndEventObjList = AddWidget<CIMGUIListBox>("EndEventObjList", 200.f);
	m_EndEventObjNameInput = AddWidget<CIMGUITextInput>("End Event Obj Name", 200.f);
	m_AddEndEventObj = AddWidget<CIMGUIButton>("Add", 0.f, 0.f);
	AddWidget<CIMGUISameLine>("Line");
	m_ClearEndEventObj = AddWidget<CIMGUIButton>("Clear", 0.f, 0.f);

	AddWidget<CIMGUISeperator>("Sep");
	Text = AddWidget<CIMGUIText>("Phase List");
	Text->SetText("Instance Dungeon Spawn Info");

	CIMGUIChild* Block = AddWidget<CIMGUIChild>("PhaseBlock", 400.f, 400.f);
	Block->EnableBorder(true);
	Block->SetSameLine(true);
	Text = Block->AddWidget<CIMGUIText>("Phase List");
	Text->SetText("Phase List");
	m_PhaseList = Block->AddWidget<CIMGUIListBox>("Phase List", 200.f);
	m_AddPhase = Block->AddWidget<CIMGUIButton>("Add", 0.f, 0.f);
	Block->AddWidget<CIMGUISameLine>("Line");
	m_DeletePhase = Block->AddWidget<CIMGUIButton>("Delete", 0.f, 0.f);

	Block = AddWidget<CIMGUIChild>("SpawnList Block", 400.f, 500.f);
	Block->EnableBorder(true);
	Block->SetSameLine(true);
	m_PhaseInterval = Block->AddWidget<CIMGUIInputFloat>("Phase Interval", 200.f);
	Text = Block->AddWidget<CIMGUIText>("Spawn Info List");
	Text->SetText("Spawn Info List");
	m_SpawnList = Block->AddWidget<CIMGUIListBox>("Spawn Info List", 200.f);
	m_AddSpawnInfo = Block->AddWidget<CIMGUIButton>("Add", 0.f, 0.f);
	Block->AddWidget<CIMGUISameLine>("Line");
	m_DeleteSpawnInfo = Block->AddWidget<CIMGUIButton>("Delete", 0.f, 0.f);

	Block = AddWidget<CIMGUIChild>("SpawnInfo Block", 400.f, 500.f);
	Block->EnableBorder(true);
	Block->SetSameLine(true);
	Text = Block->AddWidget<CIMGUIText>("Spawn Info List");
	Text->SetText("Spawn Info");
	m_PoolMonsterNames = Block->AddWidget<CIMGUIComboBox>("Pool Monster Names");
	Block->AddWidget<CIMGUISameLine>("Line");
	m_SetPoolMonster = Block->AddWidget<CIMGUIButton>("Add Monster", 0.f, 0.f);
	m_SpawnPoint = Block->AddWidget<CIMGUIInputFloat3>("Spawn Point", 200.f);
	m_SpawnRotation = Block->AddWidget<CIMGUIInputFloat3>("Spawn Rotation", 200.f);
	m_SpawnInterval = Block->AddWidget<CIMGUIInputFloat>("Spawn Interval", 200.f);

	// Initial Setting
	m_PhaseList->SetPageItemCount(10);
	m_PhaseList->SetHideName(true);
	m_SpawnList->SetPageItemCount(10);
	m_SpawnList->SetHideName(true);
	m_EnterColliderObjName->ReadOnly(true);
	m_EnterColliderObjNameInput->SetHintText("Put Name");
	m_BlockerObjName->ReadOnly(true);
	m_BlockerObjNameInput->SetHintText("Put Name");
	m_EndEventObjList->SetPageItemCount(5);
	m_EndEventObjList->SetHideName(true);

	std::vector<std::string> vecPoolMonsterNames;
	CObjectPool::GetInst()->GetAllMonsterNames(vecPoolMonsterNames);
	size_t Size = vecPoolMonsterNames.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_PoolMonsterNames->AddItem(vecPoolMonsterNames[i]);
	}

	// CallBack
	m_FindEnterColliderObj->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickFindEnterColliderObj);
	m_FindBlockerObj->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickFindBlokcerObj);
	m_PhaseList->SetSelectCallback(this, &CDDInstanceSceneModeWidget::OnSelectPhase);
	m_AddPhase->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickAddPhase);
	m_DeletePhase->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickDeletePhase);
	m_PhaseInterval->SetCallBack(this, &CDDInstanceSceneModeWidget::OnChangePhaseInterval);
	m_SpawnList->SetSelectCallback(this, &CDDInstanceSceneModeWidget::OnSelectSpawnList);
	m_AddSpawnInfo->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickAddSpawnInfo);
	m_DeleteSpawnInfo->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickDeleteSpawnInfo);
	m_SpawnPoint->SetCallBack(this, &CDDInstanceSceneModeWidget::OnChangeSetSpawnPoint);
	m_SpawnRotation->SetCallBack(this, &CDDInstanceSceneModeWidget::OnChangeSetSpawnRot);
	m_SpawnInterval->SetCallBack(this, &CDDInstanceSceneModeWidget::OnChangeSpawnInterval);
	m_SetPoolMonster->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickSetMonster);
	m_AddEndEventObj->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickAddEndEventObj);
	m_ClearEndEventObj->SetClickCallback(this, &CDDInstanceSceneModeWidget::OnClickClearEndEventObj);

	return true;
}

void CDDInstanceSceneModeWidget::RefreshWidgets()
{
	CDDSceneModeWidget::RefreshWidgets();

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	m_EnterColliderObjName->SetText(SceneMode->GetEnterTriggerObjName().c_str());
	m_BlockerObjName->SetText(SceneMode->GetBlockerObjName().c_str());

	m_PhaseList->Clear();

	// TODO : GetNextScene 함수가 nullptr 리턴
	CScene* Scene = CSceneManager::GetInst()->GetNextScene();

	size_t Size = SceneMode->GetPhaseListSize();
	for (size_t i = 0; i < Size; ++i)
	{
		m_PhaseList->AddItem(std::to_string(i));

		// 샘플 오브젝트를 생성한다.
		m_vecSpawnMonster.push_back(std::vector<CGameObject*>());

		DDInstanceSpawnPhaseInfo* Phase = SceneMode->GetPhaseInfo((int)i);

		auto iter = Phase->SpawnList.begin();
		auto iterEnd = Phase->SpawnList.end();
		for (; iter != iterEnd; ++iter)
		{
			CGameObject* Sample = CreatePhaseMonsterSample(i, *iter, Scene);
			if (Sample)
			{
				Sample->Enable(false);
			}
		}
	}
}

void CDDInstanceSceneModeWidget::OnClickFindEnterColliderObj()
{
	if (m_EnterColliderObjNameInput->Empty())
	{
		return;
	}

	CScene* Scene = CSceneManager::GetInst()->GetScene();

	std::string Name = m_EnterColliderObjNameInput->GetTextMultibyte();

	CGameObject* Find = Scene->FindObject(Name);

	if (!Find)
	{
		MessageBox(nullptr, TEXT("오브젝트 없음"), TEXT("Error"), MB_OK);
		return;
	}

	CColliderBox3D* Collider = Find->FindComponentFromType<CColliderBox3D>();

	if (!Collider)
	{
		MessageBox(nullptr, TEXT("Box Collider 없음"), TEXT("Error"), MB_OK);
		return;
	}

	dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode)->SetEnterColliderObjectName(Name);
	m_EnterColliderObjName->SetText(Name.c_str());
	MessageBox(nullptr, TEXT("Enter Trigger 등록"), TEXT("Success"), MB_OK);
}

void CDDInstanceSceneModeWidget::OnClickFindBlokcerObj()
{
	if (m_BlockerObjNameInput->Empty())
	{
		return;
	}

	CScene* Scene = CSceneManager::GetInst()->GetScene();

	std::string Name = m_BlockerObjNameInput->GetTextMultibyte();

	CGameObject* Find = Scene->FindObject(Name);

	if (!Find)
	{
		MessageBox(nullptr, TEXT("오브젝트 없음"), TEXT("Error"), MB_OK);
		return;
	}

	CColliderBox3D* Collider = Find->FindComponentFromType<CColliderBox3D>();

	if (!Collider)
	{
		MessageBox(nullptr, TEXT("Box Collider 없음"), TEXT("Error"), MB_OK);
		return;
	}

	dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode)->SetBlockerObjectName(Name);
	m_BlockerObjName->SetText(Name.c_str());
	MessageBox(nullptr, TEXT("창살 오브젝트 등록"), TEXT("Success"), MB_OK);
}

void CDDInstanceSceneModeWidget::OnSelectPhase(int Idx, const char* Label)
{
	if (m_CurSelectPhaseIdx != -1)
	{
		// 이전에 선택된 페이즈 몬스터 샘플 Disable
		EnablePhaseMonsterSamples(m_CurSelectPhaseIdx, false);
	}

	m_CurSelectPhaseIdx = Idx;

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	DDInstanceSpawnPhaseInfo* PhaseInfo = SceneMode->GetPhaseInfo(Idx);

	m_PhaseInterval->SetVal(PhaseInfo->Interval);

	m_SpawnList->Clear();

	size_t SpawnListCount = PhaseInfo->SpawnList.size();

	for (size_t i = 0; i < SpawnListCount; ++i)
	{
		m_SpawnList->AddItem(std::to_string(i));
	}

	// 현재 선택된 페이즈 몬스터 샘플 Enable
	EnablePhaseMonsterSamples(m_CurSelectPhaseIdx, true);

	SetGizmoNull();

	m_CurSpawnInfo = nullptr;
}

void CDDInstanceSceneModeWidget::OnClickAddPhase()
{
	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	SceneMode->AddSpawnPhase();

	int PhaseListSize = SceneMode->GetPhaseListSize();

	m_PhaseList->AddItem(std::to_string(PhaseListSize - 1));

	// 샘플 몬스터 벡터 사이즈 + 1
	size_t Size = m_vecSpawnMonster.size();
	m_vecSpawnMonster.resize(Size + 1);
}

void CDDInstanceSceneModeWidget::OnClickDeletePhase()
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();

	if (SelectPhaseIdx == -1)
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	bool Success = SceneMode->DeleteSpawnPhaseInfo(SelectPhaseIdx);

	if (!Success)
	{
		MessageBox(nullptr, TEXT("삭제 실패"), TEXT("Error"), MB_OK);
	}

	m_PhaseList->Clear();

	int PhaseListSize = SceneMode->GetPhaseListSize();

	for (int i = 0; i < PhaseListSize; ++i)
	{
		m_PhaseList->AddItem(std::to_string(i));
	}

	m_PhaseInterval->SetVal(0.f);
	m_SpawnList->Clear();
	m_SpawnPoint->SetVal(0.f, 0.f, 0.f);
	m_SpawnInterval->SetVal(0.f);

	// 현재 페이즈의 샘플 몬스터 오브젝트 삭제
	DestroyPhaseMonsterSample(SelectPhaseIdx);

	m_CurSelectPhaseIdx = -1;

	m_CurSpawnInfo = nullptr;

	m_PhaseList->SetSelectIndex(-1);
}

void CDDInstanceSceneModeWidget::OnChangePhaseInterval(float Val)
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();

	if (SelectPhaseIdx == -1)
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	SceneMode->SetSpawnPhaseInterval(SelectPhaseIdx, Val);
}

void CDDInstanceSceneModeWidget::OnSelectSpawnList(int Idx, const char* Label)
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	DDInstanceSpawnInfo* SpawnInfo = SceneMode->GetSpawnInfo(SelectPhaseIdx, Idx);

	m_SpawnPoint->SetVal(SpawnInfo->SpawnPosition);
	m_SpawnInterval->SetVal(SpawnInfo->Interval);

	if (m_vecSpawnMonster[SelectPhaseIdx][Idx])
	{
		SetGizmo(SelectPhaseIdx, Idx);
	}

	m_CurSpawnInfo = SpawnInfo;
}

void CDDInstanceSceneModeWidget::OnClickAddSpawnInfo()
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();

	if (SelectPhaseIdx == -1)
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	SceneMode->AddSpawnInfo(SelectPhaseIdx);
	
	int SpawnListSize = (int)SceneMode->GetSpawnListSize(SelectPhaseIdx);

	m_SpawnList->AddItem(std::to_string(SpawnListSize - 1));

	// 해당 페이즈 샘플 몬스터 벡터 리사이즈
	size_t Size = m_vecSpawnMonster[SelectPhaseIdx].size();
	m_vecSpawnMonster[SelectPhaseIdx].resize(Size + 1);
}

void CDDInstanceSceneModeWidget::OnClickDeleteSpawnInfo()
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();
	int SelectSpawnIdx = m_SpawnList->GetSelectIndex();

	if (SelectPhaseIdx == -1 || SelectSpawnIdx == -1)
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	bool Success = SceneMode->DeleteSpawnInfo(SelectPhaseIdx, SelectSpawnIdx);

	if (!Success)
	{
		MessageBox(nullptr, TEXT("삭제 실패"), TEXT("Error"), MB_OK);
	}

	int SpawnListSize = (int)SceneMode->GetSpawnListSize(SelectPhaseIdx);

	m_SpawnList->Clear();
	for (int i = 0; i < SpawnListSize; ++i)
	{
		m_SpawnList->AddItem(std::to_string(i));
	}

	m_SpawnPoint->SetVal(0.f, 0.f, 0.f);
	m_SpawnInterval->SetVal(0.f);

	// 해당 스폰 몬스터 샘플을 삭제하고, 벡터를 리사이징 한다.
	CGameObject* SampleMonster = m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx];

	for (int i = SelectSpawnIdx + 1; i < SpawnListSize + 1; ++i)
	{
		m_vecSpawnMonster[SelectPhaseIdx][(size_t)i - 1] = m_vecSpawnMonster[SelectPhaseIdx][i];
	}

	m_vecSpawnMonster[SelectPhaseIdx].resize(SpawnListSize);

	// 기즈모 선택 해제
	SetGizmoNull();

	// 오브젝트 삭제
	if(SampleMonster)
		SampleMonster->Destroy();

	m_CurSpawnInfo = nullptr;

	m_SpawnList->SetSelectIndex(-1);
}

void CDDInstanceSceneModeWidget::OnChangeSetSpawnPoint(const Vector3& Val)
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();
	int SelectSpawnIdx = m_SpawnList->GetSelectIndex();

	if (SelectPhaseIdx == -1 || SelectSpawnIdx == -1)
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	SceneMode->SetSpawnPoint(SelectPhaseIdx, SelectSpawnIdx, Val);

	// OBJ 추가
	if (m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx])
	{
		m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->SetWorldPos(Val);
	}
}

void CDDInstanceSceneModeWidget::OnChangeSetSpawnRot(const Vector3& Val)
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();
	int SelectSpawnIdx = m_SpawnList->GetSelectIndex();

	if (SelectPhaseIdx == -1 || SelectSpawnIdx == -1)
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	SceneMode->SetSpawnRotation(SelectPhaseIdx, SelectSpawnIdx, Val);
	m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->SetWorldRotation(Val);
}

void CDDInstanceSceneModeWidget::OnChangeSpawnInterval(float Val)
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();
	int SelectSpawnIdx = m_SpawnList->GetSelectIndex();

	if (SelectPhaseIdx == -1 || SelectSpawnIdx == -1)
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	SceneMode->SetSpawnInterval(SelectPhaseIdx, SelectSpawnIdx, Val);
}

void CDDInstanceSceneModeWidget::OnClickSetMonster()
{
	int SelectPhaseIdx = m_PhaseList->GetSelectIndex();
	int SelectSpawnIdx = m_SpawnList->GetSelectIndex();
	int SelectComboIdx = m_PoolMonsterNames->GetSelectIndex();

	if (SelectPhaseIdx == -1 || SelectSpawnIdx == -1 || SelectComboIdx == -1)
	{
		return;
	}

	CScene* Scene = CSceneManager::GetInst()->GetScene();

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	std::string CurSpawnMonsterName = SceneMode->GetSpawnInfo(SelectPhaseIdx, SelectSpawnIdx)->MonsterName;
	std::string MonsterName = m_PoolMonsterNames->GetSelectItem();

	if (CurSpawnMonsterName == MonsterName)
	{
		return;
	}

	CGameObject* PoolMonster = CObjectPool::GetInst()->GetMonster(MonsterName, Scene);

	CAnimationMeshComponent* AnimMesh = PoolMonster->FindComponentFromType<CAnimationMeshComponent>();

	// 스폰 위치를 나타내기 위해, 메쉬컴포넌트만 가지고 있는 오브젝트들을 맵에 반투명하게 띄워준다.
	// 현재 스폰 몬스터 정보가 이미 있는 경우 메쉬가 달라지면 메쉬만 갈아끼워준다.
	if (m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx])
	{
		CAnimationMeshComponent* CurMeshCom = m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->FindComponentFromType<CAnimationMeshComponent>();
		CAnimationMesh* CurMesh = CurMeshCom->GetMesh();
		CAnimationMesh* NewMesh = AnimMesh->GetMesh();

		if (CurMesh == NewMesh)
		{
			PoolMonster->Destroy();
			return;
		}
		else
		{
			CurMeshCom->SetMesh(NewMesh);
			CAnimationSequenceInstance* AnimInst = AnimMesh->GetAnimationInstance();
			CurMeshCom->SetAnimationInstance(AnimInst->Clone());
			CurMeshCom->SetTransparencyAllMaterial(true);
			CurMeshCom->SetOpacity(0.5f);
			m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->SetName(PoolMonster->GetName());
		}
	}
	// 현재 스폰 정보에 처음 몬스터 정보가 채워지는 경우 새로운 오브젝트를 생성한다
	else
	{
		Vector3 OriginScale = PoolMonster->GetWorldScale();

		CGameObject* NewObject = Scene->CreateEmtpyObject();
		NewObject->ExcludeFromSceneSave();

		CAnimationMeshComponent* NewAnimMesh = NewObject->CreateComponentAddChild<CAnimationMeshComponent>("AnimMesh");
		CAnimationMesh* Mesh = AnimMesh->GetMesh();
		CAnimationSequenceInstance* AnimInst = AnimMesh->GetAnimationInstance();
		NewAnimMesh->SetMesh(Mesh);
		NewAnimMesh->SetAnimationInstance(AnimInst->Clone());
		NewAnimMesh->GetMaterial()->GetCBuffer()->SetAnimation3D(true);
		NewAnimMesh->SetTransparencyAllMaterial(true);
		NewAnimMesh->SetOpacity(0.5f);
		NewObject->SetWorldScale(OriginScale);
		NewObject->SetName(PoolMonster->GetName());

		m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx] = NewObject;
	}

	// 스폰 정보 갱신
	SceneMode->SetSpawnMonsterName(SelectPhaseIdx, SelectSpawnIdx, MonsterName);

	// 기본값 설정
	Vector3 WidgetPos = m_SpawnPoint->GetValue();
	Vector3 WidgetRot = m_SpawnRotation->GetValue();
	m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->SetWorldPos(WidgetPos);
	m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->SetWorldRotation(WidgetRot);

	// 위젯 콜백 설정
	m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->GetRootComponent()->GetTransform()->AddChangePosCallBack(this, &CDDInstanceSceneModeWidget::OnChangeSpawnMonsterPos);
	m_vecSpawnMonster[SelectPhaseIdx][SelectSpawnIdx]->GetRootComponent()->GetTransform()->AddChangeRotCallBack(this, &CDDInstanceSceneModeWidget::OnChangeSpawnMonsterRot);

	// 기즈모 세팅
	SetGizmo(SelectPhaseIdx, SelectSpawnIdx);

	// 원본 오브젝트는 씬에서 삭제
	PoolMonster->Destroy();
}

void CDDInstanceSceneModeWidget::OnClickAddEndEventObj()
{
	std::string InputObjName = m_EndEventObjNameInput->GetTextMultibyte();

	if (InputObjName.empty())
	{
		return;
	}

	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	bool IsValidObj = SceneMode->AddEndEventObjName(InputObjName);

	if (!IsValidObj)
	{
		MessageBox(nullptr, TEXT("오브젝트 형식과 맞지 않음 (Paperburn Component 있어야 함"), TEXT("Error"), MB_OK);
		return;
	}
	
	m_EndEventObjList->AddItem(InputObjName);
}

void CDDInstanceSceneModeWidget::OnClickClearEndEventObj()
{
	CDDInstanceSceneMode* SceneMode = dynamic_cast<CDDInstanceSceneMode*>(m_SceneMode);

	SceneMode->ClearEndEventObj();
	m_EndEventObjList->Clear();
}

void CDDInstanceSceneModeWidget::OnChangeSpawnMonsterPos(const Vector3& WorldPos, const Vector3& RelativePos)
{
	m_SpawnPoint->SetVal(WorldPos);

	m_CurSpawnInfo->SpawnPosition = WorldPos;
}

void CDDInstanceSceneModeWidget::OnChangeSpawnMonsterRot(const Vector3& WorldRot, const Vector3& RelativeRot)
{
	m_SpawnRotation->SetVal(WorldRot);

	m_CurSpawnInfo->SpawnRotation = WorldRot;
}

CGameObject* CDDInstanceSceneModeWidget::CreatePhaseMonsterSample(int PhaseIdx, DDInstanceSpawnInfo* Info, class CScene* Scene)
{
	if (Info->MonsterName.empty())
	{
		return nullptr;
	}

	CGameObject* PoolMonster = CObjectPool::GetInst()->GetMonster(Info->MonsterName, Scene, false);

	CAnimationMeshComponent* AnimMesh = PoolMonster->FindComponentFromType<CAnimationMeshComponent>();

	Vector3 OriginScale = PoolMonster->GetWorldScale();

	CGameObject* NewObject = Scene->CreateEmtpyObject();
	NewObject->ExcludeFromSceneSave();

	CAnimationMeshComponent* NewAnimMesh = NewObject->CreateComponentAddChild<CAnimationMeshComponent>("AnimMesh");
	CAnimationMesh* Mesh = AnimMesh->GetMesh();
	CAnimationSequenceInstance* AnimInst = AnimMesh->GetAnimationInstance();
	NewAnimMesh->SetMesh(Mesh);
	NewAnimMesh->SetAnimationInstance(AnimInst->Clone());
	NewAnimMesh->GetMaterial()->GetCBuffer()->SetAnimation3D(true);
	NewAnimMesh->SetTransparencyAllMaterial(true);
	NewAnimMesh->SetOpacity(0.5f);
	NewObject->SetWorldScale(OriginScale);
	NewObject->SetName(PoolMonster->GetName());

	m_vecSpawnMonster[PhaseIdx].push_back(NewObject);

	NewObject->SetWorldPos(Info->SpawnPosition);
	NewObject->SetWorldRotation(Info->SpawnRotation);

	// 위젯 콜백 설정
	NewObject->GetRootComponent()->GetTransform()->AddChangePosCallBack(this, &CDDInstanceSceneModeWidget::OnChangeSpawnMonsterPos);
	NewObject->GetRootComponent()->GetTransform()->AddChangeRotCallBack(this, &CDDInstanceSceneModeWidget::OnChangeSpawnMonsterRot);

	// 원본 오브젝트는 씬에서 삭제
	PoolMonster->Destroy();

	return NewObject;
}

void CDDInstanceSceneModeWidget::EnablePhaseMonsterSamples(int PhaseIdx, bool Enable)
{
	size_t Size = m_vecSpawnMonster[PhaseIdx].size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecSpawnMonster[PhaseIdx][i])
		{
			m_vecSpawnMonster[PhaseIdx][i]->Enable(Enable);
		}
	}
}

void CDDInstanceSceneModeWidget::DestroyPhaseMonsterSample(int PhaseIdx)
{
	size_t Size = m_vecSpawnMonster[PhaseIdx].size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecSpawnMonster[PhaseIdx][i])
		{
			m_vecSpawnMonster[PhaseIdx][i]->Destroy();
		}
	}

	// 삭제된 페이즈 뒤의 페이즈들을 앞으로 당겨오고, 리사이즈한다.
	size_t Size2 = m_vecSpawnMonster.size();
	for (size_t i = (size_t)PhaseIdx + 1; i < Size2; ++i)
	{
		size_t SampleMonsterCount = m_vecSpawnMonster[i].size();

		m_vecSpawnMonster[i - 1].clear();
		m_vecSpawnMonster[i - 1].resize(SampleMonsterCount);

		for (size_t j = 0; j < SampleMonsterCount; ++j)
		{
			m_vecSpawnMonster[i - 1][j] = m_vecSpawnMonster[i][j];
		}
	}

	m_vecSpawnMonster.resize(Size2 - 1);
}

void CDDInstanceSceneModeWidget::SetGizmo(int PhaseIdx, int SpawnIdx)
{
	CToolWindow* ToolWindow = CEditorManager::GetInst()->GetToolWindow();
	ToolWindow->SetGizmoObject(m_vecSpawnMonster[PhaseIdx][SpawnIdx]);
}

void CDDInstanceSceneModeWidget::SetGizmoNull()
{
	CToolWindow* ToolWindow = CEditorManager::GetInst()->GetToolWindow();
	ToolWindow->SetGizmoObject(nullptr);
}
