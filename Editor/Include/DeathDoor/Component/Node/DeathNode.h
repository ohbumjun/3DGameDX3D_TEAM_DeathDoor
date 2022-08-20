#pragma once
#include "Component/Node/ActionNode.h"

class CDeathNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CDeathNode();
    CDeathNode(const CDeathNode& Node);
    virtual ~CDeathNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime) override;

};

