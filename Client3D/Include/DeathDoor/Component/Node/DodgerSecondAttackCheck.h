#pragma once

#include "Component\Node\ConditionNode.h"

class CDodgerSecondAttackCheck :
    public CConditionNode
{
public:
    CDodgerSecondAttackCheck();
    CDodgerSecondAttackCheck(const CDodgerSecondAttackCheck& Node);
    virtual ~CDodgerSecondAttackCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

