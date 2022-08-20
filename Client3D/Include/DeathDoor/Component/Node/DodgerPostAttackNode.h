#pragma once

#include "Component\Node\ActionNode.h"

class CDodgerPostAttackNode :
    public CActionNode
{
public:
    CDodgerPostAttackNode();
    CDodgerPostAttackNode(const CDodgerPostAttackNode& Node);
    virtual ~CDodgerPostAttackNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

