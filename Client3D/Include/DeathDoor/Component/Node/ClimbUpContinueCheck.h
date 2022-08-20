#pragma once

#include "Component/Node/ConditionNode.h"

class CClimbUpContinueCheck :
    public CConditionNode
{
public:
    CClimbUpContinueCheck();
    CClimbUpContinueCheck(const CClimbUpContinueCheck& Node);
    virtual ~CClimbUpContinueCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

