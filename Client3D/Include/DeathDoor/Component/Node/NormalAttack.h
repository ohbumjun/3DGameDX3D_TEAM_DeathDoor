#pragma once

#include "Component/Node/ActionNode.h"

class CNormalAttack :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CNormalAttack();
    CNormalAttack(const CNormalAttack& Node);
    virtual ~CNormalAttack();

private:
    class CNavAgent* m_NavAgent;
    float   m_AccDistance;  // 평타 한번 칠때 앞으로 나아간 거리
    int m_ConsecutiveAttackCount;

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

