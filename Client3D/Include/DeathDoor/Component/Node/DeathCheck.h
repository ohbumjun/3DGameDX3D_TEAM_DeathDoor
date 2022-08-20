#pragma once

#include "Component\Node\ConditionNode.h"

class CDeathCheck :
    public CConditionNode
{
public:
    CDeathCheck();
    CDeathCheck(const CDeathCheck& Node);
    virtual ~CDeathCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

