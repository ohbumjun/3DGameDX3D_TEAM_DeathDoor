#include "BossKnightFinalAttackCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../KnightDataComponent.h"

CBossKnightFinalAttackCheck::CBossKnightFinalAttackCheck()
{
	SetTypeID(typeid(CBossKnightFinalAttackCheck).hash_code());
}

CBossKnightFinalAttackCheck::CBossKnightFinalAttackCheck(const CBossKnightFinalAttackCheck& Node)	:
	CConditionNode(Node)
{
}

CBossKnightFinalAttackCheck::~CBossKnightFinalAttackCheck()
{
}

NodeResult CBossKnightFinalAttackCheck::OnStart(float DeltaTime)
{
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	int MeleeAttackCount = Data->GetMeleeAttackCount();

	if (MeleeAttackCount >= 2)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CBossKnightFinalAttackCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightFinalAttackCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
