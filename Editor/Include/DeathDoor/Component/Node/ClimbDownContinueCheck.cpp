
#include "ClimbDownContinueCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CClimbDownContinueCheck::CClimbDownContinueCheck()
{
	SetTypeID(typeid(CClimbDownContinueCheck).hash_code());
}

CClimbDownContinueCheck::CClimbDownContinueCheck(const CClimbDownContinueCheck& Node) :
	CConditionNode(Node)
{
}

CClimbDownContinueCheck::~CClimbDownContinueCheck()
{
}

NodeResult CClimbDownContinueCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsClimbing = DataComp->IsClimbingLadder();
	bool DownEnable = DataComp->IsLadderDownEnable();

	if (IsClimbing && DownEnable)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CClimbDownContinueCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CClimbDownContinueCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}