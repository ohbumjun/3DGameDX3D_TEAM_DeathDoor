#pragma once

#include "Component\Node\ConditionNode.h"

class CBatRecognizeStartCheck :
    public CConditionNode
{
public:
    CBatRecognizeStartCheck();
    CBatRecognizeStartCheck(const CBatRecognizeStartCheck& Node);
    virtual ~CBatRecognizeStartCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

