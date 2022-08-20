#pragma once

#include "Component\Node\ActionNode.h"

class CClearPathListNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CClearPathListNode();
    CClearPathListNode(const CClearPathListNode& Node);
    virtual ~CClearPathListNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

