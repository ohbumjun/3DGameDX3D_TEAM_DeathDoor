#include "MonsterBulletData.h"
#include "Component/ColliderSphere.h"
#include "ProjectileComponent.h"
#include "PlayerDataComponent.h"
#include "Scene/Scene.h"

CMonsterBulletData::CMonsterBulletData()
{
	SetTypeID<CMonsterBulletData>();
}

CMonsterBulletData::CMonsterBulletData(const CMonsterBulletData& com)	:
	CObjectDataComponent(com)
{
}

CMonsterBulletData::~CMonsterBulletData()
{
}

void CMonsterBulletData::Start()
{
	CObjectComponent::Start();

	m_Collider = m_Object->FindComponentFromType<CColliderSphere>();

	m_Projectile = m_Object->FindComponentFromType<CProjectileComponent>();

	CGameObject* Player = m_Scene->GetPlayerObject();

	if (Player)
	{
		m_PlayerData = Player->FindObjectComponentFromType<CPlayerDataComponent>();
	}
}

void CMonsterBulletData::OnCollide(const CollisionResult& Result)
{
	CGameObject* Player = m_Scene->GetPlayerObject();

	if (m_PlayerData->IsUnbeatable() == false && Player == Result.Dest->GetGameObject())
	{
		m_Projectile->OnEnd();
		m_PlayerData->DecreaseHP(1);
		m_Collider->Enable(false);
	}
}

void CMonsterBulletData::ShootBulletLifeTime(const Vector3& StartPos, const Vector3& Dir, float Speed, float LifeTime, CGameObject* EndParticle)
{
	m_Projectile->ShootByLifeTimeCollision(this, &CMonsterBulletData::OnCollide, Collision_State::Begin,
		StartPos, Dir, Speed, LifeTime, EndParticle);
	m_Projectile->SetDestroy(true);
}
