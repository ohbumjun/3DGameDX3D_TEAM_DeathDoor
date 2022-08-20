#pragma once
#include "Component\Node\ConditionNode.h"

// ShootChain�� �ϱ� ���� ��������? -> CrowBossShootState�� �̹� Shoot�� �ƴϰ�, Fly�� �ƴϾ�� �Ѵ�
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

