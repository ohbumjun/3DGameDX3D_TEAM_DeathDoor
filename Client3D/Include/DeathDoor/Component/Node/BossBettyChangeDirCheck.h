#pragma once
#include "Component\Node\ConditionNode.h"

class CBossBettyChangeDirCheck :
    public CConditionNode
{
public:
    CBossBettyChangeDirCheck();
    CBossBettyChangeDirCheck(const CBossBettyChangeDirCheck& Node);
    virtual ~CBossBettyChangeDirCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

