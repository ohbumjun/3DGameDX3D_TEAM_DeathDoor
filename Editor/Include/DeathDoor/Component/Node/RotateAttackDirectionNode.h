#pragma once

#include "Component/Node/ActionNode.h"

class CRotateAttackDirectionNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CRotateAttackDirectionNode();
    CRotateAttackDirectionNode(const CRotateAttackDirectionNode& Node);
    virtual ~CRotateAttackDirectionNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    Vector3 m_DestForwardVector;
    Vector3 m_CurrentForwardVector;
    bool m_Over180;
    Vector3 m_PickingPoint;
    NodeResult m_Result;

public:
    const Vector3 GetPickingPoint() const
    {
        return m_PickingPoint;
    }


public:
    virtual NodeResult Invoke(float DeltaTime) override;
};

