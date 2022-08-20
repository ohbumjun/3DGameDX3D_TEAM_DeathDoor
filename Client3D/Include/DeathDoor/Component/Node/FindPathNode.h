#pragma once

#include "Component/Node/ActionNode.h"

class CFindPathNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CFindPathNode();
    CFindPathNode(const CFindPathNode& Node);
    virtual ~CFindPathNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

public:
    virtual NodeResult Invoke(float DeltaTime) override;

private:
    class CMonsterNavAgent* m_NavAgent;

};

