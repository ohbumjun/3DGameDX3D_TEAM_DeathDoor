#pragma once

#include "Component\Node\ConditionNode.h"

class CStraightPathCheck :
    public CConditionNode
{
public:
    CStraightPathCheck();
    CStraightPathCheck(const CStraightPathCheck& Node);
    virtual ~CStraightPathCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

