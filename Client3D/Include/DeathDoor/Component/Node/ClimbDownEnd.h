#pragma once

#include "Component\Node\ActionNode.h"

class CClimbDownEnd :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClimbDownEnd();
    CClimbDownEnd(const CClimbDownEnd& Node);
    virtual ~CClimbDownEnd();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

