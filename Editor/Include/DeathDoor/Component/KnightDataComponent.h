#pragma once

#include "MonsterDataComponent.h"

enum class Knight_Attack_Rot_Type
{
    Front,
    CW,
    CCW,
};

class CKnightDataComponent final :
    public CMonsterDataComponent 
{
    friend class CGameObject;

private:
    CKnightDataComponent();
    CKnightDataComponent(const CKnightDataComponent& com);
    virtual ~CKnightDataComponent();

public:
    virtual void Start() override;
	virtual void Update(float DeltaTime) override;

public:
    virtual void OnActiveMeleeAttackCollider() override;
    void OnPlayWalkSound();
    void OnLookPlayerMove(float DeltaTime);
    void OnEndAnimJudgeContinueAttack();
    void OnEnableLookAndMove();
    void OnDisableLookAndMove();
    void OnWalk(float DeltaTime);
    void OnStartJumpAttackMove();
    void OnEndJumpAttackMove();
    void OnEndJumpAttack();
    void OnEndContinueAttack();
    void OnCutSceneSlamFloor();
    virtual void OnDeadAnimStart() override;
    virtual void OnDeadPaperBurnEnd() override;
    void OnReachedCutSceneAnimPoint();
    virtual void OnStartCutScene() override;
    virtual void OnEndCutScene() override;
    virtual void OnPlayerEnterZone(const CollisionResult& Result) override;

public:
    int GetMeleeAttackCount() const
    {
        return m_MeleeAttackCount;
    }

    float GetJumpAttackRange() const
    {
        return m_JumpAttackRange;
    }

    bool IsContinueAttack() const
    {
        return m_ContinueAttack;
    }

    Knight_Attack_Rot_Type GetAttackRotationType() const
    {
        return m_AttackRot;
    }

    const Vector3& GetFinalTraceAxisZ()
    {
        return m_FinalTraceZAxis;
    }

public:
    void IncreaseMeleeAttackCount()
    {
        ++m_MeleeAttackCount;
    }

    void ResetMeleeAttackCount()
    {
        m_MeleeAttackCount = 0;
    }

    void SetContinueAttack(bool Continue)
    {
        m_ContinueAttack = Continue;
    }

    void SetAttackRot(Knight_Attack_Rot_Type Type)
    {
        m_AttackRot = Type;
    }

protected:
    class CSceneComponent* m_ParticlePos;
    Knight_Attack_Rot_Type m_AttackRot;
    Vector3 m_FinalTraceZAxis;
    bool m_ContinueAttack;
    float m_JumpAttackRange;

    float m_OriginDOFMin;
    float m_OriginDOFMax;

    int m_MeleeAttackCount;
    int m_WalkSoundCount;
};

