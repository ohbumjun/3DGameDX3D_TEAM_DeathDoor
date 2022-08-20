#pragma once
#include "MonsterDataComponent.h"

enum class BossBettyFarAttackType
{
    Spin,
    JumpSmash
};

enum class BossBettyHPState
{
    None,
    Below60,
    Below30
};

class CBossBettyDataComponent :
    public CMonsterDataComponent
{
    friend class CGameObject;

private:
    CBossBettyDataComponent();
    CBossBettyDataComponent(const CBossBettyDataComponent& com);
    virtual ~CBossBettyDataComponent();

private :
    // Init Idle (처음에는 계속 Idle Animation 만 진행한다.)
    bool m_IsInitIdle;

    // 그러다가 CutScene 에서 Intro 로 변경한다.
    bool m_IsIntroAnimation;

    // 근거리 연속 공격 횟수
    int m_CloseSequentialAttackNum;

    // 원거리 공격 횟수 -> 3번 마다, Spin , Jump 를 번갈아가면서 실행할 것이다.
    int m_FarAttackAttackNum;

    // 6번 원거리 공격 Gauge 가 다 차면, Throw 를 할 것이다.
    bool m_ThrowFarAttackEnable;

    // 원거리 공격 Type
    BossBettyFarAttackType m_FarAttackType;

    // 근거리 공격 Animaion Change 가 가능한가 (여러 Close Attack 종류가 존재한다)
    bool m_CloseAttackAnimChangeEnable;

    // Origin MoveTime
    float m_OriginMoveSpeed;

    // HP Max
    float m_BettyHPMax;

    // 60%, 30% 가 될 때마다, Angry Attack 중에서 첫번째 Attack 설정하기 
    BossBettyHPState m_BettyHPState;

    // 근거리 공격 범위 정보 (사각형)
    Vector3 m_PunchLeftSquarePos[4];
    Vector3 m_PunchRightSquarePos[4];
    Vector3 m_SlashLeftSquarePos[4];
    Vector3 m_SlashRightSquarePos[4];
    Vector3 m_TwoSideFrontSquarePos[4];

    // 공격 Point 지정 
    Vector3 m_RelativePunchRightPos;
    Vector3 m_RelativePunchLeftPos;
    Vector3 m_RelativeSlashRightPos;
    Vector3 m_RelativeSlashLeftPos;

    // 방향 전환 판단 Limit Angle
    float m_ChangeDirLimitAngle;

    // Spin 하는 중에 벽과 충돌했는지 여부를 추가적으로 계속 조사하기 위해서
    // ColliderBox3D 를 하나 더 둘 것이다.
    class CColliderComponent* m_BossBettySpinCollider;

    // Boss Betty Throw Ball 투사체 Object
    class CGameObject* m_BossBettyThrowObject;

    // Boss Betty Punch Down Particle
    // std::vector<CParticleComponent*> m_vecAttackAfterEffectParticle;

    // Boss Betty Slash Particle
    // Boss Betty Roar Particle
    class CParticleComponent* m_BossBettyRoarParticle;

public:
    virtual void Start() override;
    virtual void OnActivateBloodParticle() override;
    virtual void DecreaseHP(int Amount);
public :
    // (아래 콜백 함수들은, 여러 Action Node 들에서 공통으로 사용하는 효과)
    // 땅을 내리칠때, 양쪽에 Attack 효과를 내기
    void OnBossBettyGenerateTwoSideCloseAttackEffect();
    // Attack Collider 의 Relative Pos 를 0 으로 (Betty 몸통 위치로)
    void OnSetBossBettyAttackColliderPosToBettyBody();
    // 땅 내리칠때 오른쪽에 Attack 효과를 내기 
    void OnBossBettyGenerateRightCloseAttackEffect();
    // 땅 내리칠 때 왼쪽에 Attack 효과를 내기 
    void OnBossBettyGenerateLeftCloseAttackEffect();
    // 울부짖기 효과
    void OnBossBettyRoarEffect(float DeltaTime);
    // 눈덩이 투사체 충돌시 발생시킬 Callback 함수
    void OnExplodeBettyThrowBallCallback();
    // Spin 중간에 벽에 부딪히면, Spin Collide Animation 으로 바꿔주는 함수 
    void OnChangeFromSpinToSpinCollideWhenCollide(const CollisionResult&);
    // MapSurroundingCollider 밖으로 나가는 것을 방지해주는 함수
    void OnPreventGoingOutOfMapSurroundingCollider(const CollisionResult&);
    // Spin Collider 비활성화
    void OnBossBettyDisableSpinCollider();
    // Spin Collider 활성화
    void OnBossBettyEnableSpinCollider();
    // Current Node 를 nullptr 로 세팅하기
    void OnBossBettySetCurrentNodeNullPtr();
    // Move Speed 조정
    void OnBossBettyResetOriginalMoveSpeed();
    // Camera Shake 기능
    void OnBossBettyNormalShakeCamera();
    // MapSurroundingCollider 로 부터 서서히 벗어나게 하기 위한 Speed 세팅
    void OnBossBettyApplyOutOfMapSurroundingColliderMoveSpeed();
    // Betty 공격 Collider Enable 처리 여부
    void OnBossBettyEnableAttackCollider();
    void OnBossBettyDisableAttackCollider();
    // Betty Attack After Effect
    void OnBossBettyActivateAfterEffect(const Vector3& WorldPos);
    // Close Attack Anim Change 여부 
    void OnBossBettyEnableCloseAttackChangeAnim();
    void OnBossBettyDisableCloseAttackChangeAnim();
    // IntroAnimation 끝난 이후 Game Start 
    void OnBossBettyEndIntroAndStartGame();
    void OnBossBettyStartIntroAnimation();
    // HPState 를 강제로 Check 하도록 해당 Node 를 Current Node 로 세팅해주는 함수
    void OnBossBettyForceCheckHPState();
    // CutScene Camera 시작
    void OnBossBettyStartCutSceneCamera(const CollisionResult& Result);
    // Roar Particle Effect
    void OnBossBettyActivateRoarParticle();

    // Sound
    void OnBossBettyAttackDownSound();
    void OnBossBettyThrowBallSound();
    void OnBossBettyMakeBallSound();
    void OnBossBettyIntroStartSound();
    void OnBossBettyThrowBallLandSound(const Vector3&);
    void OnBossBettyThrowBallFallStartSound();
    void OnBossBettyLandAfterJumpSound();
    void OnBossBettyRollStartSound();
    void OnBossBettyJumpStartSound();
    void OnBossBettySlashSound();
    void OnBossBettyChangeAttackDirJumpSound();
    void OnBossBettyCollideWithWallSound();
    void OnBossBettyRoarSound();
    virtual void OnDeadPaperBurnEnd() override;

    
    // Setter 함수 ---
public:
    void SetBettyThrowBallObject(class CGameObject* Object)
    {
        m_BossBettyThrowObject = Object;
    }
    void SetOriginSpeed(float OriginSpeed)
    {
        m_OriginMoveSpeed = OriginSpeed;
    }
    void SetBossBettyThrowObject(class CGameObject* Object)
    {
        m_BossBettyThrowObject = Object;
    }
    void ResetBettyHPState()
    {
        m_BettyHPState = BossBettyHPState::None;
    }
    void SetThrowAttackEnable(bool Enable)
    {
        m_ThrowFarAttackEnable = Enable;
    }
    void SetCloseAttackAnimChangeEnable(bool Enable)
    {
        m_CloseAttackAnimChangeEnable = Enable;
    }
    void IncCloseSequentialAttackCount()
    {
        ++m_CloseSequentialAttackNum;
    }
    void IncFarAttackCount();
    
    void SetInitIdleEnable(bool Enable)
    {
        m_IsInitIdle = Enable;
    }
    void SetIntroAnimationEnable(bool Enable)
    {
        m_IsIntroAnimation = Enable;
    }
  
public:
    class CParticleComponent* GetBettyRoarParticle() const
    {
        return m_BossBettyRoarParticle;
    }
    bool IsInitIdle() const
    {
        return m_IsInitIdle;
    }
    bool IsIntroAnimation() const
    {
        return m_IsIntroAnimation;
    }
    float GetBossBettyHPMax() const
    {
        return m_BettyHPMax;
    }
    bool IsCloseAttackAnimChangeEnable() const
    {
        return m_CloseAttackAnimChangeEnable;
    }
    float GetOriginRotSpeed() const
    {
        return m_OriginRotSpeed;
    }
    float GetBossBettyChangeDirLimitAngle() const
    {
        return m_ChangeDirLimitAngle;
    }
    class CColliderComponent* GetBossBettySpinCollider() const
    {
        return m_BossBettySpinCollider;
    }
    class CGameObject* GetBossBettyThrowObject() const
    {
        return m_BossBettyThrowObject;
    }
    // 근거리 공격 위치 
    const Vector3& GetRelativePunchRightPos()
    {
        return m_RelativePunchRightPos;
    }
    const Vector3& GetRelativePunchLeftPos()
    {
        return m_RelativePunchLeftPos;
    }
    const Vector3& GetRelativeSlashRightPos()
    {
        return m_RelativeSlashRightPos;
    }
    const Vector3& GetRelativeSlashLeftPos()
    {
        return m_RelativeSlashLeftPos;
    }
    // 근거리 사정 범위 사각형
    const Vector3* GetPunchLeftSquarePoses()
    {
        return m_PunchLeftSquarePos;
    }
    const Vector3* GetPunchRightSquarePoses()
    {
        return m_PunchRightSquarePos;
    }
    const Vector3* GetSlashLeftSquarePoses()
    {
        return m_SlashLeftSquarePos;
    }
    const Vector3* GetSlashRightSquarePoses()
    {
        return m_SlashRightSquarePos;
    }
    const Vector3* GetTwoSideFrontSquarePoses()
    {
        return m_TwoSideFrontSquarePos;
    }
    bool IsThrowFarAttackEnable() const
    {
        return m_ThrowFarAttackEnable;
    }
    int GetCloseSequentialAttackNum() const
    {
        return m_CloseSequentialAttackNum;
    }
    BossBettyHPState GetBettyHPState() const
    {
        return m_BettyHPState;
    }
    BossBettyFarAttackType GetBettyFarAttackType() const
    {
        return m_FarAttackType;
    }
    float GetOriginMoveSpeed() const
    {
        return m_OriginMoveSpeed;
    }
};

