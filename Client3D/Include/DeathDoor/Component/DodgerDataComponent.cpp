#include "DodgerDataComponent.h"
#include "../DataManager.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "PlayerDataComponent.h"
#include "Component/ColliderBox3D.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/StaticMeshComponent.h"
#include "Component/PaperBurnComponent.h"
#include "MonsterNavAgent.h"

CDodgerDataComponent::CDodgerDataComponent()	:
	m_AttackCount(0),
	m_DashReady(false),
	m_DashRotaiting(false)
{
	SetTypeID<CDodgerDataComponent>();
}

CDodgerDataComponent::CDodgerDataComponent(const CDodgerDataComponent& com)	:
	CMonsterDataComponent(com)
{
	m_AttackCount = 0;
	m_DashReady = false;
}

CDodgerDataComponent::~CDodgerDataComponent()
{
}

void CDodgerDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_Data = CDataManager::GetInst()->GetObjectData("Dodger");

	m_StopChaseRange = m_Data.MeleeAttackRange;

	m_SlashLine = m_Object->FindComponentFromType<CStaticMeshComponent>();
	m_SlashLine->SetDrawShadow(false);
	m_SlashLinePaperBurn = (CPaperBurnComponent*)m_Object->FindComponent("SlashPaperBurn1");
	m_SlashLine->Enable(false);

	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();

	AnimInst->AddNotify("AttackReady", "OnAttackReady", 0, (CMonsterDataComponent*)this, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->SetEndFunction("AttackReady", this, &CDodgerDataComponent::OnAttack1ReadyEnd);
	AnimInst->AddNotify("AttackReady2", "OnAttackReady2", 0, (CMonsterDataComponent*)this, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->SetEndFunction("AttackReady2", this, &CDodgerDataComponent::OnAttack2ReadyEnd);

	AnimInst->AddNotify("Attack", "OnAttackMove", 0, this, &CDodgerDataComponent::OnAttackMoveStart);
	AnimInst->AddNotify("Attack", "OnAttackHit", 2, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack", "OnAttackHit", 3, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack", "OnAttackHit", 4, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack", "OnAttackHit", 5, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack", "OnAttackMove1End", 5, this, &CDodgerDataComponent::OnAttackMove1End);
	AnimInst->AddNotify("Attack2", "OnAttackMove", 0, this, &CDodgerDataComponent::OnAttackMoveStart);
	AnimInst->AddNotify("Attack2", "OnAttackHit", 2, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack2", "OnAttackHit", 3, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack2", "OnAttackHit", 4, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack2", "OnAttackHit", 5, this, &CDodgerDataComponent::OnAttackHit);
	AnimInst->AddNotify("Attack2", "OnAttackMove2End", 5, this, &CDodgerDataComponent::OnAttackMove2End);

	AnimInst->AddNotify("AttackEnd", "OnAttackEnd", 0, this, &CDodgerDataComponent::OnPostAttackStart);
	AnimInst->SetEndFunction("AttackEnd", this, &CDodgerDataComponent::OnPostAttackEnd);

	AnimInst->AddNotify("Dash", "OnDashReady", 0, this, &CDodgerDataComponent::OnDashReady);
	AnimInst->AddNotify("Dash", "OnDashStart", 8, this, &CDodgerDataComponent::OnDashStart);
	AnimInst->AddNotify("Dash", "OnDashEnd", 20, this, &CDodgerDataComponent::OnDashEnd);
}

void CDodgerDataComponent::Update(float DeltaTime)
{
	CMonsterDataComponent::Update(DeltaTime);

	if (m_DashRotaiting)
	{
		m_Object->AddWorldRotationY(m_CurRotSpeed * DeltaTime);
	}

	if (m_Dash)
	{
		bool In = m_MonsterNavAgent->IsInNavMesh();

		if (!In)
		{
			m_CurMoveSpeed = 0.f;
			Vector3 PrevPos = m_Object->GetPrevFramePos();
			m_Object->SetWorldPos(PrevPos);
			m_Dash = false;
		}
	}
}

void CDodgerDataComponent::SetIsHit(bool Hit)
{
	CMonsterDataComponent::SetIsHit(Hit);

	if (Hit)
	{
		CResourceManager::GetInst()->SoundPlay("DodgerHit");
	}
}

void CDodgerDataComponent::DecreaseHP(int Amount)
{
	CMonsterDataComponent::DecreaseHP(Amount);

	if (m_Data.HP <= 0)
	{
		CResourceManager::GetInst()->SoundPlay("DodgerDeath");
	}
}

void CDodgerDataComponent::OnAttack1ReadyEnd()
{
	OnDisableLookPlayer();

	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();
	AnimInst->ChangeAnimation("Attack");

	CResourceManager::GetInst()->SoundPlay("DodgerAttackReady1");
}

void CDodgerDataComponent::OnAttack2ReadyEnd()
{
	OnDisableLookPlayer();

	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();
	AnimInst->ChangeAnimation("Attack2");

	CResourceManager::GetInst()->SoundPlay("DodgerAttackReady2");
}

void CDodgerDataComponent::OnAttackMoveStart()
{
	int Rand = rand() % 2;

	std::string SoundKey = "DodgerAttack" + std::to_string(Rand);

	CResourceManager::GetInst()->SoundPlay(SoundKey);

	m_SlashLine->Enable(true);
	m_SlashLinePaperBurn->SetEndEvent(PaperBurnEndEvent::Disable);
	m_SlashLinePaperBurn->StartPaperBurn();

	m_CurMoveSpeed = m_Data.MoveSpeed * 2.f;

	OnEnableMoveZ();
}

void CDodgerDataComponent::OnAttackMove1End()
{
	m_CurMoveSpeed = 0.f;

	++m_AttackCount;

	OnDisableMoveZ();

	SetCurrentNodeNull();

	bool PlayerInRange = IsPlayerInMeleeAttackRange();

	if (PlayerInRange)
	{
		m_PostAttackDelaying = false;
	}
	else
	{
		m_PostAttackDelaying = true;
	}
}

void CDodgerDataComponent::OnAttackMove2End()
{
	m_CurMoveSpeed = 0.f;

	m_AttackCount = 0;

	OnDisableMoveZ();

	m_DashReady = true;

	SetCurrentNodeNull();

	// 공격 시점의 플레이어 위치를 기억해서, 대쉬 방향을 판별
	CGameObject* Player = m_Scene->GetPlayerObject();

	if (Player)
	{
		m_PrevPlayerPos = Player->GetWorldPos();
	}
}

void CDodgerDataComponent::OnAttackHit()
{
	if (m_PlayerData->IsUnbeatable())
	{
		return;
	}

	Vector3 MyLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;

	Vector3 MyPos = m_Object->GetWorldPos();
	Vector3 PlayerPos = m_PlayerData->GetGameObject()->GetWorldPos();

	Vector3 ToPlayer = PlayerPos - MyPos;
	ToPlayer.y = 0.f;
	ToPlayer.Normalize();

	float Angle = MyLookDir.Angle(ToPlayer);

	if (Angle <= 45.f && Angle >= -45.f)
	{
		m_PlayerData->DecreaseHP(1);
	}
}

void CDodgerDataComponent::OnDashReady()
{
	// 공격 시작 시점의 플레이어 위치와 현재 플레이어 위치를 기준으로
	// 플레이어가 이동한 동선과 똑같은 동선으로 대쉬함
	CGameObject* Player = m_Scene->GetPlayerObject();

	if (Player)
	{
		Vector3 PlayerPos = Player->GetWorldPos();
		Vector3 DashDir = (PlayerPos - m_PrevPlayerPos);
		DashDir.y = 0.f;

		Vector3 MyPos = m_Object->GetWorldPos();

		if (DashDir.Length() == 0.f)
		{
			DashDir = m_PrevPlayerPos - MyPos;
			DashDir.y = 0.f;
		}

		DashDir.Normalize();

		m_DashDest = MyPos + (DashDir * 7.f);

		// 내가 바라보는 방향과 현재 대쉬해야 할 방향의 각도를 통해, 애니메이션 프레임동안 회전 속도를 결정함
		Vector3 MyLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
		float Angle = m_DashDest.Angle(MyLookDir);

		CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();
		float FrameTime = AnimInst->GetAnimationFrameTime("Dash");

		m_CurRotSpeed = Angle / (FrameTime * 8);

		m_DashRotaiting = true;
	}
}

void CDodgerDataComponent::OnDashStart()
{
	CResourceManager::GetInst()->SoundPlay("DodgerJump");

	m_HitBox->Enable(false);

	// 대시 애니메이션 프레임 시간을 통해 이동 스피드를 결정함
	Vector3 MyPos = m_Object->GetWorldPos();
	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();

	float FrameTime = AnimInst->GetAnimationFrameTime("Dash");
	float DashTime = 12 * FrameTime;
	float ToDashPointLength = (m_DashDest - MyPos).Length();

	m_CurMoveSpeed = ToDashPointLength / DashTime;

	m_MoveZ = true;
	m_Dash = true;

	m_DashRotaiting = false;
	m_CurRotSpeed = 0.f;
}

void CDodgerDataComponent::OnDashEnd()
{
	CResourceManager::GetInst()->SoundPlay("DodgerLand");

	m_HitBox->Enable(true);

	m_CurMoveSpeed = 0.f;
	m_MoveZ = false;

	m_CurRotSpeed = 0.f;

	m_DashReady = false;
	m_Dash = false;

	SetCurrentNodeNull();
}

void CDodgerDataComponent::OnPostAttackStart()
{
	m_PostAttackDelaying = true;
}

void CDodgerDataComponent::OnPostAttackEnd()
{
	m_AttackCount = 0;
	m_PostAttackDelaying = false;

	SetCurrentNodeNull();
}

