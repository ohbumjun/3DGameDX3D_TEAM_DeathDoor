#pragma once

#include "Component\Node\ActionNode.h"

class CHeadRollerStunNode :
    public CActionNode
{
public:
    CHeadRollerStunNode();
    CHeadRollerStunNode(const CHeadRollerStunNode& Node);
    virtual ~CHeadRollerStunNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

