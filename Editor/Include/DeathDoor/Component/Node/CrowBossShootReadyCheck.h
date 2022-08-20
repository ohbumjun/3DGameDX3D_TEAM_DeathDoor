#pragma once

#include "Component/Node/ConditionNode.h"

class CCrowBossShootReadyCheck :
    public CConditionNode
{
public:
    CCrowBossShootReadyCheck();
    CCrowBossShootReadyCheck(const CCrowBossShootReadyCheck& Node);
    virtual ~CCrowBossShootReadyCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

