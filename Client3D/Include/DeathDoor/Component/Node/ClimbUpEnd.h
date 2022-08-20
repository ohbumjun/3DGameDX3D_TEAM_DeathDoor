#pragma once

#include "Component\Node\ActionNode.h"

class CClimbUpEnd :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClimbUpEnd();
    CClimbUpEnd(const CClimbUpEnd& Node);
    virtual ~CClimbUpEnd();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

