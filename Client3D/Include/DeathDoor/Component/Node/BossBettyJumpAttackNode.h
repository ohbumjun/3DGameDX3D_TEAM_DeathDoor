#pragma once
#include "Component\Node\ActionNode.h"


// ���Ÿ� ���� ����
// PunchDouble �� Change
// - ���� �߿� Player �������� ������ �̵�
// - Player �������� ������ ȸ����
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

