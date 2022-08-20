#pragma once
#include "Component\Node\ConditionNode.h"

// Jump , PunchDouble 이라는 2가지 종류의 원거리 공격 중에서
// 어떤 원거리 공격을 Check할지를 선택한다
class CBossBettyFarAttackTypeCheck :
    public CConditionNode
{
public:
    CBossBettyFarAttackTypeCheck();
    CBossBettyFarAttackTypeCheck(const CBossBettyFarAttackTypeCheck& Node);
    virtual ~CBossBettyFarAttackTypeCheck();

public:
    virtual NodeResult OnStart(float DeltaTime) override;
    virtual NodeResult OnUpdate(float DeltaTime) override;
    virtual NodeResult OnEnd(float DeltaTime) override;
};

