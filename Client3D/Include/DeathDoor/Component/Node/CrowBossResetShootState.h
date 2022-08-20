#pragma once

#include "Component\Node\ActionNode.h"

class CCrowBossResetShootState :
    public CActionNode
{
public:
    CCrowBossResetShootState();
    CCrowBossResetShootState(const CCrowBossResetShootState& Node);
    virtual ~CCrowBossResetShootState();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

