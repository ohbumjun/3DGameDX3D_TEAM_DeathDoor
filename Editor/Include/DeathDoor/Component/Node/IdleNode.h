#pragma once

#include "Component/Node/ActionNode.h"

class CIdleNode :
    public CActionNode
{
    friend class CGameBehaviorTree;
    
public:
    CIdleNode();
    CIdleNode(const CIdleNode& Node);
    virtual ~CIdleNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

