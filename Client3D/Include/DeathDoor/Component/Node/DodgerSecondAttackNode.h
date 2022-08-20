#pragma once

#include "Component\Node\ActionNode.h"

class CDodgerSecondAttackNode :
    public CActionNode
{
public:
    CDodgerSecondAttackNode();
    CDodgerSecondAttackNode(const CDodgerSecondAttackNode& Node);
    virtual ~CDodgerSecondAttackNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

