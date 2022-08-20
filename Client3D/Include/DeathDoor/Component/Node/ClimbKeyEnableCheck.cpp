
#include "ClimbKeyEnableCheck.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CClimbKeyEnableCheck::CClimbKeyEnableCheck()
{
	SetTypeID(typeid(CClimbKeyEnableCheck).hash_code());
}

CClimbKeyEnableCheck::CClimbKeyEnableCheck(const CClimbKeyEnableCheck& Node)	:
	CConditionNode(Node)
{
}

CClimbKeyEnableCheck::~CClimbKeyEnableCheck()
{
}

NodeResult CClimbKeyEnableCheck::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool ClimbingStartEnable = DataComp->IsClimbingStartEnable();
	bool IsClimbing = DataComp->IsClimbingLadder();

	if (ClimbingStartEnable || IsClimbing)
		return NodeResult::Node_True;

	else
		return NodeResult::Node_False;
}

NodeResult CClimbKeyEnableCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CClimbKeyEnableCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
