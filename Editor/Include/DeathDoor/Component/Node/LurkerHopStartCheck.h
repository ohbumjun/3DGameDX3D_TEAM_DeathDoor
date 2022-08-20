#pragma once

#include "Component\Node\ConditionNode.h"

class CLurkerHopStartCheck :
    public CConditionNode
{
public:
    CLurkerHopStartCheck();
    CLurkerHopStartCheck(const CLurkerHopStartCheck& Node);
    virtual ~CLurkerHopStartCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

