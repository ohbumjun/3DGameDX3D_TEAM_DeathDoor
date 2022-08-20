#pragma once

#include "Component/Node/ConditionNode.h"

class CMouseRButtonCheckNode :
    public CConditionNode
{
public:
    CMouseRButtonCheckNode();
    CMouseRButtonCheckNode(const CMouseRButtonCheckNode& Node);
    virtual ~CMouseRButtonCheckNode();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    int m_FrameCount;

public:
    virtual NodeResult Invoke(float DeltaTime);
};

