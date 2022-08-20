#pragma once

#include "Component/Node/ConditionNode.h"

class CHPCheck :
    public CConditionNode
{
public:
    CHPCheck();
    CHPCheck(const CHPCheck& Node);
    virtual ~CHPCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime);
};

