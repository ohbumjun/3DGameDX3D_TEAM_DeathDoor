#pragma once
#include "Component\Node\ConditionNode.h"
class CClimbKeyEnableCheck :
    public CConditionNode
{
public:
    CClimbKeyEnableCheck();
    CClimbKeyEnableCheck(const CClimbKeyEnableCheck& Node);
    virtual ~CClimbKeyEnableCheck();

public:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

