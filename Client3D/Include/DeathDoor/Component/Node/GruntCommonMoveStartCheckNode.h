#pragma once
#include "Component\Node\ConditionNode.h"

class CGruntCommonMoveStartCheckNode :
    public CConditionNode
{
public:
    CGruntCommonMoveStartCheckNode();
    CGruntCommonMoveStartCheckNode(const CGruntCommonMoveStartCheckNode& Node);
    virtual ~CGruntCommonMoveStartCheckNode();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

