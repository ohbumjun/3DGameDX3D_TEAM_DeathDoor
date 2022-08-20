#pragma once

#include "Component\Node\ActionNode.h"

class CBossKnightMeleeAttackNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBossKnightMeleeAttackNode();
    CBossKnightMeleeAttackNode(const CBossKnightMeleeAttackNode& Node);
    virtual ~CBossKnightMeleeAttackNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};
 