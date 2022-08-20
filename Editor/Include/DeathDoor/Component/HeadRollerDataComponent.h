#pragma once

#include "MonsterDataComponent.h"

class CHeadRollerDataComponent :
    public CMonsterDataComponent
{
    friend class CGameObject;

private:
    CHeadRollerDataComponent();
    CHeadRollerDataComponent(const CHeadRollerDataComponent& com);
    virtual ~CHeadRollerDataComponent();

public:
	virtual void Start() override;

public:
    virtual void SetIsHit(bool Enable) override;
    virtual void DecreaseHP(int Amount) override;

public:
    bool IsStun() const
    {
        return m_Stun;
    }

public:
    void OnRollReadyStart();
    void OnPlayStunRecoverSound();
    void OnRollStart();
    void OnRollEnd();
    void OnStunStart();
    void OnStunEnd();
    void OnCollide(const CollisionResult& Result);

private:
    class CParticleComponent* m_RollParticle;
    bool m_Rolling;
    bool m_Stun;
};

