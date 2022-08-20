#pragma once

#include "MonsterDataComponent.h"
#include "../DDFlag.h"

class CCrowBossDataComponent :
    public CMonsterDataComponent
{
	friend class CGameObject;

private:
	CCrowBossDataComponent();
	CCrowBossDataComponent(const CCrowBossDataComponent& com);
	virtual ~CCrowBossDataComponent();

public:
	virtual void Start() override;

private:
	//int m_ChainAttackCount;
	bool m_StartFlying; // 쇠사슬타고 날아가기 시작했는데
	bool m_StartJump; // 점프 시작했는지
	bool m_InFlying;
	bool m_StartStomp;
	int m_CurrentShootCount;
	CrowBossShootState m_ShootState;
	int m_Phase;
	bool m_PhasePick;
	// Bypass하는 기준을 잡기 위해 플레이어의 순간적 위치를 설정한다
	Vector3 m_PlayerOriginPos;
	Vector3 m_MyOriginPos;

	std::vector<class CStaticMeshComponent*> m_vecHookChain;
	float m_ShootAccTime;
	int m_CurrentHookIndex; 
	int m_ClearHookIndex;
	Vector2	m_UnitSize;
	Vector3	m_ShootDir;
	int m_HookChainTotal;

	std::queue<int> m_PhaseQueue;
	bool m_AfterShoot;

	bool m_SpittingStart;
	float m_SpittingAccTime;
	int m_CurrentTinyCrowIndex;

	bool m_ShootDirFixed;
	std::list<class CGameObject*> m_TinyCrowList;

	class CParticleComponent* m_FeatherParticle;

	bool m_FlySoundPlayed;
	bool m_ShootSoundPlayed;
	bool m_StepSoundPlayed;

public:
	virtual void Update(float DeltaTime);
	
public:
	void ClearTinyCrow();

	void ClearPhaseQueue()
	{
		while (!m_PhaseQueue.empty())
			m_PhaseQueue.pop();
	}

	void SetShootDirFixed(bool Fix)
	{
		m_ShootDirFixed = Fix;
	}

	void SetShootState(const CrowBossShootState& State)
	{
		m_ShootState = State;
	}

	void SetCurrentShootCount(int Count)
	{
		m_CurrentShootCount = Count;
	}
	
	void AddCurrentShootCount()
	{
		++m_CurrentShootCount;
	}

	void SetPlayerOriginPos(const Vector3& Pos)
	{
		m_PlayerOriginPos = Pos;
	}

	void SetMyOriginPos(const Vector3& Pos)
	{
		m_MyOriginPos = Pos;
	}

	void SetPhasePick(bool Pick)
	{
		m_PhasePick = Pick;
	}

	void SetPhase(int Phase)
	{
		m_Phase = Phase;
	}

	void SetStartJump(bool Start)
	{
		m_StartJump = Start;
	}

	void SetAfterShoot(bool Shoot)
	{
		m_AfterShoot = Shoot;
	}

	void SetStepSoundPlayed(bool Played)
	{
		m_StepSoundPlayed = Played;
	}

	bool IsAfterShoot()	const
	{
		return m_AfterShoot;
	}

	bool GetShootDirFixed() const
	{
		return m_ShootDirFixed;
	}

	size_t GetPhaseQueueSize()	const
	{
		return m_PhaseQueue.size();
	}

	int GetPhaseQueueFront()
	{
		return m_PhaseQueue.front();
	}
	
	bool IsSpittingStart()	const
	{
		return m_SpittingStart;
	}

	void PopPhaseQueue()
	{
		m_PhaseQueue.pop();
	}

	void PushPhaseQueue(int Phase)
	{
		m_PhaseQueue.push(Phase);
	}

	bool IsPhaseQueueEmpty()	const
	{
		return m_PhaseQueue.empty();
	}

	const CrowBossShootState& GetShootState()	const
	{
		return m_ShootState;
	}

	const Vector3& GetShootDir()	const
	{
		return m_ShootDir;
	}

	int GetCurrentShootCount()	const
	{
		return m_CurrentShootCount;
	}

	const Vector3& GetPlayerOriginPos()	const
	{
		return m_PlayerOriginPos;
	}

	const Vector3& GetMyOriginPos()	const
	{
		return m_MyOriginPos;
	}

	bool GetStartJump()	const
	{
		return m_StartJump;
	}

	bool GetStartStomp()	const
	{
		return m_StartStomp;
	}

	void SetSpittingStart(bool Start)
	{
		m_SpittingStart = Start;
	}

	int GetPhase()	const
	{
		return m_Phase;
	}

	bool GetPhasePick()	const
	{
		return m_PhasePick;
	}

	void SetStartFlying(bool Start)
	{
		m_StartFlying = Start;
	}

	bool IsStartFlying()	const
	{
		return m_StartFlying;
	}

	void SetInFlying(bool InFlying)
	{
		m_InFlying = InFlying;
	}

	bool IsInFlying()	const
	{
		return m_InFlying;
	}

	void SetStartStomp(bool Start)
	{
		m_StartStomp = Start;
	}

	bool IsStartStomp()	const
	{
		return m_StartStomp;
	}

	bool GetStepSoundPlayed()	const
	{
		return m_StepSoundPlayed;
	}

public:
	void OnEndCrowBossJump();
	void OnCollision(const CollisionResult& Result);
	void OnScreamSoundPlay();
	void OnFlySoundPlay();
	void OnStepSoundPlay();
	virtual void OnDeadAnimStart() override;
	virtual void OnDeadPaperBurnEnd() override;
	virtual void OnEndCutScene();

public:
	virtual void SetIsHit(bool Hit) override;
	void ShootChain(const Vector3& ShootDir, float DeltaTime);
	void Fly(const Vector3& FlyDir, float DeltaTime);
	void Teleport();
	bool Spitting(float DeltaTime);

};

