#pragma once

#include "MonsterDataComponent.h"

class CSporeBoomerDataComponent :
    public CMonsterDataComponent
{
    friend class CGameObject;

private:
    CSporeBoomerDataComponent();
    CSporeBoomerDataComponent(const CSporeBoomerDataComponent& com);
    virtual ~CSporeBoomerDataComponent();

public:
	virtual void Start() override;
    virtual void Update(float DeltaTime) override;

public:
    void OnBombGround(const Vector3& BombGroundPos);
    void OnCollideBomb(const CollisionResult& Result);
    virtual void SetIsHit(bool Hit) override;

public:
    void OnShootBullet();

private:
    class CSceneComponent* m_FirePos;
    class CColliderSphere* m_BombCollider;
    bool m_BombColliderOn;
    bool m_ColliderOnPrev;
};

