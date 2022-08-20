#pragma once

#include "Component\Node\ConditionNode.h"

class CPostAttackDelayCheck :
    public CConditionNode
{
public:
    CPostAttackDelayCheck();
    CPostAttackDelayCheck(const CPostAttackDelayCheck& Node);
    virtual ~CPostAttackDelayCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

