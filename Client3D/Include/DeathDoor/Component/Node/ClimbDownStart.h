#pragma once

#include "Component/Node/ActionNode.h"

class CClimbDownStart :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClimbDownStart();
    CClimbDownStart(const CClimbDownStart& Node);
    virtual ~CClimbDownStart();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

