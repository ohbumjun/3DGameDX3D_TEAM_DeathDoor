#pragma once

#include "Component/Node/ActionNode.h"

class CBossKnightCutScenePlayNode :
    public CActionNode
{
public:
    CBossKnightCutScenePlayNode();
    CBossKnightCutScenePlayNode(const CBossKnightCutScenePlayNode& Node);
    virtual ~CBossKnightCutScenePlayNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};
