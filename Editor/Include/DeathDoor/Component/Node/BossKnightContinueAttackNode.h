#pragma once

#include "Component/Node/ActionNode.h"

class CBossKnightContinueAttackNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CBossKnightContinueAttackNode();
    CBossKnightContinueAttackNode(const CBossKnightContinueAttackNode& Node);
    virtual ~CBossKnightContinueAttackNode();

public:
    virtual void Init() override;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    float m_RotatePerSec;
};

