#pragma once

#include "Component\Node\ConditionNode.h"

class CCrowBossJumpAttackRangeCheck :
    public CConditionNode
{
public:
    CCrowBossJumpAttackRangeCheck();
    CCrowBossJumpAttackRangeCheck(const CCrowBossJumpAttackRangeCheck& Node);
    virtual ~CCrowBossJumpAttackRangeCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

