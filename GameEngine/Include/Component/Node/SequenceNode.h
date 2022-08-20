#pragma once

#include "CompositeNode.h"

class CSequenceNode :
    public CCompositeNode
{
    friend class CBehaviorTree;

public:
    CSequenceNode();
    CSequenceNode(const CSequenceNode& Node);
    virtual ~CSequenceNode();

public:
    virtual NodeResult Invoke(float DeltaTime)   override;

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

};

