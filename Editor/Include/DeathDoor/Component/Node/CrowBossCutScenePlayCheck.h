#pragma once

#include "Component/Node/ConditionNode.h"

class CCrowBossCutScenePlayCheck :
    public CConditionNode
{
public:
    CCrowBossCutScenePlayCheck();
    CCrowBossCutScenePlayCheck(const CCrowBossCutScenePlayCheck& Node);
    virtual ~CCrowBossCutScenePlayCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

