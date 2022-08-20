#pragma once

#include "Component\Node\ActionNode.h"

class CClimbUp :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClimbUp();
    CClimbUp(const CClimbUp& Node);
    virtual ~CClimbUp();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

