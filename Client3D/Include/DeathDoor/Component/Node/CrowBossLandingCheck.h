#pragma once

#include "Component/Node/ConditionNode.h"

class CCrowBossLandingCheck :
    public CConditionNode
{
public:
    CCrowBossLandingCheck();
    CCrowBossLandingCheck(const CCrowBossLandingCheck& Node);
    virtual ~CCrowBossLandingCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

