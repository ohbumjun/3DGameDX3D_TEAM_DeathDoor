#pragma once
#include "Component\Node\ActionNode.h"


// 1. Jump Spin 
// - ������ Frame �� Spin ���� Change
// 2. Spin
// - Collide �ϴ� ���� Spin Collide �� Change
// 3. Spin Collide
// - ������ Floor �� �����ϴ� ����, �ֺ� Damage
// - ���� �߿� Player �������� ȸ��

class CBossBettySpinAttackNode :
    public CActionNode
{
public:
    CBossBettySpinAttackNode();
    CBossBettySpinAttackNode(const CBossBettySpinAttackNode& Node);
    virtual ~CBossBettySpinAttackNode();

public:
    virtual void Init() override;
    
protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
private :
    bool m_JumpSpinChangeEnable;
private:
    void OnBossBettyChangeToSpinAnimation();
    void OnBossBettyEnableSpinChange();
    void OnBossBettyDisableSpinChange();

public :
    void SetJumpSpinChangeEnable(bool Enable)
    {
        m_JumpSpinChangeEnable = Enable;
    }
};

