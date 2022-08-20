#pragma once

#include "MonsterDataComponent.h"

class CBatDataComponent :
    public CMonsterDataComponent
{
	friend class CGameObject;

private:
	CBatDataComponent();
	CBatDataComponent(const CBatDataComponent& com);
	virtual ~CBatDataComponent();

private:
	bool m_RecognizeStart;
	bool m_RecognizeEnd;
	bool m_DeathAnimStart;

public:

	void SetRecognizeStart(bool Start)
	{
		m_RecognizeStart = Start;
	}

	void SetRecognizeEnd(bool End)
	{
		m_RecognizeEnd = End;
	}

	bool IsRecognizeStart()	const
	{
		return m_RecognizeStart;
	}

	bool IsRecognizeEnd()	const
	{
		return m_RecognizeEnd;
	}



public:
	virtual void Start() override;
	virtual void Update(float DeltaTime) override;
	virtual void OnHitMeleeAttack(const CollisionResult& Result);

public:
	virtual void SetIsHit(bool Hit) override;

public:
	void OnRecognizeEnd();
	void OnEndMeleeAttack();
	void OnMeleeAttackColliderEnable();
	void OnMeleeAttackColliderDisable();
	void OnCollision(const CollisionResult& Result);
	void OnAttackSoundPlay();
	void OnDeathSoundPlay();
	void OnFlapSoundPlay();
};

