#pragma once
#include "Component\Node\ActionNode.h"

class CGruntCommonTraceNode :
    public CActionNode
{
public:
    CGruntCommonTraceNode();
    CGruntCommonTraceNode(const CGruntCommonTraceNode& Node);
    virtual ~CGruntCommonTraceNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

