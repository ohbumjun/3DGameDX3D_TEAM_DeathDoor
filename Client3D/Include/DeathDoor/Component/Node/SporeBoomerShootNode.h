#pragma once

#include "Component/Node/ActionNode.h"

class CSporeBoomerShootNode :
    public CActionNode
{
public:
    CSporeBoomerShootNode();
    CSporeBoomerShootNode(const CSporeBoomerShootNode& Node);
    virtual ~CSporeBoomerShootNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

