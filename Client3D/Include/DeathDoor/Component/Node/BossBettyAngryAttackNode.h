#pragma once
#include "Component\Node\ActionNode.h"

// 1. HP�� 30%, 60%�� �Ǵ� ���� --> FirstSlamAnger + Roar / �ֺ� Particle ������ Damage
class CBossBettyAngryAttackNode :
    public CActionNode
{
public:
    CBossBettyAngryAttackNode();
    CBossBettyAngryAttackNode(const CBossBettyAngryAttackNode& Node);
    virtual ~CBossBettyAngryAttackNode();
public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
private :
    // 1) BossBettyAngryType::HPDamageAngry
    // ���߿��� Snow Ball ���� �������� �Ѵ�.
    void OnBossBettyStartFallingSnowBallEffect();
};

