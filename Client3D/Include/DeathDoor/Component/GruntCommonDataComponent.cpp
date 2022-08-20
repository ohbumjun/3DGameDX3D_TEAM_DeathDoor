#include "GruntCommonDataComponent.h"
#include "GameBehaviorTree.h"
#include "GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Collision/Collision.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../DataManager.h"
#include "Component/CameraComponent.h"
#include "GameObject/GameObject.h"
#include "ObjectPool.h"
#include "Node/MonsterHitBack.h"
#include "Resource/ResourceManager.h"

CGruntCommonDataComponent::CGruntCommonDataComponent() :
	m_StartAct(false),
	m_SpawnEnable(true) // SpawnNode 에서 Animation 이 모두 진행된 이후, false 로 처리해줄 것이다.
{
	SetTypeID<CGruntCommonDataComponent>();
}

CGruntCommonDataComponent::CGruntCommonDataComponent(const CGruntCommonDataComponent& com) :
	CMonsterDataComponent(com)
{
}

CGruntCommonDataComponent::~CGruntCommonDataComponent()
{
}

void CGruntCommonDataComponent::Start()
{
	CMonsterDataComponent::Start();

	if (m_HitBox)
	{
		m_HitBox->SetExtent(1.0f, 2.0f, 1.0f);
	}

	m_Data = CDataManager::GetInst()->GetObjectData("GruntNormal");

	// MoveSpeed
	m_OriginMoveSpeed = m_Data.MoveSpeed;
	m_CurMoveSpeed = m_Data.MoveSpeed;

	m_MeleeAttackCollider = dynamic_cast<CColliderBox3D*>((m_Object->FindComponent("AttackCollider")));


	if (m_MeleeAttackCollider)
	{
		m_MeleeAttackCollider->SetExtent(0.5f, 0.5f, 0.5f);

		m_MeleeAttackCollider->Enable(false);

		m_MeleeAttackCollider->AddCollisionCallback(Collision_State::Begin,
			(CMonsterDataComponent*)this, &CMonsterDataComponent::OnHitMeleeAttack);
	}

	m_BloodParticle = nullptr;

	// Change Range
	m_StopChaseRange = m_Data.MeleeAttackRange;

	// Attack Cool Time Check
	m_AttackCoolDelayTimeMax = 1.0f;
}

void CGruntCommonDataComponent::ActivateHitSubEffect()
{
	// MonsterHitBack Node 를 Current Node 로 세팅하기
	// CNode* MonsterHitBackNode = m_State->GetBehaviorTree()->FindNodeByType<CMonsterHitBack>();
	// m_State->GetBehaviorTree()->SetCurrentNode(MonsterHitBackNode);
}

void CGruntCommonDataComponent::OnActiveMeleeAttackCollider()
{
	CMonsterDataComponent::OnActiveMeleeAttackCollider();

	const Vector3& ZDir = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f ;

	m_MeleeAttackCollider->SetRelativePos(ZDir * 2.f);
}

void CGruntCommonDataComponent::OnResetOriginalMoveSpeed()
{
	m_CurMoveSpeed = m_OriginMoveSpeed;
}

void CGruntCommonDataComponent::OnGruntAttackSound()
{
	// GruntAttack
	CResourceManager::GetInst()->SoundPlay("GruntAttack");

}

void CGruntCommonDataComponent::OnGruntSpawnSound()
{
	CResourceManager::GetInst()->SoundPlay("GruntSpawn");
}
