#pragma once

#include "ObjectDataComponent.h"
#include "../DDFlag.h"
#include "GameObject/GameObject.h"
#include "../UI/UIManager.h"

class CPlayerDataComponent :
    public CObjectDataComponent
{
	friend class CGameObject;

protected:
	CPlayerDataComponent();
	CPlayerDataComponent(const CPlayerDataComponent& com);
	virtual ~CPlayerDataComponent();

private:
	PlayerData m_PlayerData;
	bool m_OnSlash;
	class CAnimationMeshComponent* m_AnimComp;
	class CPlayerNormalAttackCheckCollider* m_AttackCheckCollider;
	class CColliderComponent* m_Body;
	Vector3 m_AttackDir;
	std::queue<char> m_KeyStateQueue;
	int m_FrameCount;
	bool m_MouseLButtonDown;
	bool m_Unbeatable; // 구르기상태에 순간적으로 무적 상태인지
	float m_UnbeatableAccTime; // 구르기하고 무적시간 판정용 누적 시간 
	// --> OnRollEnd에 m_Unbeatable = false로 해주면 m_Unbeatable = false가 먼저 되고 HitCheck가 되어서
	// 구르기인데도 맞아 눕는 문제가 생길 수 있다
	float m_UnbeatableTime; // 구르기하고 무적시간 

	bool m_LadderUpEnable;
	bool m_LadderDownEnable;
	bool m_IsClimbingLadder;
	bool m_ClimbingStartEnable;

	bool m_HitBackUnbeatable;
	float m_HitBackUnbeatableAccTime;
	float m_HitBackUnbeatableTime;

	DDPlayerRollDirection m_RollDirection;

	class CGameObject* m_AdjLadder;
	class CStaticMeshComponent* m_Slash;
	class CAnimationMeshComponent* m_Sword;
	std::vector<class CParticleComponent*> m_vecMoveDust;
	int m_CurrentDustIndex;
	Vector3 m_SlashDir;

	Vector3 m_CamRelativePos;
	class CPaperBurnComponent* m_SlashPaperBurn;
	int m_ConsecutiveAttackCount;

	bool m_HPChargeTimeStart;
	float m_AccHPChargeTime;
	float m_HPChargeTime;

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CPlayerDataComponent* Clone();


public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

public:
	void AddHP(int HP)
	{
		m_Data.HP += HP;
	}

	void SetHPChargeTimeStart(bool Start)
	{
		m_HPChargeTimeStart = Start;
	}

	void SetConsecutiveAttackCount(int Count)
	{
		m_ConsecutiveAttackCount = Count;
	}

	void SetAdjLadder(class CGameObject* Ladder);

	void SetRollDirection(DDPlayerRollDirection Dir)
	{
		m_RollDirection = Dir;
	}

	void SetClimbingStartEnable(bool Enable)
	{
		m_ClimbingStartEnable = Enable;
	}

	void SetLadderUpEnable(bool Enable)
	{
		m_LadderUpEnable = Enable;
	}

	bool IsLadderUpEnable()	const
	{
		return m_LadderUpEnable;
	}

	void SetLadderDownEnable(bool Enable)
	{
		m_LadderDownEnable = Enable;
	}

	bool IsLadderDownEnable()	const
	{
		return m_LadderDownEnable;
	}

	void SetClimbingLadder(bool Enable)
	{
		m_IsClimbingLadder = Enable;
	}

	bool IsClimbingLadder()	const
	{
		return m_IsClimbingLadder;
	}

	bool IsClimbingStartEnable()	const
	{
		return m_ClimbingStartEnable;
	}

	void SetTrueOnSlash();

	void SetFalseOnSlash();

	void SetOnSlash(bool Slash)
	{
		m_OnSlash = Slash;
	}

	class CAnimationMeshComponent* GetSword()	const;

	DDPlayerRollDirection GetRollDirection()	const
	{
		return m_RollDirection;
	}

	bool GetOnSlash()	const
	{
		return m_OnSlash;
	}

	void SetPlayerAbilityArrow(float DeltaTime)
	{
		if (m_PlayerData.Abilty_Type == Player_Ability::Arrow)
			return;

		CUIManager::GetInst()->ActivateAbility(Player_Ability::Arrow);

		m_PlayerData.Abilty_Type = Player_Ability::Arrow;

	}

	void SetPlayerAbilityFire(float DeltaTime)
	{
		m_PlayerData.Abilty_Type = Player_Ability::Fire;

		//m_PlayerHook->Enable(false);
	}

	void SetPlayerAbilityChain(float DeltaTime)
	{
		if (m_PlayerData.Abilty_Type == Player_Ability::Hook)
			return;

		m_PlayerData.Abilty_Type = Player_Ability::Hook;

		CUIManager::GetInst()->ActivateAbility(Player_Ability::Hook);
	}

	void SetPlayerAbilityBomb(float DeltaTime)
	{
		if (m_PlayerData.Abilty_Type == Player_Ability::Bomb)
			return;

		m_PlayerData.Abilty_Type = Player_Ability::Bomb;

		CUIManager::GetInst()->ActivateAbility(Player_Ability::Bomb);
	}

	void SetSkillNone(float DeltaTime)
	{
		m_PlayerData.Abilty_Type = Player_Ability::None;
	}

	Player_Ability GetPlayerAbility()	const
	{
		return m_PlayerData.Abilty_Type;
	}

	int GetConsecutiveAttackCount()	const
	{
		return m_ConsecutiveAttackCount;
	}

	void SetAttackDir(const Vector3& Dir)
	{
		m_AttackDir = Dir;
	}

	bool IsUnbeatable()	const
	{
		return m_Unbeatable;
	}

	const Vector3& GetAttackDir()	const
	{
		return m_AttackDir;
	}

	void PushKeyState(const char State)
	{
		m_KeyStateQueue.push(State);
	}

	void PopKeyState()
	{
		m_KeyStateQueue.pop();
	}

	const char GetFrontKeyState()	const
	{
		return m_KeyStateQueue.front();
	}

	bool IsKeyStateQueueEmpty()	const
	{
		return m_KeyStateQueue.empty();
	}

	size_t GetInputQueueSize()	const
	{
		return m_KeyStateQueue.size();
	}

	virtual void DecreaseHP(int Amout)
	{
		if (!m_IsHit)
		{
			if (m_Data.HP > 0 && !m_Unbeatable && !m_HitBackUnbeatable)
			{
				CObjectDataComponent::DecreaseHP(Amout);
				CUIManager::GetInst()->DecreaseHP();
			}

			m_IsHit = true;
		}
	}

	const Vector3& GetCamRelativePos() const
	{
		return m_CamRelativePos;
	}

	class CGameObject* GetAdjLadder() const;

	class CAnimationMeshComponent* GetAnimationMeshComponent() const;

	void OnHitBack();
	void OnHitBackEnd();
	void OnHitRecoverEnd();
	void OnRoll();
	void OnRollEnd();
	void OnBombLift();
	void OnBombCountReset();

	void ForceUpdateAttackDirection();
	void OnEnableAttackCollider();
	void OnDisableAttackCollider();
	void OnResetDustParticle();
	void OnLadderStepSoundPlay();
};

