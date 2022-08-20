#pragma once

#include "Component/Node/ActionNode.h"

class CCrowBossCutScenePlayNode :
    public CActionNode
{
public:
    CCrowBossCutScenePlayNode();
    CCrowBossCutScenePlayNode(const CCrowBossCutScenePlayNode& Node);
    virtual ~CCrowBossCutScenePlayNode();

public:
    virtual void Init() override;

private:
    float m_AccTime;
    float m_CutScenePrevDelay;

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;

public:
    void CrowBossCutSceneCameraShake();
};

