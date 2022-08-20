
#include "CrowBossDoneCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"

CCrowBossDoneCheck::CCrowBossDoneCheck()
{
	SetTypeID(typeid(CCrowBossDoneCheck).hash_code());
}

CCrowBossDoneCheck::CCrowBossDoneCheck(const CCrowBossDoneCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossDoneCheck::~CCrowBossDoneCheck()
{
}

NodeResult CCrowBossDoneCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CrowBossShootState State = Data->GetShootState();

	if (State == CrowBossShootState::Done)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CCrowBossDoneCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossDoneCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
