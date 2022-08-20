#pragma once
#include "Component\Node\ActionNode.h"

class CLockstone3TriggerBoxAction :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CLockstone3TriggerBoxAction();
    CLockstone3TriggerBoxAction(const CLockstone3TriggerBoxAction& Node);
    virtual ~CLockstone3TriggerBoxAction();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

