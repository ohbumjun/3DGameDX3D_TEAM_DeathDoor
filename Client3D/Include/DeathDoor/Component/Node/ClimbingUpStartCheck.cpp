
#include "ClimbingUpStartCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CClimbingUpStartCheck::CClimbingUpStartCheck()
{
	SetTypeID(typeid(CClimbingUpStartCheck).hash_code());
}

CClimbingUpStartCheck::CClimbingUpStartCheck(const CClimbingUpStartCheck& Node) :
	CConditionNode(Node)
{
}

CClimbingUpStartCheck::~CClimbingUpStartCheck()
{
}

NodeResult CClimbingUpStartCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsClimbing = DataComp->IsClimbingLadder();
	bool UpEnable = DataComp->IsLadderUpEnable();

	if (!IsClimbing && UpEnable)
	{
		return NodeResult::Node_True;
	}


	return NodeResult::Node_False;
}

NodeResult CClimbingUpStartCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CClimbingUpStartCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}