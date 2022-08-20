#pragma once
#include "Component\Node\ActionNode.h"
class CMonsterHitBack :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CMonsterHitBack();
    CMonsterHitBack(const CMonsterHitBack& Node);
    virtual ~CMonsterHitBack();
public:
    virtual void Init() override;
protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);
};

