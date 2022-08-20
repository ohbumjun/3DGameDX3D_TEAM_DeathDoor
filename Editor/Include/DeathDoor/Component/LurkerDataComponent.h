#pragma once

#include "MonsterDataComponent.h"

class CLurkerDataComponent :
    public CMonsterDataComponent
{
    friend class CGameObject;

private:
	CLurkerDataComponent();
	CLurkerDataComponent(const CLurkerDataComponent& com);
	virtual ~CLurkerDataComponent();

private:
	bool m_HopStart;
	Vector3 m_PrevAttackPos;
	bool m_DeathAnimStart;

public:
	void SetPrevAttackPos(const Vector3& Pos)
	{
		m_PrevAttackPos = Pos;
	}

	void SetHopStart(bool Start)
	{
		m_HopStart = Start;
	}

	const Vector3& GetPrevAttackPos()	const
	{
		return m_PrevAttackPos;
	}

	bool IsHopStart()	const
	{
		return m_HopStart;
	}

public:
	virtual void Start() override;
	virtual void Update(float DeltaTime) override;
	virtual void OnHitMeleeAttack(const CollisionResult& Result);
	virtual void SetIsHit(bool Hit) override;

public:
	void OnHopEnd();
	void OnEndMeleeAttackPrep();
	void OnEndMeleeAttack();
	void OnMeleeAttackColliderEnable();
	void OnMeleeAttackColliderDisable();
	void OnMeleeAttackRush();
	void OnRestorePrevAttackPos();
	void OnAttackSoundPlay();
	void OnDeathSoundPlay();
	void OnPrepAttackSoundPlay();
	void OnStepSoundPlay();

public:
	void LookPlayer();
};

