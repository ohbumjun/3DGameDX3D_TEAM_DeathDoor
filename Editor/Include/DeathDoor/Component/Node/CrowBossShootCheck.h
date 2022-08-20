#pragma once
#include "Component\Node\ConditionNode.h"

// ShootChain을 하기 위한 조건인지? -> CrowBossShootState가 이미 Shoot이 아니고, Fly도 아니어야 한다
class CCrowBossShootCheck :
    public CConditionNode
{
public:
    CCrowBossShootCheck();
    CCrowBossShootCheck(const CCrowBossShootCheck& Node);
    virtual ~CCrowBossShootCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

