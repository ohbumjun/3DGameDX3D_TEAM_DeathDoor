#pragma once

#include "Component\Node\ConditionNode.h"

class CPathFindExecuteCheck :
    public CConditionNode
{
    friend class CGameBehaviorTree;

public:
    CPathFindExecuteCheck();
    CPathFindExecuteCheck(const CPathFindExecuteCheck& Node);
    virtual ~CPathFindExecuteCheck();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

