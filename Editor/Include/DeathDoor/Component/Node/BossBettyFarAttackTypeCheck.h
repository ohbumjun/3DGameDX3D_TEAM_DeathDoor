#pragma once
#include "Component\Node\ConditionNode.h"

// Jump , PunchDouble �̶�� 2���� ������ ���Ÿ� ���� �߿���
// � ���Ÿ� ������ Check������ �����Ѵ�
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

