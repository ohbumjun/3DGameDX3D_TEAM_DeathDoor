#pragma once

#include "Component\Node\ConditionNode.h"

class CHeadRollerStunCheck :
    public CConditionNode
{
public:
    CHeadRollerStunCheck();
    CHeadRollerStunCheck(const CHeadRollerStunCheck& Node);
    virtual ~CHeadRollerStunCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

