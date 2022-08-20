#pragma once

#include "Component\Node\ConditionNode.h"

class CSKeyInputCheck :
    public CConditionNode
{
public:
    CSKeyInputCheck();
    CSKeyInputCheck(const CSKeyInputCheck& Node);
    virtual ~CSKeyInputCheck();

private:
    int m_FrameCount;

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

