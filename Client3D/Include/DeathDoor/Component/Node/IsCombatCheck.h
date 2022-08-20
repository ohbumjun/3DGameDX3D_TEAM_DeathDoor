#pragma once

#include "Component/Node/ConditionNode.h"

class CIsCombatCheck :
    public CConditionNode
{
public:
    CIsCombatCheck();
    CIsCombatCheck(const CIsCombatCheck& Node);
    virtual ~CIsCombatCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

