
#include "LurkerDataComponent.h"
#include "../DataManager.h"
#include "MonsterNavAgent.h"
#include "Component/ColliderBox3D.h"
#include "GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Scene/Scene.h"

CLurkerDataComponent::CLurkerDataComponent()	:
	m_HopStart(false),
	m_DeathAnimStart(false)
{
	SetTypeID<CLurkerDataComponent>();

	m_ComponentType = Component_Type::ObjectComponent;
}

CLurkerDataComponent::CLurkerDataComponent(const CLurkerDataComponent& com)	:
	CMonsterDataComponent(com)
{
}

CLurkerDataComponent::~CLurkerDataComponent()
{
}

void CLurkerDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_Data = CDataManager::GetInst()->GetObjectData("Lurker");

	m_MonsterNavAgent->SetMoveSpeed(m_Data.MoveSpeed);

	m_HitBox = m_Object->FindComponentFromType<CColliderBox3D>();

	m_MeleeAttackCollider->Enable(false);

	if (m_MonsterNavAgent)
		m_MonsterNavAgent->SetMoveSpeed(m_Data.MoveSpeed);

	CMonsterDataComponent* Data = (CMonsterDataComponent*)m_Object->FindComponent("ObjectData");
	Data->SetAttackCoolDelayTimeMax(1.4f);


	CAnimationSequenceInstance* Instance = m_AnimMesh->GetAnimationInstance();

	Instance->AddNotify<CLurkerDataComponent>("MeleeAttack", "MeleeAttackColliderEnable", 6, this, &CLurkerDataComponent::OnMeleeAttackColliderEnable);
	Instance->AddNotify<CLurkerDataComponent>("MeleeAttack", "MeleeAttackColliderDisable", 16, this, &CLurkerDataComponent::OnMeleeAttackColliderDisable);

	Instance->AddNotify<CLurkerDataComponent>("MeleeAttack", "MeleeAttackColliderEnable", 9, this, &CLurkerDataComponent::OnMeleeAttackRush);
	Instance->AddNotify<CLurkerDataComponent>("MeleeAttack", "MeleeAttackColliderEnable", 15, this, &CLurkerDataComponent::OnRestorePrevAttackPos);
	Instance->AddNotify<CLurkerDataComponent>("MeleeAttackprep", "MeleeAttackprepRotate", 0, this, &CLurkerDataComponent::LookPlayer);

	Instance->AddNotify<CLurkerDataComponent>("MeleeAttackprep", "MeleeAttackprepSoundPlay", 0, this, &CLurkerDataComponent::OnPrepAttackSoundPlay);
	Instance->AddNotify<CLurkerDataComponent>("MeleeAttack", "MeleeAttackSoundPlay", 0, this, &CLurkerDataComponent::OnAttackSoundPlay);
	Instance->AddNotify<CLurkerDataComponent>("Death", "DeathSoundPlay", 0, this, &CLurkerDataComponent::OnDeathSoundPlay);
	Instance->AddNotify<CLurkerDataComponent>("Run", "RunSoundPlay", 2, this, &CLurkerDataComponent::OnStepSoundPlay);
	Instance->AddNotify<CLurkerDataComponent>("Run", "RunSoundPlay", 12, this, &CLurkerDataComponent::OnStepSoundPlay);

	Instance->SetEndFunction<CLurkerDataComponent>("MeleeAttackprep", this, &CLurkerDataComponent::OnEndMeleeAttackPrep);
	Instance->SetEndFunction<CLurkerDataComponent>("MeleeAttack", this, &CLurkerDataComponent::OnEndMeleeAttack);
	Instance->SetEndFunction<CLurkerDataComponent>("Hop", this, &CLurkerDataComponent::OnHopEnd);


}

void CLurkerDataComponent::Update(float DeltaTime)
{
	CMonsterDataComponent::Update(DeltaTime);

	if (m_DeathAnimStart)
		return;

	if (m_Data.HP <= 0.f && !m_DeathAnimStart)
	{
		m_AnimMesh->GetAnimationInstance()->ChangeAnimation("Death");
		m_MonsterNavAgent->ClearPathList();
		m_MonsterNavAgent->SetPathFindEnable(false);
		m_Object->FindObjectComponentFromType<CGameStateComponent>()->SetTreeUpdate(false);
		m_DeathAnimStart = true;

		return;
	}

	CAnimationSequenceInstance* Instance = m_AnimMesh->GetAnimationInstance();

	bool IsOnMeleeAttack = Instance->CheckCurrentAnimation("MeleeAttack");

	if (!IsOnMeleeAttack)
	{
		m_MeleeAttackCollider->Enable(false);
	}


	bool IsRunning = Instance->CheckCurrentAnimation("Run");

	if (IsRunning)
	{
		int Index = Instance->GetCurrentAnimation()->GetAnimationSequence()->GetCurrentFrameIdx();
		float Speed = m_Data.MoveSpeed;

		if ((Index > 2 && Index < 8) || (Index > 11 && Index < 19))
			m_MonsterNavAgent->SetMoveSpeed(Speed * 1.5f);

		else
			m_MonsterNavAgent->SetMoveSpeed(Speed / 3.f);
	}
}

