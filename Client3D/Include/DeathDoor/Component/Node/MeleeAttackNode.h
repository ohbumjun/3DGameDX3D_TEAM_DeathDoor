#pragma once

#include "Component\Node\ActionNode.h"

class CMeleeAttackNode :
    public CActionNode
{
    friend class CGameBehaviorTree;
    
public:
    CMeleeAttackNode();
    CMeleeAttackNode(const CMeleeAttackNode& Node);
    virtual ~CMeleeAttackNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

