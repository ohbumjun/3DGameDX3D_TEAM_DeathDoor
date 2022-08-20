#pragma once

#include "Component\Node\ActionNode.h"

class CBossKnightFinalAttackNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBossKnightFinalAttackNode();
    CBossKnightFinalAttackNode(const CBossKnightFinalAttackNode& Node);
    virtual ~CBossKnightFinalAttackNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

