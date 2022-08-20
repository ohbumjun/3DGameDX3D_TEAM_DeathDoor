#pragma once

#include "Component\Node\ActionNode.h"

class CClimbPause :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClimbPause();
    CClimbPause(const CClimbPause& Node);
    virtual ~CClimbPause();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

