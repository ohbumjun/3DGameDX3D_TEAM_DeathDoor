#pragma once

#include "Component\Node\ActionNode.h"

class CClimbDown :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClimbDown();
    CClimbDown(const CClimbDown& Node);
    virtual ~CClimbDown();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

