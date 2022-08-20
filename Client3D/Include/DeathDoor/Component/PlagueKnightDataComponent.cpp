#include "PlagueKnightDataComponent.h"
#include "../DataManager.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "PlayerDataComponent.h"
#include "ProjectileComponent.h"
#include "Component/ColliderSphere.h"
#include "Component/AnimationMeshComponent.h"
#include "MonsterNavAgent.h"
#include "ObjectPool.h"

CPlagueKnightDataComponent::CPlagueKnightDataComponent()
{
	SetTypeID<CPlagueKnightDataComponent>();
}

CPlagueKnightDataComponent::CPlagueKnightDataComponent(const CPlagueKnightDataComponent& com)	:
	CMonsterDataComponent(com)
{
}

CPlagueKnightDataComponent::~CPlagueKnightDataComponent()
{
}

void CPlagueKnightDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_Data = CDataManager::GetInst()->GetObjectData("PlagueKnight");

	m_FirePos = (CSceneComponent*)m_Object->FindComponent("FirePos");
	m_MeleeParticlePos = (CSceneComponent*)m_Object->FindComponent("MeleeParticlePos");
	m_ExplodeCollider = m_Object->FindComponentFromType<CColliderSphere>();

	if (m_ExplodeCollider)
	{
		m_ExplodeCollider->AddCollisionCallback(Collision_State::Begin, this, &CPlagueKnightDataComponent::OnCollideExplode);
		m_ExplodeCollider->Enable(false);
	}

	m_StopChaseRange = m_Data.JumpAttackRange;
	m_CurRotSpeed = m_Data.RotateSpeedPerSec;

	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();

	AnimInst->AddNotify("StartShoot", "OnReadyShoot", 0, this, &CPlagueKnightDataComponent::OnShootReadyAnimStart);
	AnimInst->SetEndFunction("StartShoot", this, &CPlagueKnightDataComponent::OnShootReadyAnimEnd);
	AnimInst->SetEndFunction("ChargeShoot", this, &CPlagueKnightDataComponent::OnChargeAnimEnd);
	AnimInst->AddNotify("Shoot", "OnShoot", 0, this, &CPlagueKnightDataComponent::OnShoot);
	AnimInst->SetEndFunction("Shoot", this, &CPlagueKnightDataComponent::OnShootAnimEnd);
	AnimInst->AddNotify("MeleeAttack", "OnStartMeleeAttack", 0, (CMonsterDataComponent*)this, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->AddNotify("MeleeAttack", "OnStartMeleeAttack", 25, (CMonsterDataComponent*)this, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify("MeleeAttack", "OnEnableMeleeCollider", 30, (CMonsterDataComponent*)this, &CMonsterDataComponent::OnActiveMeleeAttackCollider);
	AnimInst->AddNotify("MeleeAttack", "OnDisableMeleeCollider", 32, (CMonsterDataComponent*)this, &CMonsterDataComponent::OnInActiveMeleeAttackCollider);
	AnimInst->AddNotify("MeleeAttack", "OnAttackEnd", 69, (CMonsterDataComponent*)this, &CMonsterDataComponent::SetCurrentNodeNull);
}

void CPlagueKnightDataComponent::Update(float DeltaTime)
{
	CMonsterDataComponent::Update(DeltaTime);

	if (m_ColliderOn)
	{
		m_ExplodeCollider->Enable(false);
	}

	if (m_ExplodeCollider)
	{
		if (m_ExplodeCollider->IsEnable())
		{
			m_ColliderOn = true;
		}
	}
}

void CPlagueKnightDataComponent::DecreaseHP(int Amount)
{
	CMonsterDataComponent::DecreaseHP(Amount);

	CResourceManager::GetInst()->SoundPlay("PlagueKnightDeath");
}

void CPlagueKnightDataComponent::OnShootReadyAnimStart()
{
	CResourceManager::GetInst()->SoundPlay("PlagueKnightCharge");

	m_LookPlayer = true;
}

void CPlagueKnightDataComponent::OnShootReadyAnimEnd()
{
	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();
	AnimInst->ChangeAnimation("ChargeShoot");
}

void CPlagueKnightDataComponent::OnChargeAnimEnd()
{
	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();
	AnimInst->ChangeAnimation("Shoot");

	m_LookPlayer = false;
}

void CPlagueKnightDataComponent::OnShoot()
{
	CResourceManager::GetInst()->SoundPlay("PlagueKnightFire");

	CGameObject* Bullet = CObjectPool::GetInst()->GetProjectile("PlagueKnightBullet", m_Scene);

	CProjectileComponent* Proj = Bullet->FindObjectComponentFromType<CProjectileComponent>();
	
	Vector3 PlayerPos = m_PlayerData->GetGameObject()->GetWorldPos();
	Vector3 FirePos = m_FirePos->GetWorldPos();

	Vector3 XZ = PlayerPos - FirePos;
	XZ.y = 0.f;
	XZ.Normalize();

	CGameObject* EndParticle = CObjectPool::GetInst()->GetParticle("PlagueKnightParticle", m_Scene);
	Proj->SetEndCallBack(this, &CPlagueKnightDataComponent::OnBulletGround);
	Proj->ShootByGravityTargetPos(FirePos, XZ, 60.f, PlayerPos, 30.f, EndParticle);
	Proj->SetDestroy(true);

	CCameraComponent* CurCam = m_Object->GetScene()->GetCameraManager()->GetCurrentCamera();
	CurCam->Shake(0.1f, 0.3f);
}

void CPlagueKnightDataComponent::OnShootAnimEnd()
{
	SetCurrentNodeNull();
}

void CPlagueKnightDataComponent::OnBulletGround(const Vector3& Pos)
{
	CResourceManager::GetInst()->SoundPlay("PlagueKnightExplosion");

	m_ExplodeCollider->Enable(true);
	m_ExplodeCollider->SetWorldPos(Pos);

	CCameraComponent* CurCam = m_Object->GetScene()->GetCameraManager()->GetCurrentCamera();
	CurCam->Shake(0.3f, 0.8f);
}

void CPlagueKnightDataComponent::OnCollideExplode(const CollisionResult& Result)
{
	if (Result.Dest->GetGameObject() == m_PlayerData->GetGameObject())
	{
		if (!m_PlayerData->IsUnbeatable())
		{
			m_PlayerData->DecreaseHP(1);
		}
	}
}

void CPlagueKnightDataComponent::OnActiveMeleeAttackCollider()
{
	CResourceManager::GetInst()->SoundPlay("PlagueKnightSlam");

	CMonsterDataComponent::OnActiveMeleeAttackCollider();

	CGameObject* Particle = CObjectPool::GetInst()->GetParticle("BossKnightImpact", m_Scene);

	Vector3 ParticlePos = m_MeleeParticlePos->GetWorldPos();
	Particle->StartParticle(ParticlePos);

	CCameraComponent* CurCam = m_Object->GetScene()->GetCameraManager()->GetCurrentCamera();
	CurCam->Shake(0.3f, 0.8f);
}
