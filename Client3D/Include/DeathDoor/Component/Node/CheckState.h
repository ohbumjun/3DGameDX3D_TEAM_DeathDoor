#pragma once
#include "Component/Node/ConditionNode.h"

// �ڽ��� � State�� ���� �ִ��� üũ
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

