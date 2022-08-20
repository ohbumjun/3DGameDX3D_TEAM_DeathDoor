#pragma once

#include "Component\Node\ConditionNode.h"

class CClimbingUpStartCheck :
    public CConditionNode
{
public:
    CClimbingUpStartCheck();
    CClimbingUpStartCheck(const CClimbingUpStartCheck& Node);
    virtual ~CClimbingUpStartCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

