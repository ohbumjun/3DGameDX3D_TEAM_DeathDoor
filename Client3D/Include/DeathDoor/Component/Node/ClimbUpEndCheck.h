#pragma once

#include "Component\Node\ConditionNode.h"

class CClimbUpEndCheck :
    public CConditionNode
{
public:
    CClimbUpEndCheck();
    CClimbUpEndCheck(const CClimbUpEndCheck& Node);
    virtual ~CClimbUpEndCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

