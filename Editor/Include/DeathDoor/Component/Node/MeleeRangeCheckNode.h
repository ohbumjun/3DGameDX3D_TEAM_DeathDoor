#pragma once

#include "Component/Node/ConditionNode.h"

class CMeleeRangeCheckNode :
    public CConditionNode
{
    friend class CGameBehaviorTree;

public:
    CMeleeRangeCheckNode();
    CMeleeRangeCheckNode(const CMeleeRangeCheckNode& Node);
    virtual ~CMeleeRangeCheckNode();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

