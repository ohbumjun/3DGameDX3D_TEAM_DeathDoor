#pragma once
#include "Component\Node\ActionNode.h"

class CCrowBossRunNode :
    public CActionNode
{
public:
    CCrowBossRunNode();
    CCrowBossRunNode(const CCrowBossRunNode& Node);
    virtual ~CCrowBossRunNode();


public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

