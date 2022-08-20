#pragma once

#include "Component/Node/ActionNode.h"

class CCancleShootNode :
    public CActionNode
{
    friend class CGameBehaviorTree;

public:
    CCancleShootNode();
    CCancleShootNode(const CCancleShootNode& Node);
    virtual ~CCancleShootNode();

protected:
    virtual NodeResult OnStart(float DeltaTime);
    virtual NodeResult OnUpdate(float DeltaTime);
    virtual NodeResult OnEnd(float DeltaTime);

private:
    float m_CameraMoveSpeed;
    double m_CameraMoveTime;
    Vector3 m_CurrentCamPos;

public:
    virtual NodeResult Invoke(float DeltaTime) override;

};

