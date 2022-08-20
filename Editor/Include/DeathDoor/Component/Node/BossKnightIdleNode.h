#pragma once

#include "Component/Node/ActionNode.h"

class CBossKnightIdleNode :
    public CActionNode
{
protected:
    friend class CGameBehaviorTree;

public:
    CBossKnightIdleNode();
    CBossKnightIdleNode(const CBossKnightIdleNode& Node);
    virtual ~CBossKnightIdleNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

