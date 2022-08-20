#pragma once

#include "Component\Node\ConditionNode.h"

class CCrowBossPhasePickNode :
    public CConditionNode
{
public:
    CCrowBossPhasePickNode();
    CCrowBossPhasePickNode(const CCrowBossPhasePickNode& Node);
    virtual ~CCrowBossPhasePickNode();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

