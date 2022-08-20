#include "DDPuzzleSceneMode.h"
#include "Scene/Scene.h"
#include "Component/ColliderBox3D.h"
#include "Component/PaperBurnComponent.h"
#include "../Component/GameStateComponent.h"
#include "../Component/LadderCollider.h"
#include "Component/CameraComponent.h"
#include "Component/StaticMeshComponent.h"

CDDPuzzleSceneMode::CDDPuzzleSceneMode() : 
	m_ClearCamMove(false)
{
	SetTypeID<CDDPuzzleSceneMode>();
}

CDDPuzzleSceneMode::~CDDPuzzleSceneMode()
{
}

void CDDPuzzleSceneMode::Start()
{
	CDDSceneMode::Start();

	m_BlockerObj = m_Scene->FindObject(m_BlockerObjectName);

	if (m_BlockerObj)
	{
		m_BlockerOriginY = m_BlockerObj->GetWorldPos().y;
	}

	m_LadderObj = m_Scene->FindObject(m_LadderObjectName);

	if (m_LadderObj)
	{
		m_LadderPaperBurn = m_LadderObj->FindObjectComponentFromType<CPaperBurnComponent>();

		m_LadderObj->FindAllSceneComponentFromType<CLadderCollider>(m_vecLadderCollider);

		m_LadderObj->Enable(false);
	}
}

void CDDPuzzleSceneMode::Update(float DeltaTime)
{
	CDDSceneMode::Update(DeltaTime);

	if (m_BlockerObj && m_BlockerDownMoving)
	{
		// Blocker Down Moving
		Vector3 CurPos = m_BlockerObj->GetWorldPos();

		if (CurPos.y <= m_BlockerOriginY - 7.f)
		{
			m_BlockerDownMoving = false;
			m_BlockerObj->Enable(false);

			// 카메라 복귀 시작
			OnClearObjectEventEnd();
		}
		else
		{
			// Blocker UP Moving
			m_BlockerObj->AddWorldPos(0.f, -7.f * DeltaTime, 0.f);
		}
	}
}

void CDDPuzzleSceneMode::OnDieMonster()
{
	// 몬스터 죽는 경우는 아무 처리하지 않음
}

void CDDPuzzleSceneMode::OnClearDungeon()
{
	CDDSceneMode::OnClearDungeon();

	if (m_ClearCamMove)
	{
		// 카메라 Puzzle 위치로 이동하고, 이동 끝나면 창살이나 사다리 오브젝트 움직임 시작한다.
		CCameraComponent* Cam = m_Scene->GetCameraManager()->GetCurrentCamera();

		m_ClearOriginCamPos = Cam->GetWorldPos();

		Vector3 MoveDest;

		if (m_BlockerObj)
		{
			MoveDest = m_BlockerObj->GetWorldPos();
		}

		else if (m_LadderObj)
		{
			MoveDest = m_LadderObj->GetWorldPos();
		}

		MoveDest.y = m_ClearOriginCamPos.y;

		Cam->StartMove(m_ClearOriginCamPos, MoveDest, 1.f, false, true);
		Cam->SetMoveEndCallBack<CDDPuzzleSceneMode>(this, &CDDPuzzleSceneMode::OnClearCamMoveToClearObjectEnd);

		// 플레이어 잠깐 조작 불가, 히트박스 없앰
		CGameStateComponent* State = m_PlayerObject->FindObjectComponentFromType<CGameStateComponent>();
		State->SetTreeUpdate(false);
		CColliderBox3D* HitBox = (CColliderBox3D*)m_PlayerObject->FindComponent("Body");
		HitBox->Enable(false);
	}
	else
	{
		if (m_BlockerObj)
		{
			m_BlockerDownMoving = true;
		}

		else if (m_LadderObj)
		{
			m_LadderObj->Enable(true);
			m_LadderPaperBurn->SetEndEvent(PaperBurnEndEvent::Reset);
			m_LadderPaperBurn->SetInverse(true);
			m_LadderPaperBurn->StartPaperBurn();
			m_LadderPaperBurn->SetFinishCallback(this, &CDDPuzzleSceneMode::OnClearObjectEventEnd);
		}
	}

}

