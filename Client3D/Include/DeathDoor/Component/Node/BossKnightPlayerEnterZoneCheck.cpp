#include "BossKnightPlayerEnterZoneCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../KnightDataComponent.h"

CBossKnightPlayerEnterZoneCheck::CBossKnightPlayerEnterZoneCheck()
{
	SetTypeID(typeid(CBossKnightPlayerEnterZoneCheck).hash_code());
}

CBossKnightPlayerEnterZoneCheck::CBossKnightPlayerEnterZoneCheck(const CBossKnightPlayerEnterZoneCheck& Node)	:
	CConditionNode(Node)
{
}

CBossKnightPlayerEnterZoneCheck::~CBossKnightPlayerEnterZoneCheck()
{
}

NodeResult CBossKnightPlayerEnterZoneCheck::OnStart(float DeltaTime)
{
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool EnterZone = Data->IsPlayerEnterZone();

	if (EnterZone)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CBossKnightPlayerEnterZoneCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightPlayerEnterZoneCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
