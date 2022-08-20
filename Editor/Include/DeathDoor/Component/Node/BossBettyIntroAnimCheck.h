#pragma once
#include "Component\Node\ConditionNode.h"

class CBossBettyIntroAnimCheck :
    public CConditionNode
{
public:
    CBossBettyIntroAnimCheck();
    CBossBettyIntroAnimCheck(const CBossBettyIntroAnimCheck& Node);
    virtual ~CBossBettyIntroAnimCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

