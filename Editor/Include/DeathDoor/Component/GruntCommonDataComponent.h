#pragma once
#include "MonsterDataComponent.h"

class CGruntCommonDataComponent :
    public CMonsterDataComponent
{
    friend class CGameObject;

private:
    CGruntCommonDataComponent();
    CGruntCommonDataComponent(const CGruntCommonDataComponent& com);
    virtual ~CGruntCommonDataComponent();
private :
    bool m_StartAct;
    bool m_SpawnEnable;

    // Origin MoveTime
    float m_OriginMoveSpeed;

public:
    virtual void Start() override;
public :
    virtual void ActivateHitSubEffect();
    virtual void OnActiveMeleeAttackCollider();
    void OnResetOriginalMoveSpeed();

public :
    void OnGruntAttackSound();
    void OnGruntSpawnSound();
    
    // Getter
public:
    float GetGruntOriginSpeed() const
    {
        return m_OriginMoveSpeed;
    }
    bool IsStartActEnable() const
    {
        return m_StartAct;
    }
    bool IsSpawnEnable() const
    {
        return m_SpawnEnable;
    }
    // Setter
public:
    void SetStartActEnable(bool Enable)
    {
        m_StartAct = Enable;
    }
    void SetSpawnEnable(bool Enable)
    {
        m_SpawnEnable = Enable;
    }
    void DisableSpawnEnable()
    {
        m_SpawnEnable = false;
    }
};

