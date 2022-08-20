#pragma once

#include "Component/Node/ConditionNode.h"

class CMouseRButtonUpCheckNode :
    public CConditionNode
{
public:
    CMouseRButtonUpCheckNode();
    CMouseRButtonUpCheckNode(const CMouseRButtonUpCheckNode& Node);
    virtual ~CMouseRButtonUpCheckNode();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);


public:
    virtual NodeResult Invoke(float DeltaTime);
};

