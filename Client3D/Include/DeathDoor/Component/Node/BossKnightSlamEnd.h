#pragma once

#include "Component\Node\ActionNode.h"

class CBossKnightSlamEnd :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBossKnightSlamEnd();
    CBossKnightSlamEnd(const CBossKnightSlamEnd& Node);
    virtual ~CBossKnightSlamEnd();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

