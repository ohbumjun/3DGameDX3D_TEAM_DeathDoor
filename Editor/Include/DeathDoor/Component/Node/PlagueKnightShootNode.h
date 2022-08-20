#pragma once

#include "Component\Node\ActionNode.h"

class CPlagueKnightShootNode :
    public CActionNode
{
public:
    CPlagueKnightShootNode();
    CPlagueKnightShootNode(const CPlagueKnightShootNode& Node);
    virtual ~CPlagueKnightShootNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

