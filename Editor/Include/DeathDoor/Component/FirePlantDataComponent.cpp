#include "FirePlantDataComponent.h"
#include "../DataManager.h"
#include "Component/AnimationMeshComponent.h"
#include "PlayerDataComponent.h"
#include "ObjectPool.h"
#include "ProjectileComponent.h"
#include "Scene/Scene.h"
#include "MonsterBulletData.h"

CFirePlantDataComponent::CFirePlantDataComponent()
{
	SetTypeID<CFirePlantDataComponent>();

	m_AttackCoolDelayTimeMax = 1.f;
}

CFirePlantDataComponent::CFirePlantDataComponent(const CFirePlantDataComponent& com)	:
	CMonsterDataComponent(com)
{
}

CFirePlantDataComponent::~CFirePlantDataComponent()
{
}

void CFirePlantDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_Data = CDataManager::GetInst()->GetObjectData("FirePlant");
	m_FirePos = dynamic_cast<CSceneComponent*>(m_Object->FindComponent("FirePos"));

	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();

	AnimInst->AddNotify("Fire", "OnStartFire", 0, this, &CFirePlantDataComponent::OnAttackCoolDisable);
	AnimInst->AddNotify("Fire", "OnFire", 25, this, &CFirePlantDataComponent::OnShootBullet);
	AnimInst->SetEndFunction("Fire", this, &CFirePlantDataComponent::OnAttackCoolEnable);
}

void CFirePlantDataComponent::SetIsHit(bool Hit)
{
	CMonsterDataComponent::SetIsHit(Hit);

	CResourceManager::GetInst()->SoundPlay("FirePlantHit");
}

void CFirePlantDataComponent::OnDeadAnimStart()
{
	CMonsterDataComponent::OnDeadAnimStart();

	CResourceManager::GetInst()->SoundPlay("FirePlantDeath");
}

void CFirePlantDataComponent::OnAttackCoolEnable()
{
	SetAttackCoolDelayTimeEnable(true);

	SetCurrentNodeNull();

	m_LookPlayer = false;
}

void CFirePlantDataComponent::OnAttackCoolDisable()
{
	SetAttackCoolDelayTimeEnable(false);

	m_LookPlayer = true;

	CResourceManager::GetInst()->SoundPlay("FirePlantCharge");
}

void CFirePlantDataComponent::OnShootBullet()
{
	CGameObject* Projectile = CObjectPool::GetInst()->GetProjectile("FirePlantBullet", m_Scene);

	CMonsterBulletData* Data = Projectile->FindObjectComponentFromType<CMonsterBulletData>();

	if (!m_PlayerData)
	{
		return;
	}

	CGameObject* Player = m_PlayerData->GetGameObject();

	Vector3 PlayerPos = m_PlayerData->GetGameObject()->GetWorldPos();
	Vector3 MyPos = m_FirePos->GetWorldPos();

	Vector3 ToPlayer = PlayerPos - MyPos;
	ToPlayer.y = 0.f;
	ToPlayer.Normalize();

	if (Data)
	{
		CGameObject* EndParticle = CObjectPool::GetInst()->GetParticle("FirePlantBulletDust", m_Scene);
		Data->ShootBulletLifeTime(MyPos, ToPlayer, 10.f, 5.f, EndParticle);
	}

	CResourceManager::GetInst()->SoundPlay("FirePlantFire");
}
