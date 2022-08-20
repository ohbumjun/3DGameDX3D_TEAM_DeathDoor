#pragma once

#include "Component\Node\ActionNode.h"

class CClimbUpStart :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClimbUpStart();
    CClimbUpStart(const CClimbUpStart& Node);
    virtual ~CClimbUpStart();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

