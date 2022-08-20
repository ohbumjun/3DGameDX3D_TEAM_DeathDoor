#include "HeadRollerStunCheck.h"
#include "../HeadRollerDataComponent.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"

CHeadRollerStunCheck::CHeadRollerStunCheck()
{
	SetTypeID(typeid(CHeadRollerStunCheck).hash_code());
}

CHeadRollerStunCheck::CHeadRollerStunCheck(const CHeadRollerStunCheck& Node)	:
	CConditionNode(Node)
{
}

CHeadRollerStunCheck::~CHeadRollerStunCheck()
{
}

NodeResult CHeadRollerStunCheck::OnStart(float DeltaTime)
{
	CHeadRollerDataComponent* Data = dynamic_cast<CHeadRollerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool Stun = Data->IsStun();

	if (Stun)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CHeadRollerStunCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CHeadRollerStunCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
