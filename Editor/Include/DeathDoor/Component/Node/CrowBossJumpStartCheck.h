#pragma once

#include "Component/Node/ConditionNode.h"

class CCrowBossJumpStartCheck :
    public CConditionNode
{
public:
    CCrowBossJumpStartCheck();
    CCrowBossJumpStartCheck(const CCrowBossJumpStartCheck& Node);
    virtual ~CCrowBossJumpStartCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

