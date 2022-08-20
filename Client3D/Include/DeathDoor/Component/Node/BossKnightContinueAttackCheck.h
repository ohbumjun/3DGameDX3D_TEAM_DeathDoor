#pragma once

#include "Component\Node\ConditionNode.h"

class CBossKnightContinueAttackCheck :
    public CConditionNode
{
public:
    CBossKnightContinueAttackCheck();
    CBossKnightContinueAttackCheck(const CBossKnightContinueAttackCheck& Node);
    virtual ~CBossKnightContinueAttackCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

