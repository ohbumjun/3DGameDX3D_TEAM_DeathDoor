#pragma once

#include "Component/Node/ActionNode.h"

class CCrowBossSlide :
    public CActionNode
{
public:
    CCrowBossSlide();
    CCrowBossSlide(const CCrowBossSlide& Node);
    virtual ~CCrowBossSlide();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

