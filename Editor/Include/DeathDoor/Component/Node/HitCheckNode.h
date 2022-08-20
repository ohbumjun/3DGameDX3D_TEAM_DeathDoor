#pragma once

#include "Component/Node/ConditionNode.h"

class CHitCheckNode :
    public CConditionNode
{
    friend class CGameBehaviorTree;

public:
    CHitCheckNode();
    CHitCheckNode(const CHitCheckNode& Node);
    virtual ~CHitCheckNode();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

