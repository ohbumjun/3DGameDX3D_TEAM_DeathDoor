#include "BossKnightContinueAttackCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../KnightDataComponent.h"

CBossKnightContinueAttackCheck::CBossKnightContinueAttackCheck()
{
	SetTypeID(typeid(CBossKnightContinueAttackCheck).hash_code());
}

CBossKnightContinueAttackCheck::CBossKnightContinueAttackCheck(const CBossKnightContinueAttackCheck& Node)	:
	CConditionNode(Node)
{
}

CBossKnightContinueAttackCheck::~CBossKnightContinueAttackCheck()
{
}

NodeResult CBossKnightContinueAttackCheck::OnStart(float DeltaTime)
{
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool ContinueAttack = Data->IsContinueAttack();
	
	if (ContinueAttack)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CBossKnightContinueAttackCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossKnightContinueAttackCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
