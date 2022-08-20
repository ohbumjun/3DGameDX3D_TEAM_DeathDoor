#pragma once
#include "Component/Node/ConditionNode.h"

// 자신이 어떤 State를 갖고 있는지 체크
class CCheckState :
    public CConditionNode
{
    friend class CGameBehaviorTree;

public:
    CCheckState();
    CCheckState(const CCheckState& Node);
    virtual ~CCheckState();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime);
};

