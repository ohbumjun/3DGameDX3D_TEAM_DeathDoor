#pragma once

#include "Component/ObjectComponent.h"
#include "Component/NavAgent.h"
#include "GameObject/GameObject.h"

class CObjectDataComponent :
    public CObjectComponent
{
	friend class CGameObject;

protected:
	CObjectDataComponent();
	CObjectDataComponent(const CObjectDataComponent& com);
	virtual ~CObjectDataComponent();

protected:
	ObjectData m_Data;
	bool m_IsHit;
	bool m_NoInterrupt;	// 다른 상태로 전환 불가능한 상태인지

public:
	void SetObjectData(const ObjectData& Data)
	{
		m_Data = Data;
	}

	virtual void SetIsHit(bool Hit)
	{
		m_IsHit = Hit;
		// m_IsHitPrevFrame = false;
	}

	float GetFlySpeed()	const
	{
		return m_Data.FlySpeed;
	}

	bool GetIsHit()	const
	{
		return m_IsHit;
	}

	void SetHP(int HP)
	{
		m_Data.HP = HP;
	}

	virtual void DecreaseHP(int Amount)
	{
		m_Data.HP -= Amount;
	}

	void SetFlySpeed(float Speed)
	{
		m_Data.FlySpeed = Speed;
	}

	void SetDetectRange(float Range)
	{
		m_Data.DetectRange = Range;
	}

	void SetMoveSpeed(float Speed)
	{
		m_Data.MoveSpeed = Speed;

		CNavAgent* Agent = m_Object->FindObjectComponentFromType<CNavAgent>();

		if (Agent)
		{
			Agent->SetMoveSpeed(m_Data.MoveSpeed);
		}
	}

	void SetAttack(int Attack)
	{
		m_Data.Attack = Attack;
	}
	
	void SetMeleeAttackRange(float Range)
	{
		m_Data.MeleeAttackRange = Range;
	}

	int GetHP()	const
	{
		return m_Data.HP;
	}

	float GetDetectRange() const
	{
		return m_Data.DetectRange;
	}

	float GetMoveSpeed()	const
	{
		return m_Data.MoveSpeed;
	}

	int GetAttack()	const
	{
		return m_Data.Attack;
	}

	float GetMeleeAttackRange() const
	{
		return m_Data.MeleeAttackRange;
	}

	bool IsNoInterrupt()	const
	{
		return m_NoInterrupt;
	}

	void SetNoInterrupt(bool Enable)
	{
		m_NoInterrupt = Enable;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CObjectDataComponent* Clone();

public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;
};

