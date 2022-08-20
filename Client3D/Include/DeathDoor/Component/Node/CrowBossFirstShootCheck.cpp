
#include "CrowBossFirstShootCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"

CCrowBossFirstShootCheck::CCrowBossFirstShootCheck()
{
	SetTypeID(typeid(CCrowBossFirstShootCheck).hash_code());
}

CCrowBossFirstShootCheck::CCrowBossFirstShootCheck(const CCrowBossFirstShootCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossFirstShootCheck::~CCrowBossFirstShootCheck()
{
}

NodeResult CCrowBossFirstShootCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	int Count = Data->GetCurrentShootCount();

	if (Count == 0)
	{
		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CCrowBossFirstShootCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossFirstShootCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
