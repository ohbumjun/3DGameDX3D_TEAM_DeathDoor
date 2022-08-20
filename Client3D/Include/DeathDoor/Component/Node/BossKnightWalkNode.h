#pragma once

#include "Component\Node\ActionNode.h"

class CBossKnightWalkNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBossKnightWalkNode();
    CBossKnightWalkNode(const CBossKnightWalkNode& Node);
    virtual ~CBossKnightWalkNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

