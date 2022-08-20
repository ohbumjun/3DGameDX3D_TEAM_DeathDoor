#pragma once
#include "Component\Node\ConditionNode.h"

class CClimbDownContinueCheck :
    public CConditionNode
{
public:
    CClimbDownContinueCheck();
    CClimbDownContinueCheck(const CClimbDownContinueCheck& Node);
    virtual ~CClimbDownContinueCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

