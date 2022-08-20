#pragma once

#include "Node.h"

class CConditionNode :
    public CNode
{
protected:
    CConditionNode();
    CConditionNode(const CConditionNode& Node);
    virtual ~CConditionNode();

public:
    virtual NodeResult OnStart(float DeltaTime) = 0;
    virtual NodeResult OnUpdate(float DeltaTime) = 0;
    virtual NodeResult OnEnd(float DeltaTime) = 0;

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

