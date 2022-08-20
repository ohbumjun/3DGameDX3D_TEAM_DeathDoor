#pragma once

#include "Component\Node\ConditionNode.h"

class CClimbDownEndCheck :
    public CConditionNode
{
public:
    CClimbDownEndCheck();
    CClimbDownEndCheck(const CClimbDownEndCheck& Node);
    virtual ~CClimbDownEndCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

};