void CLurkerDataComponent::OnHitMeleeAttack(const CollisionResult& Result)
{
	Vector3 Vec1 = (Result.Dest->GetGameObject()->GetWorldPos() - m_Object->GetWorldPos());
	Vec1.Normalize();
	Vec1.y = 0.f;
	Vector3 Vec2 = m_Object->GetWorldAxis(AXIS_Z) * -1.f;
	Vec2.Normalize();
	Vec2.y = 0.f;

	// 공격 방향에 존재하는지 판정
	if (Vec1.Dot(Vec2) > 0.f)
		CMonsterDataComponent::OnHitMeleeAttack(Result);
}

void CLurkerDataComponent::SetIsHit(bool Hit)
{
	CMonsterDataComponent::SetIsHit(Hit);

	m_Object->GetScene()->GetResource()->SoundPlay("LurkerTakeDamage");
}

void CLurkerDataComponent::OnHopEnd()
{
	m_HopStart = true;
}

void CLurkerDataComponent::OnEndMeleeAttackPrep()
{
	CAnimationSequenceInstance* Instance = m_AnimMesh->GetAnimationInstance();
	
	if(!m_DeathAnimStart)
		Instance->ChangeAnimation("MeleeAttack");
}

void CLurkerDataComponent::OnEndMeleeAttack()
{
	CMonsterDataComponent* Data = (CMonsterDataComponent*)m_Object->FindComponent("ObjectData");

	Data->SetAttackCoolDelayTimeEnable(true);

	CGameStateComponent* State = m_Object->FindObjectComponentFromType<CGameStateComponent>();
	State->GetBehaviorTree()->SetCurrentNode(nullptr);
}

void CLurkerDataComponent::OnMeleeAttackColliderEnable()
{
	m_MeleeAttackCollider->Enable(true);
}

void CLurkerDataComponent::OnMeleeAttackColliderDisable()
{
	m_MeleeAttackCollider->Enable(false);
}

void CLurkerDataComponent::OnMeleeAttackRush()
{
	m_PrevAttackPos = m_Object->GetWorldPos();

	Vector3 ZDir = m_Object->GetWorldAxis(AXIS_Z);
	ZDir *= -1.f;

	m_Object->SetWorldPos(m_PrevAttackPos + ZDir / 5.f);
}

void CLurkerDataComponent::OnRestorePrevAttackPos()
{
	//m_Object->SetWorldPos(m_PrevAttackPos);

	Vector3 ZDir = m_Object->GetWorldAxis(AXIS_Z);
	ZDir *= -1.f;

	m_Object->SetWorldPos(m_PrevAttackPos - ZDir / 5.f);
}

void CLurkerDataComponent::OnAttackSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("LurkerAttack");
}

void CLurkerDataComponent::OnDeathSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("LurkerDeath");
}

void CLurkerDataComponent::OnPrepAttackSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("LurkerPrepAttack");
}

void CLurkerDataComponent::OnStepSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("LurkerStep");
}

void CLurkerDataComponent::LookPlayer()
{
	Vector3 ZAxis = m_Object->GetWorldAxis(AXIS_Z);
	ZAxis *= -1.f;

	Vector3 MyPos = m_Object->GetWorldPos();
	Vector3 PlayerPos = m_Scene->GetPlayerObject()->GetWorldPos();

	Vector3 LookDir = PlayerPos - MyPos;
	LookDir.y = 0.f;
	LookDir.Normalize();

	float Angle = 0.f;
	float DotProduct = LookDir.Dot(ZAxis);
	Vector3 CrossVec = LookDir.Cross(ZAxis);

	if (DotProduct >= -0.99999999999f && DotProduct <= 0.99999999999f)
	{
		Angle = RadianToDegree(acosf(DotProduct));

		if (CrossVec.y > 0)
			Angle *= -1.f;
	}

	else
	{
		if (DotProduct == -1.f)
			Angle = 180.f;
	}

	m_Object->AddWorldRotationY(Angle);
}
