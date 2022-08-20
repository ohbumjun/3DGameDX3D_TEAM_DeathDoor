#pragma once

#include "Component\Node\ActionNode.h"

class CHeadRollerRollNode :
    public CActionNode
{
public:
    CHeadRollerRollNode();
    CHeadRollerRollNode(const CHeadRollerRollNode& Node);
    virtual ~CHeadRollerRollNode();

public:
    virtual void Init() override;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;

private:
    class CHeadRollerDataComponent* m_Data;
    class CMonsterNavAgent* m_Nav;
    Vector3 m_PrevPos;
};

