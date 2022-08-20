#pragma once

#include "MonsterDataComponent.h"

class CFirePlantDataComponent :
    public CMonsterDataComponent
{
    friend class CGameObject;

private:
    CFirePlantDataComponent();
    CFirePlantDataComponent(const CFirePlantDataComponent& com);
    virtual ~CFirePlantDataComponent();

public:
	virtual void Start() override;
    virtual void SetIsHit(bool Hit) override;
    virtual void OnDeadAnimStart() override;

public:
    void OnAttackCoolEnable();
    void OnAttackCoolDisable();
    void OnShootBullet();

private:
    class CSceneComponent* m_FirePos;
};

