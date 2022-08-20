
#include "BatDataComponent.h"
#include "../DataManager.h"
#include "Component/ColliderBox3D.h"
#include "Component/AnimationMeshComponent.h"
#include "MonsterNavAgent.h"
#include "GameStateComponent.h"
#include "Scene/Scene.h"

CBatDataComponent::CBatDataComponent() :
	m_RecognizeStart(false),
	m_RecognizeEnd(false),
	m_DeathAnimStart(false)
{
	SetTypeID<CBatDataComponent>();

	m_ComponentType = Component_Type::ObjectComponent;
}

CBatDataComponent::CBatDataComponent(const CBatDataComponent& com)	:
	CMonsterDataComponent(com)
{
}

CBatDataComponent::~CBatDataComponent()
{
}

void CBatDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_Data = CDataManager::GetInst()->GetObjectData("Bat");

	m_HitBox = m_Object->FindComponentFromType<CColliderBox3D>();

	m_AnimMesh->GetAnimationInstance()->SetEndFunction("Recognize", this, &CBatDataComponent::OnRecognizeEnd);
	m_AnimMesh->GetAnimationInstance()->SetEndFunction("MeleeAttack", this, &CBatDataComponent::OnEndMeleeAttack);

	m_AnimMesh->GetAnimationInstance()->AddNotify<CBatDataComponent>("MeleeAttack", "MeleeAttackStart", 19, this, &CBatDataComponent::OnMeleeAttackColliderEnable);
	m_AnimMesh->GetAnimationInstance()->AddNotify<CBatDataComponent>("MeleeAttack", "MeleeAttackEnd", 32, this, &CBatDataComponent::OnMeleeAttackColliderDisable);

	m_AnimMesh->GetAnimationInstance()->AddNotify<CBatDataComponent>("MeleeAttack", "MeleeAttackSoundPlay", 18, this, &CBatDataComponent::OnAttackSoundPlay);
	m_AnimMesh->GetAnimationInstance()->AddNotify<CBatDataComponent>("Death", "DeathSoundPlay", 32, this, &CBatDataComponent::OnDeathSoundPlay);
	m_AnimMesh->GetAnimationInstance()->AddNotify<CBatDataComponent>("Run", "FlapSoundPlay", 10, this, &CBatDataComponent::OnFlapSoundPlay);
	m_AnimMesh->GetAnimationInstance()->AddNotify<CBatDataComponent>("Run", "FlapSoundPlay", 30, this, &CBatDataComponent::OnFlapSoundPlay);


	if (m_MonsterNavAgent)
		m_MonsterNavAgent->SetMoveSpeed(m_Data.MoveSpeed);

	CMonsterDataComponent* Data = (CMonsterDataComponent*)m_Object->FindComponent("ObjectData");
	Data->SetAttackCoolDelayTimeMax(1.7f);
}

void CBatDataComponent::Update(float DeltaTime)
{
	CMonsterDataComponent::Update(DeltaTime);

	CAnimationSequenceInstance* Instance = m_AnimMesh->GetAnimationInstance();

	bool IsOnMeleeAttack = Instance->CheckCurrentAnimation("MeleeAttack");

	if (!IsOnMeleeAttack)
	{
		m_MeleeAttackCollider->Enable(false);
	}

	if (m_Data.HP <= 0.f && !m_DeathAnimStart)
	{
		m_MonsterNavAgent->ClearPathList();
		m_MonsterNavAgent->SetPathFindEnable(false);
		m_Object->FindObjectComponentFromType<CGameStateComponent>()->SetTreeUpdate(false);
		m_DeathAnimStart = true;
	}
}

void CBatDataComponent::OnHitMeleeAttack(const CollisionResult& Result)
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

void CBatDataComponent::SetIsHit(bool Hit)
{
	CMonsterDataComponent::SetIsHit(Hit);

	if (m_Data.HP <= 0)
	{
		m_HitBox->Enable(false);
		m_MeleeAttackCollider->Enable(false);
		m_AnimMesh->GetAnimationInstance()->GetCurrentAnimation()->SetPlayScale(0.f);
		OnDeadAnimEnd();
	}
}

void CBatDataComponent::OnRecognizeEnd()
{
 	m_RecognizeEnd = true;
}

void CBatDataComponent::OnEndMeleeAttack()
{
	CMonsterDataComponent* Data = (CMonsterDataComponent*)m_Object->FindComponent("ObjectData");

	Data->SetAttackCoolDelayTimeEnable(true);

	CGameStateComponent* State = m_Object->FindObjectComponentFromType<CGameStateComponent>();
	State->GetBehaviorTree()->SetCurrentNode(nullptr);

	//m_AnimMesh->GetAnimationInstance()->SetCurrentAnimation("Idle");
}

void CBatDataComponent::OnMeleeAttackColliderEnable()
{
	m_MeleeAttackCollider->Enable(true);
}

void CBatDataComponent::OnMeleeAttackColliderDisable()
{
	m_MeleeAttackCollider->Enable(false);
}

void CBatDataComponent::OnCollision(const CollisionResult& Result)
{
	if (Result.Dest->GetGameObject()->GetObjectType() == Object_Type::Player)
	{
		m_Object->Destroy();

		CGameObject* DestObject = Result.Dest->GetGameObject();

		CObjectDataComponent* Data = (CObjectDataComponent*)DestObject->FindComponent("ObjectData");
		float Attack = m_Data.Attack;

		Data->DecreaseHP(Attack);
	}
}

void CBatDataComponent::OnAttackSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("BatAttack");
}

void CBatDataComponent::OnDeathSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("BatDeath");
}

void CBatDataComponent::OnFlapSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("BatFlap");
}
