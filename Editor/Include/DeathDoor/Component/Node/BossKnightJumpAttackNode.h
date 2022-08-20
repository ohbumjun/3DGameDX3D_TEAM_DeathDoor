#pragma once

#include "Component\Node\ActionNode.h"

class CBossKnightJumpAttackNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBossKnightJumpAttackNode();
    CBossKnightJumpAttackNode(const CBossKnightJumpAttackNode& Node);
    virtual ~CBossKnightJumpAttackNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

