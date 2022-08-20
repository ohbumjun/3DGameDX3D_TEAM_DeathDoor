#pragma once

#include "Component/Node/ActionNode.h"

class CMoveNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CMoveNode();
    CMoveNode(const CMoveNode& Node);
    virtual ~CMoveNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    class CNavAgent* m_NavAgent;
    bool m_ClockWise;
    Vector3 m_AccOffsetChange;

public:
    void SetNavAgent(class CNavAgent* Agent);

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

