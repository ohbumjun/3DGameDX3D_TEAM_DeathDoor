#pragma once

#include "Component/Node/ActionNode.h"

class CHitBackNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CHitBackNode();
    CHitBackNode(const CHitBackNode& Node);
    virtual ~CHitBackNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

};

