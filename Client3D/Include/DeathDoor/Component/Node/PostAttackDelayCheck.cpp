#include "PostAttackDelayCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"

CPostAttackDelayCheck::CPostAttackDelayCheck()
{
	SetTypeID(typeid(CPostAttackDelayCheck).hash_code());
}

CPostAttackDelayCheck::CPostAttackDelayCheck(const CPostAttackDelayCheck& Node)	:
	CConditionNode(Node)
{
}

CPostAttackDelayCheck::~CPostAttackDelayCheck()
{
}

NodeResult CPostAttackDelayCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// 공격 후딜레이인가
	bool SlamRecovering = Data->IsPostAttackDelaying();

	if (SlamRecovering)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CPostAttackDelayCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CPostAttackDelayCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
