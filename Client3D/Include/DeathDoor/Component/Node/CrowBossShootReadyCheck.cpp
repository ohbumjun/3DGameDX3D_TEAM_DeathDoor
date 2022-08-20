
#include "CrowBossShootReadyCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"

CCrowBossShootReadyCheck::CCrowBossShootReadyCheck()
{
	SetTypeID(typeid(CCrowBossShootReadyCheck).hash_code());
}

CCrowBossShootReadyCheck::CCrowBossShootReadyCheck(const CCrowBossShootReadyCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossShootReadyCheck::~CCrowBossShootReadyCheck()
{
}

NodeResult CCrowBossShootReadyCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CrowBossShootState State = Data->GetShootState();

	if (State == CrowBossShootState::Ready)
	{
		Data->SetShootState(CrowBossShootState::ReadyEnd);
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CCrowBossShootReadyCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossShootReadyCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
