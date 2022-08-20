#pragma once

#include "Component/Node/ActionNode.h"

class CShootNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CShootNode();
    CShootNode(const CShootNode& Node);
    virtual ~CShootNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    bool m_InRestoreCam;
    bool m_Throw;

public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

