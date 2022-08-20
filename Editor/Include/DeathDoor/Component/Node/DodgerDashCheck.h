#pragma once

#include "Component\Node\ConditionNode.h"

class CDodgerDashCheck :
    public CConditionNode
{
public:
    CDodgerDashCheck();
    CDodgerDashCheck(const CDodgerDashCheck& Node);
    virtual ~CDodgerDashCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

