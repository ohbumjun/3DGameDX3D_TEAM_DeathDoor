#pragma once

#include "Component/Node/ConditionNode.h"

class CCrowBossBypassCheck :
    public CConditionNode
{
public:
    CCrowBossBypassCheck();
    CCrowBossBypassCheck(const CCrowBossBypassCheck& Node);
    virtual ~CCrowBossBypassCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

