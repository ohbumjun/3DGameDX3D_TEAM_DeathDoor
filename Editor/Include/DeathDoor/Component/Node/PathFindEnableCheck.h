#pragma once

#include "Component/Node/ConditionNode.h"

class CPathFindEnableCheck :
    public CConditionNode
{
    friend class CGameBehaviorTree;

public:
    CPathFindEnableCheck();
    CPathFindEnableCheck(const CPathFindEnableCheck& Node);
    virtual ~CPathFindEnableCheck();

private:


public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime);
};

