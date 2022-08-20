#pragma once
#include "Component/Node/ConditionNode.h"

class CCrowBossFirstShootCheck :
    public CConditionNode
{
public:
    CCrowBossFirstShootCheck();
    CCrowBossFirstShootCheck(const CCrowBossFirstShootCheck& Node);
    virtual ~CCrowBossFirstShootCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

