#pragma once

#include "Component\Node\ConditionNode.h"

class CPlayerEnterTriggerIsEnable :
    public CConditionNode
{
public:
    CPlayerEnterTriggerIsEnable();
    CPlayerEnterTriggerIsEnable(const CPlayerEnterTriggerIsEnable& Node);
    virtual ~CPlayerEnterTriggerIsEnable();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

