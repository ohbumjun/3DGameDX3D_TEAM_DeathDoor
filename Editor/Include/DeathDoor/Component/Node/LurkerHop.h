#pragma once

#include "Component\Node\ActionNode.h"

class CLurkerHop :
    public CActionNode
{
public:
    CLurkerHop();
    CLurkerHop(const CLurkerHop& Node);
    virtual ~CLurkerHop();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

