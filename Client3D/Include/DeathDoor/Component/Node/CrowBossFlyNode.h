#pragma once

#include "Component/Node/ActionNode.h"

class CCrowBossFlyNode :
    public CActionNode
{
public:
    CCrowBossFlyNode();
    CCrowBossFlyNode(const CCrowBossFlyNode& Node);
    virtual ~CCrowBossFlyNode();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

