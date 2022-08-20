#include "BossBettyFarAttackTypeCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BossBettyDataComponent.h"

CBossBettyFarAttackTypeCheck::CBossBettyFarAttackTypeCheck()
{
	SetTypeID(typeid(CBossBettyFarAttackTypeCheck).hash_code());
}

CBossBettyFarAttackTypeCheck::CBossBettyFarAttackTypeCheck(const CBossBettyFarAttackTypeCheck& Node)
{
}

CBossBettyFarAttackTypeCheck::~CBossBettyFarAttackTypeCheck()
{
}

NodeResult CBossBettyFarAttackTypeCheck::OnStart(float DeltaTime)
{
	// Spin 이면 true 를 리턴한다.
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	BossBettyFarAttackType BettyFarAttackType = Data->GetBettyFarAttackType();

	if (BettyFarAttackType == BossBettyFarAttackType::Spin)
		return NodeResult::Node_True;
	else
		return NodeResult::Node_False;
}

NodeResult CBossBettyFarAttackTypeCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossBettyFarAttackTypeCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
