#pragma once

#include "ObjectDataComponent.h"

#define HIT_EFFECT_FIRST	0x01
#define HIT_EFFECT_SECOND	0x02
#define HIT_EFFECT_THIRD	0x04
#define HIT_EFFECT_FOURTH	0x08
#define HIT_EFFECT_FIFTH	0x10

class CMonsterDataComponent :
    public CObjectDataComponent
{
	friend class CGameObject;

protected:
	CMonsterDataComponent();
	CMonsterDataComponent(const CMonsterDataComponent& com);
	virtual ~CMonsterDataComponent();

public:
	virtual void Start() override;
	virtual void Update(float DeltaTime) override;
	virtual void PostUpdate(float DeltaTime) override;
	virtual void Reset() override;
	virtual CMonsterDataComponent* Clone();

public:
	void LookPlayer(float DeltaTime);
	void RightLookPlayer(float DeltaTime);
	void LeftLookPlayer(float DeltaTime);
	void MoveZ(float DeltaTime);
	void ChangeColorBossDeath(float DeltaTime);	// 죽을 때 색상 변화가 필요한 보스나 몬스터의 경우 이 함수를 Update에서 호출

public:
    virtual void OnActiveMeleeAttackCollider();
    virtual void OnInActiveMeleeAttackCollider();
    virtual void OnHitMeleeAttack(const CollisionResult& Result);
    void OnEndAnimPostAttackDelayOn();
	void OnEndAnimPostAttackDelayOff();
    void OnEnableLookPlayer();
	void OnDisableLookPlayer();
	void OnEnableRightLookPlayer();
	void OnDisableRightLookPlayer();
	void OnEnableLeftLookPlayer();
	void OnDisableLeftLookPlayer();
	void OnEnableMoveZ();
	void OnDisableMoveZ();
	void OnResetPrevMoveZ();
	void OnKeepPrevMoveZ();
	void OnEnableRandomRotate();
	void OnSetMoveSpeedZero(float DeltaTime)
	{
		m_CurMoveSpeed = 0.f;
	}
    virtual void OnDeadPaperBurnEnd();			// PaperBurn 종료시 호출
    virtual void OnDeadAnimStart();				// Death Animation 시작시 호출
    virtual void OnDeadAnimEnd();				// Death Animation 종료시 호출
    virtual void OnPlayerEnterZone(const CollisionResult& Result);		// Boss Cutscene Trigger
	virtual void OnStartCutScene();
    virtual void OnEndCutScene();
	// Blood Particle
	virtual void OnActivateBloodParticle();
	// Spawn Particle
	virtual void OnActivateSpawnParticle();
	virtual void OnInActivateSpawnParticle();
	// AttackCoolTime
	void EnableAttackCoolDelay();

public:
	virtual void SetIsHit(bool Hit) override;

	void SetPostAttckDelaying(bool Enable)
	{
		m_PostAttackDelaying = Enable;
	}
	void OnCombatStart()
	{
		m_IsCombat = true;
	}
	void SetMoveZEnableMaxTime(float Time)
	{
		m_MoveZEnableMaxTime = Time;
	}
	void SetCurMoveSpeed(float Speed)
	{
		m_CurMoveSpeed = Speed;
	}	
	void SetCurRotSpeed(float Speed)
	{
		m_CurRotSpeed = Speed;
	}
	void SetAttackCoolDelayTimeMax(float Time)
	{
		m_AttackCoolDelayTimeMax = Time;
	}
	void SetAttackCoolDelayTimeEnable(bool Enable)
	{
		m_AttackCoolTimeEnable = Enable;

		if (Enable)
		{
			m_AttackCoolDelayTime = m_AttackCoolDelayTimeMax;
		}
	}
	void SetCurrentNodeNull();

	void SetPlayerEnterZone(bool Enter)
	{
		m_PlayerEnterZone = Enter;
	}

	void SetPlayerEnterZoneFalse()
	{
		m_PlayerEnterZone = false;
	}

	void SetCutScenePlaying(bool Playing)
	{
		m_IsCutScenePlaying = Playing;
	}

	void SetPahtFindExecute(bool Execute)
	{
		m_PathFindExecute = Execute;
	}

public:
	bool IsAttackCoolTimeEnable() const
	{
		return m_AttackCoolTimeEnable;
	}
	float GetRotateSpeed() const
	{
		return m_Data.RotateSpeedPerSec;
	}
	float GetMeleeAttackRange() const
	{
		return m_Data.MeleeAttackRange;
	}
	float GetCurMoveSpeed() const
	{
		return m_CurMoveSpeed;
	}
	float GetJumpAttackRange() const
	{
		return m_Data.JumpAttackRange;
	}

	class CParticleComponent* GetBloodParticle() const
	{
		return m_BloodParticle;
	}

	class CParticleComponent* GetSpawnParticle() const
	{
		return m_SpawnParticle;
	}

    class CColliderBox3D* GetMeleeAttackCollider() const
    {
        return m_MeleeAttackCollider;
    }

	CColliderBox3D* GetPlayerEnterZoneTrigger()	const
	{
		return m_PlayerEnterZoneTrigger;
	}

	class CCameraComponent* GetCutSceneCam() const
	{
		return m_CutSceneCam;
	}

	bool IsPostAttackDelaying() const
	{
		return m_PostAttackDelaying;
	}

	bool IsCombat() const
	{
		return m_IsCombat;
	}

	bool IsCutScenePlaying() const
	{
		return m_IsCutScenePlaying;
	}

	bool IsPlayerEnterZone() const
	{
		return m_PlayerEnterZone;
	}

	bool IsPathFindExecute()	const
	{
		return m_PathFindExecute;
	}

	float GetAnglePlayer();
	bool IsPlayerLeftBasedInLookDir(); // 플레이어가 몬스터가 보는 방향 기준 왼쪽에 있는지 판별
	bool IsPlayerInMeleeAttackRange();
	Vector3	ToPlayer();
	float DistToPlayer();
	bool IsPlayerInStopChaseRange();	// 길찾기를 종료해야 하는 범위에 있는지 (기본적으로 MeleeAttackRange)
	void SetRotateRandom();
	void RotateRandomly(float DeltaTime);
	virtual void DecreaseHP(int Amount) override;

public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

protected:
	// Hit 시 호출할 함수
	virtual void ActivateHitSubEffect();
	void ActiveHitEffect(float DeltaTime);
	void ChangeHitColor(int EffectNum);

protected:
	class CPlayerDataComponent* m_PlayerData;
	class CMonsterNavAgent* m_MonsterNavAgent;
	class CAnimationMeshComponent* m_AnimMesh;
	class CColliderBox3D* m_HitBox;
	class CColliderBox3D* m_MeleeAttackCollider;
	class CGameStateComponent* m_State;
	class CParticleComponent* m_BloodParticle;
	class CParticleComponent* m_SpawnParticle;

	bool m_PostAttackDelaying;  // 공격 후딜레이 중인지 여부
	bool m_IsCombat;			// 전투 시작 여부

	// Hit Effect 관련 변수
	bool m_HitEffectStart;
	float m_HitEffectTimer;
	float m_HitEffectMax;
	int m_HitEffectFlag;

	size_t m_MeshMatSize;
	std::vector<Vector4> m_vecOriginDiffuse;
	std::vector<Vector4> m_vecOriginSpecluar;
	std::vector<Vector4> m_vecOriginAmbient;
	std::vector<Vector4> m_vecOriginEmissive;

	// Attack Cool Time
	bool m_AttackCoolTimeEnable;  // true 라면, 공격 X
	float m_AttackCoolDelayTime;
	float m_AttackCoolDelayTimeMax;

	// Rot 
	float m_OriginRotSpeed;
	float m_CurRotSpeed;

	// Move 관련 변수
	float m_CurMoveSpeed;
	bool m_LookPlayer;
	bool m_MoveZ;
	bool m_PrevMoveZ;
	// 해당 시간 동안만 m_MoveZ 를 true 로 만들었다가, false 로 다시 세팅
	float m_MoveZEnableMaxTime;
	// 왼쪽으로만 무조건 돌아야 하는 순간
	bool m_LeftLookPlayer;
	// 오른쪽으로만 무조건 돌아야 하는 순간
	bool m_RightLookPlayer;

	// Death Effect 관련 변수
    class CPaperBurnComponent* m_PaperBurn;
    bool m_DeathColorChangeStart;
    float m_DeathTimer;
    float m_DeathColorChangeTimeMax;
	Vector4 m_DeathColorStart;
	Vector4	m_DeathColorEnd;
	std::vector<Vector4> m_OriginEmissive;

	// CutScene관련 변수
    bool m_PlayerEnterZone;
    class CColliderBox3D* m_PlayerEnterZoneTrigger;

    bool m_IsCutScenePlaying;
    class CCameraComponent* m_CutSceneCam;
    Vector3 m_CutSceneBeforePlayerPos;

	// 길찾기 종료하는 거리
	float m_StopChaseRange;
	bool m_PathFindExecute;

	// Monster 가 랜덤한 방향으로 회전
	bool m_RotateToRandom;
	bool m_RandomRotatePrevMoveZ;
	float m_RotateRandomAngle;
	Vector3 m_RotateRandomTargetPos; // 어떤 벡터 쪽으로 회전할 것인지

public:
	class CMonsterNavAgent* GetMonsterNavAgent()	const;

public:
	class CColliderBox3D* GetHitBox()	const;
};

