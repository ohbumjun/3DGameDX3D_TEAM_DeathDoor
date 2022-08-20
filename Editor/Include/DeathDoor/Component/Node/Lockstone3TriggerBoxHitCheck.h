#pragma once

#include "Component/Node/ConditionNode.h"

class CLockstone3TriggerBoxHitCheck :
    public CConditionNode
{
public:
    CLockstone3TriggerBoxHitCheck();
    CLockstone3TriggerBoxHitCheck(const CLockstone3TriggerBoxHitCheck& Node);
    virtual ~CLockstone3TriggerBoxHitCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    class CGameObject* m_TriggerTarget;


public:
    virtual NodeResult Invoke(float DeltaTime);
};

