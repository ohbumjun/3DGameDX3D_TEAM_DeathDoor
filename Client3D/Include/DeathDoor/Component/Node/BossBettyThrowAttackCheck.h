#pragma once
#include "Component\Node\ConditionNode.h"

class CBossBettyThrowAttackCheck :
    public CConditionNode
{
public:
    CBossBettyThrowAttackCheck();
    CBossBettyThrowAttackCheck(const CBossBettyThrowAttackCheck& Node);
    virtual ~CBossBettyThrowAttackCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

