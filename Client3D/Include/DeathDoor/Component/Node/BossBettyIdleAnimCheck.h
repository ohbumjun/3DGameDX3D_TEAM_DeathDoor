#pragma once
#include "Component\Node\ConditionNode.h"
class CBossBettyIdleAnimCheck :
    public CConditionNode
{
public:
    CBossBettyIdleAnimCheck();
    CBossBettyIdleAnimCheck(const CBossBettyIdleAnimCheck& Node);
    virtual ~CBossBettyIdleAnimCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

