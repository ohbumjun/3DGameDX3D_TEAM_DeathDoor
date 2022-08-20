#pragma once

#include "Component\Node\ActionNode.h"

class CDodgerDashNode :
    public CActionNode
{
public:
    CDodgerDashNode();
    CDodgerDashNode(const CDodgerDashNode& Node);
    virtual ~CDodgerDashNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

