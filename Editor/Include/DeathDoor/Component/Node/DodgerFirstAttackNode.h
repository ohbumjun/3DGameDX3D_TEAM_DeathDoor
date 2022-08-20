#pragma once

#include "Component\Node\ActionNode.h"

class CDodgerFirstAttackNode :
    public CActionNode
{
public:
    CDodgerFirstAttackNode();
    CDodgerFirstAttackNode(const CDodgerFirstAttackNode& Node);
    virtual ~CDodgerFirstAttackNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

