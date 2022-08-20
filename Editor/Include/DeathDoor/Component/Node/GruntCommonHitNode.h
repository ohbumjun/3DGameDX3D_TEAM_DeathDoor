#pragma once
#include "Component\Node\ActionNode.h"

class CGruntCommonHitNode :
    public CActionNode
{
public:
    CGruntCommonHitNode();
    CGruntCommonHitNode(const CGruntCommonHitNode& Node);
    virtual ~CGruntCommonHitNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

