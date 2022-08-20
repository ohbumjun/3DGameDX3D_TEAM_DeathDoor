#pragma once
#include "Component\Node\ActionNode.h"


// 원거리 점프 공격
// PunchDouble 로 Change
// - 진행 중에 Player 방향으로 서서히 이동
// - Player 방향으로 서서히 회전도
class CBossBettyJumpAttackNode :
    public CActionNode
{
public:
    CBossBettyJumpAttackNode();
    CBossBettyJumpAttackNode(const CBossBettyJumpAttackNode& Node);
    virtual ~CBossBettyJumpAttackNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
private:
    float m_JumpAccTime;
    float m_JumpTotalTime;
    float m_InitJumpStartSpeed;
    bool m_CalRealTimeChangingSpeed;
private :
    void OnBossBettyStartJump();
    void OnBossBettyEndJump();
    void OnBossBettySlowMoveSpeed();
    void OnBossBettyCalJumpAttackTimeAndDist();
};

