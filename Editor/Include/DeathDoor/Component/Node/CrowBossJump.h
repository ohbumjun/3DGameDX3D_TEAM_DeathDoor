#pragma once

#include "Component/Node/ActionNode.h"

class CCrowBossJump :
    public CActionNode
{
public:
    CCrowBossJump();
    CCrowBossJump(const CCrowBossJump& Node);
    virtual ~CCrowBossJump();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

