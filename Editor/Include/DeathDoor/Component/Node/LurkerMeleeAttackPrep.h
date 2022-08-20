#pragma once

#include "Component\Node\ActionNode.h"

class CLurkerMeleeAttackPrep :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CLurkerMeleeAttackPrep();
    CLurkerMeleeAttackPrep(const CLurkerMeleeAttackPrep& Node);
    virtual ~CLurkerMeleeAttackPrep();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

