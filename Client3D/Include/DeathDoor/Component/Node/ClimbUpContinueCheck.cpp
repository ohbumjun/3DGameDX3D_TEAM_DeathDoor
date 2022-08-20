
#include "ClimbUpContinueCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CClimbUpContinueCheck::CClimbUpContinueCheck()
{
	SetTypeID(typeid(CClimbUpContinueCheck).hash_code());
}

CClimbUpContinueCheck::CClimbUpContinueCheck(const CClimbUpContinueCheck& Node) :
	CConditionNode(Node)
{
}

CClimbUpContinueCheck::~CClimbUpContinueCheck()
{
}

NodeResult CClimbUpContinueCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsClimbing = DataComp->IsClimbingLadder();
	bool UpEnable = DataComp->IsLadderUpEnable();

	if (IsClimbing && UpEnable)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CClimbUpContinueCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CClimbUpContinueCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}