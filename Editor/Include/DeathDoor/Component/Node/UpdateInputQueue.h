#pragma once

#include "Component/Node/ConditionNode.h"

class CUpdateInputQueue :
    public CConditionNode
{
public:
    CUpdateInputQueue();
    CUpdateInputQueue(const CUpdateInputQueue& Node);
    virtual ~CUpdateInputQueue();

private:
    int m_FrameCount;
    class CPlayerDataComponent* m_DataComp;

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

