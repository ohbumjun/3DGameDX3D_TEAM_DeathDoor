#pragma once

#include "Component\Node\ActionNode.h"

class CCrowBossMoveShootSpot :
    public CActionNode
{
public:
    CCrowBossMoveShootSpot();
    CCrowBossMoveShootSpot(const CCrowBossMoveShootSpot& Node);
    virtual ~CCrowBossMoveShootSpot();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

