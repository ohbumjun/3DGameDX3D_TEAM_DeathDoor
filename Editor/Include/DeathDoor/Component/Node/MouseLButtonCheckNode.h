#pragma once

#include "Component/Node/ConditionNode.h"

class CMouseLButtonCheckNode :
    public CConditionNode
{
public:
    CMouseLButtonCheckNode();
    CMouseLButtonCheckNode(const CMouseLButtonCheckNode& Node);
    virtual ~CMouseLButtonCheckNode();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    int m_FrameCount;


public:
    virtual NodeResult Invoke(float DeltaTime);

};

