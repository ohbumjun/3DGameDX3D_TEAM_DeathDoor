#include "KnightDataComponent.h"
#include "../DataManager.h"
#include "Component/ColliderBox3D.h"
#include "Component/CameraComponent.h"
#include "Component/PaperBurnComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../Component/PlayerDataComponent.h"
#include "../Component/GameStateComponent.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Render/RenderManager.h"
#include "ObjectPool.h"

CKnightDataComponent::CKnightDataComponent()	:
	m_JumpAttackRange(0.f),
	m_MeleeAttackCount(0)
{
	SetTypeID<CKnightDataComponent>();
}

CKnightDataComponent::CKnightDataComponent(const CKnightDataComponent& com)	:
	CMonsterDataComponent(com)
{
	m_JumpAttackRange = com.m_JumpAttackRange;
	m_MeleeAttackCount = 0;
}

CKnightDataComponent::~CKnightDataComponent()
{
}

void CKnightDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_Data = CDataManager::GetInst()->GetObjectData("BossKnight");

	m_ParticlePos = (CSceneComponent*)m_Object->FindComponent("ParticlePos");

	m_JumpAttackRange = m_Data.JumpAttackRange;
	
	m_MeleeAttackCollider->Enable(false);

	m_CutSceneCam->AddCutSceneMoveCallBack(3, CamMoveCallBackCallType::REACHED_POINT, this, &CKnightDataComponent::OnReachedCutSceneAnimPoint);
	m_CutSceneCam->AddCutSceneMoveEndCallBack<CKnightDataComponent>(this, &CKnightDataComponent::OnEndCutScene);
}

void CKnightDataComponent::Update(float DeltaTime)
{
	CMonsterDataComponent::Update(DeltaTime);

	if (m_DeathColorChangeStart)
	{
		ChangeColorBossDeath(DeltaTime);
	}
}

void CKnightDataComponent::OnLookPlayerMove(float DeltaTime)
{
	// �÷��̾� �������� ȸ�� ������ŭ ȸ��
	float AnglePlayer = GetAnglePlayer();

	CGameObject* MyObj = m_Object;

	if (abs(AnglePlayer) < m_Data.RotateSpeedPerSec)
	{
		MyObj->AddWorldRotationY(-AnglePlayer * DeltaTime);
	}
	else
	{
		MyObj->AddWorldRotationY(m_Data.RotateSpeedPerSec * DeltaTime);
	}

	// Walk Speed �ι�� �̵�
	MyObj->AddWorldPosByLocalAxis(AXIS::AXIS_Z, -1.f * m_Data.MoveSpeed * 2.f);

	// ���� ���� �� ���� ����
	Vector3 FinalZAxis = ToPlayer();
	m_FinalTraceZAxis = FinalZAxis;
	m_FinalTraceZAxis.Normalize();
}

// ���� ����, �÷��̾���� ������ ������ �̾��, ������ �Ǵ��ϴ� �Լ�
void CKnightDataComponent::OnEndAnimJudgeContinueAttack()
{
	if (m_MeleeAttackCount >= 2)
	{
		SetContinueAttack(false);
		SetPostAttckDelaying(false);
	}
	// �ִϸ��̼� ���� ���� �÷��̾ ���� ���� ���� ���� ���� ���
	else if (IsPlayerInMeleeAttackRange())
	{
		// ���� �̾�� ���·�
		SetContinueAttack(true);

		// ���� ���� ���� �Ǵ�
		float AnglePlayer = GetAnglePlayer();

		// �÷��̾���� ������ ���� ������ ���
		if (abs(AnglePlayer) <= 45.f)
		{
			// ���� ��带 Null��
			SetCurrentNodeNull();
			SetAttackRot(Knight_Attack_Rot_Type::Front);
			return;
		}

		CGameObject* Player = m_Object->GetScene()->GetPlayerObject();

		if (!Player)
		{
			return;
		}

		bool IsPlayerLeft = IsPlayerLeftBasedInLookDir();

		if (IsPlayerLeft)
		{
			SetAttackRot(Knight_Attack_Rot_Type::CCW);
		}
		else
		{
			SetAttackRot(Knight_Attack_Rot_Type::CW);
		}
	}
	else
	{
		// �ĵ����� ���·� ��ȯ
		SetPostAttckDelaying(true);
		SetContinueAttack(false);
	}

	// ���� ��带 Null��
	SetCurrentNodeNull();
}

