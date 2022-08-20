#pragma once
#include "Component/Node/ConditionNode.h"

class CCrowBossDoneCheck :
    public CConditionNode
{
public:
    CCrowBossDoneCheck();
    CCrowBossDoneCheck(const CCrowBossDoneCheck& Node);
    virtual ~CCrowBossDoneCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

