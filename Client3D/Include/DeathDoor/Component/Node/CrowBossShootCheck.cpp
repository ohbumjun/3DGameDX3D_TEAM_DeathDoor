

#include "CrowBossShootCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"

CCrowBossShootCheck::CCrowBossShootCheck()
{
	SetTypeID(typeid(CCrowBossShootCheck).hash_code());
}

CCrowBossShootCheck::CCrowBossShootCheck(const CCrowBossShootCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossShootCheck::~CCrowBossShootCheck()
{
}

NodeResult CCrowBossShootCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CrowBossShootState State = Data->GetShootState();

	// ShootChain을 하기 위한 조건인지? -> ShootChain은 State가 Shoot인 상태에서도 계속 들어와야하고, 첫번째 Shoot이라서 State가 ReadyEnd이거나, 첫번째 Shoot이 아니라 Teleport 상태여도
	// 여기서 True를 리턴해서 Sibling Node인 CrowBossShootNode로 가야 한다
	if (State == CrowBossShootState::Shoot || State == CrowBossShootState::ReadyEnd || State == CrowBossShootState::Teleport)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CCrowBossShootCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossShootCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
