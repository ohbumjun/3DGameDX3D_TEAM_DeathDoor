#pragma once

#include "Component\Node\ConditionNode.h"
class CPathListEmptyCheck :
    public CConditionNode
{
public:
    CPathListEmptyCheck();
    CPathListEmptyCheck(const CPathListEmptyCheck& Node);
    virtual ~CPathListEmptyCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

