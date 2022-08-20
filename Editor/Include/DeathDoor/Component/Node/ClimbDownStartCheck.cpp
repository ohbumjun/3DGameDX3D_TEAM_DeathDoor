
#include "ClimbDownStartCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CClimbDownStartCheck::CClimbDownStartCheck()
{
	SetTypeID(typeid(CClimbDownStartCheck).hash_code());
}

CClimbDownStartCheck::CClimbDownStartCheck(const CClimbDownStartCheck& Node) :
	CConditionNode(Node)
{
}

CClimbDownStartCheck::~CClimbDownStartCheck()
{
}

NodeResult CClimbDownStartCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsClimbing = DataComp->IsClimbingLadder();
	bool DownEnable = DataComp->IsLadderDownEnable();

	if (!IsClimbing && DownEnable)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CClimbDownStartCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CClimbDownStartCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}