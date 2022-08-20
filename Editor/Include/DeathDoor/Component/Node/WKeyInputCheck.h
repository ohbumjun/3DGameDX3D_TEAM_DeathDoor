#pragma once

#include "Component/Node/ConditionNode.h"

class CWKeyInputCheck :
    public CConditionNode
{
public:
    CWKeyInputCheck();
    CWKeyInputCheck(const CWKeyInputCheck& Node);
    virtual ~CWKeyInputCheck();

private:
    int m_FrameCount;

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

