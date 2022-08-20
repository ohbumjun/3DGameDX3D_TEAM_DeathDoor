#pragma once

#include "Component/Node/ConditionNode.h"

class CCheckDetectRangeNode :
    public CConditionNode
{
    friend class CGameBehaviorTree;

public:
    CCheckDetectRangeNode();
    CCheckDetectRangeNode(const CCheckDetectRangeNode& Node);
    virtual ~CCheckDetectRangeNode();

private:
    float   m_DetectRange;
    CGameObject* m_Player;

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime);
};

