#pragma once
#include "Component\Node\ConditionNode.h"
class CClimbDownStartCheck :
    public CConditionNode
{
public:
    CClimbDownStartCheck();
    CClimbDownStartCheck(const CClimbDownStartCheck& Node);
    virtual ~CClimbDownStartCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