void CDDPuzzleSceneMode::OnClearCamMoveToClearObjectEnd()
{
	// 창살 혹은 사다리 이벤트 시작, 이벤트 끝나면 카메라 복귀
	if (m_BlockerObj)
	{
		m_BlockerDownMoving = true;
	}

	else if (m_LadderObj)
	{
		m_LadderObj->Enable(true);
		m_LadderPaperBurn->SetEndEvent(PaperBurnEndEvent::None);
		m_LadderPaperBurn->SetInverse(true);
		m_LadderPaperBurn->StartPaperBurn();
		m_LadderPaperBurn->SetFinishCallback(this, &CDDPuzzleSceneMode::OnClearObjectEventEnd);
	}
}

void CDDPuzzleSceneMode::OnClearObjectEventEnd()
{
	if (m_ClearCamMove)
	{
		// 클리어 오브젝트 이벤트 끝나면, 캠 복구
		CCameraComponent* Cam = m_Scene->GetCameraManager()->GetCurrentCamera();

		Cam->SetMoveFreeze(false);
		Cam->SetMoveReverse(true);
		Cam->SetMoveEndCallBack<CDDPuzzleSceneMode>(this, &CDDPuzzleSceneMode::OnClearCamRestoreEnd);
	}
}

void CDDPuzzleSceneMode::OnClearCamRestoreEnd()
{
	// 플레이어 조작 가능 상태로 변경
	CGameStateComponent* State = m_PlayerObject->FindObjectComponentFromType<CGameStateComponent>();
	State->SetTreeUpdate(true);
	CColliderBox3D* HitBox = (CColliderBox3D*)m_PlayerObject->FindComponent("Body");
	HitBox->Enable(true);
}

bool CDDPuzzleSceneMode::Save(FILE* File)
{
	CDDSceneMode::Save(File);

	fwrite(&m_ClearCamMove, sizeof(bool), 1, File);

	int Length = (int)m_BlockerObjectName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_BlockerObjectName.c_str(), sizeof(char), Length, File);

	Length = (int)m_LadderObjectName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_LadderObjectName.c_str(), sizeof(char), Length, File);

	return true;
}

bool CDDPuzzleSceneMode::Load(FILE* File)
{
	CDDSceneMode::Load(File);

	fread(&m_ClearCamMove, sizeof(bool), 1, File);

	int Length = 0;
	char Buf[128] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);
	m_BlockerObjectName = Buf;

	ZeroMemory(Buf, Length);
	fread(&Length, sizeof(int), 1, File);
	fread(Buf, sizeof(char), Length, File);
	m_LadderObjectName = Buf;

	return true;
}

bool CDDPuzzleSceneMode::SetBlockerObjectName(const std::string& Name)
{
	CGameObject* FindObj = m_Scene->FindObject(Name);

	if (!FindObj)
	{
		return false;
	}

	CColliderBox3D* Collider = FindObj->FindComponentFromType<CColliderBox3D>();
	CStaticMeshComponent* Mesh = FindObj->FindComponentFromType<CStaticMeshComponent>();

	if (!Collider || !Mesh)
	{
		return false;
	}

	m_BlockerObjectName = Name;

	return true;
}

bool CDDPuzzleSceneMode::SetLadderObjectName(const std::string& Name)
{
	CGameObject* FindObj = m_Scene->FindObject(Name);

	if (!FindObj)
	{
		return false;
	}

	CLadderCollider* Collider = FindObj->FindComponentFromType<CLadderCollider>();
	CStaticMeshComponent* Mesh = FindObj->FindComponentFromType<CStaticMeshComponent>();
	CPaperBurnComponent* PaperBurn = FindObj->FindComponentFromType<CPaperBurnComponent>();

	if (!Collider || !Mesh || !PaperBurn)
	{
		return false;
	}

	m_LadderObjectName = Name;

	return true;
}

CGameObject* CDDPuzzleSceneMode::GetBlockerObject() const
{
	return m_BlockerObj;
}
