
#include "ClimbDownEndCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CClimbDownEndCheck::CClimbDownEndCheck()
{
	SetTypeID(typeid(CClimbDownEndCheck).hash_code());
}

CClimbDownEndCheck::CClimbDownEndCheck(const CClimbDownEndCheck& Node) :
	CConditionNode(Node)
{
}

CClimbDownEndCheck::~CClimbDownEndCheck()
{
}

NodeResult CClimbDownEndCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsClimbing = DataComp->IsClimbingLadder();
	bool DownEnable = DataComp->IsLadderDownEnable();

	if (IsClimbing && !DownEnable)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CClimbDownEndCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CClimbDownEndCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}