void CKnightDataComponent::OnEnableLookAndMove()
{
	OnPlayWalkSound();
	m_MoveZ = true;
	m_LookPlayer = true;
	m_CurMoveSpeed = m_Data.MoveSpeed;
}

void CKnightDataComponent::OnDisableLookAndMove()
{
	m_MoveZ = false;
	m_LookPlayer = false;
	m_CurMoveSpeed = 0.f;
}

void CKnightDataComponent::OnWalk(float DeltaTime)
{
	LookPlayer(DeltaTime);

	m_Object->AddWorldPosByLocalAxis(AXIS::AXIS_Z, -m_Data.MoveSpeed * DeltaTime);
}

void CKnightDataComponent::OnStartJumpAttackMove()
{
	m_CurMoveSpeed = m_Data.MoveSpeed * 5.f;
	m_MoveZ = true;
	m_LookPlayer = true;
}

void CKnightDataComponent::OnEndJumpAttackMove()
{
	CResourceManager::GetInst()->SoundPlay("BossKnightJump");
	m_CurMoveSpeed = m_Data.MoveSpeed * 3.f;
	m_LookPlayer = false;
}

void CKnightDataComponent::OnEndJumpAttack()
{
	m_CurMoveSpeed = 0.f;
	m_MoveZ = false;
}

void CKnightDataComponent::OnEndContinueAttack()
{
	SetCurrentNodeNull();
	m_ContinueAttack = false;
}

void CKnightDataComponent::OnCutSceneSlamFloor()
{
	CResourceManager::GetInst()->SoundPlay("BossKnightCutsceneSlam");
	m_Scene->GetCameraManager()->ShakeCamera(0.5f, 2.f);
}

void CKnightDataComponent::OnDeadAnimStart()
{
	CMonsterDataComponent::OnDeadAnimStart();

	CResourceManager::GetInst()->SoundPlay("BossKnightDeath");

	m_AnimMesh->GetAnimationInstance()->GetCurrentAnimation()->SetPlayScale(0.5f);
	m_DeathColorChangeTimeMax = m_AnimMesh->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationPlayTime() * 0.5f;
}

void CKnightDataComponent::OnDeadPaperBurnEnd()
{
	CResourceManager::GetInst()->SoundStop("KnightBossBattleBGM");
	CMonsterDataComponent::OnDeadPaperBurnEnd();
}

void CKnightDataComponent::OnReachedCutSceneAnimPoint()
{
	m_AnimMesh->GetAnimationInstance()->ChangeAnimation("MegaSlam");
}

void CKnightDataComponent::OnStartCutScene()
{
	CMonsterDataComponent::OnStartCutScene();

	m_OriginDOFMin = CRenderManager::GetInst()->GetDOFMin();
	m_OriginDOFMax = CRenderManager::GetInst()->GetDOFMax();

	CRenderManager::GetInst()->SetDOFMin(8.f);
	CRenderManager::GetInst()->SetDOFMax(60.f);
}

void CKnightDataComponent::OnEndCutScene()
{
	CMonsterDataComponent::OnEndCutScene();

	CRenderManager::GetInst()->SetDOFMin(m_OriginDOFMin);
	CRenderManager::GetInst()->SetDOFMax(m_OriginDOFMax);

	CResourceManager::GetInst()->SoundPlay("KnightBossBattleBGM");
}

void CKnightDataComponent::OnPlayerEnterZone(const CollisionResult& Result)
{
	CMonsterDataComponent::OnPlayerEnterZone(Result);

	CResourceManager::GetInst()->SoundStop("MushroomDungeonBGM");
	CResourceManager::GetInst()->SoundPlay("BossKnightAmbientBGM");
}

void CKnightDataComponent::OnActiveMeleeAttackCollider()
{
	CMonsterDataComponent::OnActiveMeleeAttackCollider();

	int Rand = rand() % 5;
	std::string Key = "BossKnightSlam" + std::to_string(Rand);
	CResourceManager::GetInst()->SoundPlay(Key);

	CGameObject* Particle = CObjectPool::GetInst()->GetParticle("BossKnightImpact", m_Scene);

	Vector3 Pos = m_ParticlePos->GetWorldPos();
	Particle->StartParticle(Pos);

	m_Scene->GetCameraManager()->ShakeCamera(0.5f, 2.f);
}

void CKnightDataComponent::OnPlayWalkSound()
{
	int WalkSound = m_WalkSoundCount % 2;
	std::string Key = "BossKnightStep" + std::to_string(WalkSound);
	CResourceManager::GetInst()->SoundPlay(Key);
	++m_WalkSoundCount;
}
