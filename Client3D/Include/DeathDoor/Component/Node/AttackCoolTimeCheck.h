#pragma once
#include "Component\Node\ConditionNode.h"
class CAttackCoolTimeCheck :
    public CConditionNode
{
public:
    CAttackCoolTimeCheck();
    CAttackCoolTimeCheck(const CAttackCoolTimeCheck& Node);
    virtual ~CAttackCoolTimeCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

