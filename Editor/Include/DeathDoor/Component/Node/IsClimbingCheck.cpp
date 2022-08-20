
#include "IsClimbingCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CIsClimbingCheck::CIsClimbingCheck()
{
	SetTypeID(typeid(CIsClimbingCheck).hash_code());
}

CIsClimbingCheck::CIsClimbingCheck(const CIsClimbingCheck& Node) :
	CConditionNode(Node)
{
}

CIsClimbingCheck::~CIsClimbingCheck()
{
}

NodeResult CIsClimbingCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsClimbing = DataComp->IsClimbingLadder();

	if (IsClimbing)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CIsClimbingCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CIsClimbingCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}