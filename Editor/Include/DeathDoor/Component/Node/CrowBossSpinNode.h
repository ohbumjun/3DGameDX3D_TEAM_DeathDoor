#pragma once

#include "Component/Node/ActionNode.h"

class CCrowBossSpinNode :
    public CActionNode
{
public:
    CCrowBossSpinNode();
    CCrowBossSpinNode(const CCrowBossSpinNode& Node);
    virtual ~CCrowBossSpinNode();

public:
    virtual void Init() override;

private:
    float m_AccRotation;
    float m_AccSlidingTime;
    float m_SpinDegree;
    bool m_RotateCW;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

