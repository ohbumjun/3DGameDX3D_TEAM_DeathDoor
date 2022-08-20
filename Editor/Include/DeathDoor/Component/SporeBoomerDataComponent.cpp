#include "SporeBoomerDataComponent.h"
#include "../DataManager.h"
#include "Component/AnimationMeshComponent.h"
#include "PlayerDataComponent.h"
#include "ObjectPool.h"
#include "ProjectileComponent.h"
#include "Scene/Scene.h"
#include "Component/ColliderSphere.h"
#include "Component/CameraComponent.h"
#include "Resource/Sound/SoundManager.h"

CSporeBoomerDataComponent::CSporeBoomerDataComponent()
{
	SetTypeID<CSporeBoomerDataComponent>();
}

CSporeBoomerDataComponent::CSporeBoomerDataComponent(const CSporeBoomerDataComponent& com)	:
	CMonsterDataComponent(com)
{
}

CSporeBoomerDataComponent::~CSporeBoomerDataComponent()
{
}

void CSporeBoomerDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_Data = CDataManager::GetInst()->GetObjectData("SporeBoomer");

	CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();
	AnimInst->AddNotify("Fire", "OnFire", 44, this, &CSporeBoomerDataComponent::OnShootBullet);
	AnimInst->SetEndFunction("Fire", (CMonsterDataComponent*)this, &CMonsterDataComponent::SetCurrentNodeNull);

	m_FirePos = dynamic_cast<CSceneComponent*>(m_Object->FindComponent("FirePos"));
	m_BombCollider = m_Object->FindComponentFromType<CColliderSphere>();
	m_BombCollider->AddCollisionCallback(Collision_State::Begin, this, &CSporeBoomerDataComponent::OnCollideBomb);
	m_BombCollider->Enable(false);
}

void CSporeBoomerDataComponent::Update(float DeltaTime)
{
	CMonsterDataComponent::Update(DeltaTime);

	if (m_BombColliderOn && !m_ColliderOnPrev)
	{
		m_ColliderOnPrev = true;
	}
	else if (m_ColliderOnPrev)
	{
		m_BombCollider->Enable(false);
		m_ColliderOnPrev = false;
		m_BombColliderOn = false;
	}
}

void CSporeBoomerDataComponent::OnBombGround(const Vector3& BombGroundPos)
{
	m_BombCollider->Enable(true);
	m_BombCollider->SetWorldPos(BombGroundPos);

	m_BombColliderOn = true;
	m_ColliderOnPrev = false;

	// Shake
	CCameraComponent* Cam = m_Scene->GetCameraManager()->GetCurrentCamera();
	Cam->Shake(0.3f, 1.5f);

	CResourceManager::GetInst()->SoundPlay("SporeBoomerBombGround");
}

void CSporeBoomerDataComponent::OnCollideBomb(const CollisionResult& Result)
{
	CGameObject* Obj = Result.Dest->GetGameObject();

	// 플레이어일 경우 Hit 처리
	if (Obj == m_Scene->GetPlayerObject())
	{
		if (m_PlayerData)
		{
			m_PlayerData->DecreaseHP(1);
		}
	}
}

void CSporeBoomerDataComponent::SetIsHit(bool Hit)
{
	CMonsterDataComponent::SetIsHit(Hit);

	CResourceManager::GetInst()->SoundPlay("SporeBoomerHit");
}

void CSporeBoomerDataComponent::OnShootBullet()
{
	CGameObject* Projectile = CObjectPool::GetInst()->GetProjectile("SporeBomb", m_Scene);

	CProjectileComponent* ProjComp = Projectile->FindObjectComponentFromType<CProjectileComponent>();

	Vector3 PlayerPos = m_Scene->GetPlayerObject()->GetWorldPos();
	Vector3 MyPos = m_FirePos->GetWorldPos();

	Vector3 XZ = PlayerPos - MyPos;
	XZ.y = 0.f;

	 CGameObject* EndParticle = CObjectPool::GetInst()->GetParticle("SporeBoomerParticle", m_Scene);

	ProjComp->ShootByGravityTargetPos(MyPos, XZ, 60.f, PlayerPos, 50.f, EndParticle);
	ProjComp->SetEndCallBack(this, &CSporeBoomerDataComponent::OnBombGround);
	ProjComp->SetDestroy(true);

	CResourceManager::GetInst()->SoundPlay("SporeBoomerFire");
}
