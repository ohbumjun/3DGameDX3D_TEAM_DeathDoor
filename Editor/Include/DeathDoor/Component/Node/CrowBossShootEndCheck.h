#pragma once

#include "Component\Node\ConditionNode.h"

class CCrowBossShootEndCheck :
    public CConditionNode
{
public:
    CCrowBossShootEndCheck();
    CCrowBossShootEndCheck(const CCrowBossShootEndCheck& Node);
    virtual ~CCrowBossShootEndCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

