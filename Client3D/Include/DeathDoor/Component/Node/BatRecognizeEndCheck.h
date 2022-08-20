#pragma once

#include "Component\Node\ConditionNode.h"

class CBatRecognizeEndCheck :
    public CConditionNode
{
public:
    CBatRecognizeEndCheck();
    CBatRecognizeEndCheck(const CBatRecognizeEndCheck& Node);
    virtual ~CBatRecognizeEndCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

