#pragma once

#include "Component\Node\ActionNode.h"

class CCrowBossSpitting :
    public CActionNode
{
public:
    CCrowBossSpitting();
    CCrowBossSpitting(const CCrowBossSpitting& Node);
    virtual ~CCrowBossSpitting();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

