
#include "CrowBossShootEndCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"

CCrowBossShootEndCheck::CCrowBossShootEndCheck()
{
	SetTypeID(typeid(CCrowBossShootEndCheck).hash_code());
}

CCrowBossShootEndCheck::CCrowBossShootEndCheck(const CCrowBossShootEndCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossShootEndCheck::~CCrowBossShootEndCheck()
{
}

NodeResult CCrowBossShootEndCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CrowBossShootState State = Data->GetShootState();

	// Shoot이 끝난 직후와, Fly중일때도 계속 Fly노드로 들어가서 Fly를 호출해줘야하니 Fly일때도 Node_True를 리턴
	if (State != CrowBossShootState::Shoot && (State == CrowBossShootState::ShootEnd || State == CrowBossShootState::Fly))
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CCrowBossShootEndCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossShootEndCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
