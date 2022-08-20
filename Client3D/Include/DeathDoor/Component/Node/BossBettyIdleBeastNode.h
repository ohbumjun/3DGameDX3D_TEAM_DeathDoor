#pragma once
#include "Component\Node\ActionNode.h"

class CBossBettyIdleBeastNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBossBettyIdleBeastNode();
    CBossBettyIdleBeastNode(const CBossBettyIdleBeastNode& Node);
    virtual ~CBossBettyIdleBeastNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

