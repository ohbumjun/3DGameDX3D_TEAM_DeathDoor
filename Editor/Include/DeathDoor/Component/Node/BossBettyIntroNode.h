#pragma once
#include "Component\Node\ActionNode.h"

class CBossBettyIntroNode :
    public CActionNode
{
public:
    CBossBettyIntroNode();
    CBossBettyIntroNode(const CBossBettyIntroNode& Node);
    virtual ~CBossBettyIntroNode();
public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

