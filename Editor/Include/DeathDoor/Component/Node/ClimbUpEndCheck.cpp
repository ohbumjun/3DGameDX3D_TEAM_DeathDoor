
#include "ClimbUpEndCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CClimbUpEndCheck::CClimbUpEndCheck()
{
	SetTypeID(typeid(CClimbUpEndCheck).hash_code());
}

CClimbUpEndCheck::CClimbUpEndCheck(const CClimbUpEndCheck& Node) :
	CConditionNode(Node)
{
}

CClimbUpEndCheck::~CClimbUpEndCheck()
{
}

NodeResult CClimbUpEndCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsClimbing = DataComp->IsClimbingLadder();
	bool UpEnable = DataComp->IsLadderUpEnable();

	if (IsClimbing && !UpEnable)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CClimbUpEndCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CClimbUpEndCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}