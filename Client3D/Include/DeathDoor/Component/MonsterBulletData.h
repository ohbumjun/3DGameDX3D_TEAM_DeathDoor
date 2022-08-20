#pragma once

#include "ObjectDataComponent.h"

class CMonsterBulletData :
    public CObjectDataComponent
{
	friend class CGameObject;

protected:
	CMonsterBulletData();
	CMonsterBulletData(const CMonsterBulletData& com);
	virtual ~CMonsterBulletData();

public:
	virtual void Start() override;
	void OnCollide(const CollisionResult& Result);
	
public:
	void ShootBulletLifeTime(const Vector3& StartPos, const Vector3& Dir, float Speed, float LifeTime, class CGameObject* EndParticle);

private:
	class CColliderSphere* m_Collider;
	class CProjectileComponent* m_Projectile;
	class CPlayerDataComponent* m_PlayerData;
};

