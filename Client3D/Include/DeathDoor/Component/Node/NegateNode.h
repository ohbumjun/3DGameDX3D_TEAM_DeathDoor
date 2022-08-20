#pragma once

#include "Component/Node/DecoratorNode.h"

class CNegateNode :
    public CDecoratorNode
{

    friend class CGameBehaviorTree;

public:
    CNegateNode();
    CNegateNode(const CNegateNode& Node);
    virtual ~CNegateNode();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime);
};

