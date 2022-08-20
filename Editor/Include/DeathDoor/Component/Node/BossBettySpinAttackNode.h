#pragma once
#include "Component\Node\ActionNode.h"


// 1. Jump Spin 
// - 마지막 Frame 에 Spin 으로 Change
// 2. Spin
// - Collide 하는 순간 Spin Collide 로 Change
// 3. Spin Collide
// - 마지막 Floor 에 도달하는 순간, 주변 Damage
// - 동작 중에 Player 방향으로 회전

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

