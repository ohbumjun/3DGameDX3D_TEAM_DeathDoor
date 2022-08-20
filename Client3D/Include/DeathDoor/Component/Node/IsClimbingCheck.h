#pragma once

#include "Component\Node\ConditionNode.h"

class CIsClimbingCheck :
    public CConditionNode
{
public:
    CIsClimbingCheck();
    CIsClimbingCheck(const CIsClimbingCheck& Node);
    virtual ~CIsClimbingCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

