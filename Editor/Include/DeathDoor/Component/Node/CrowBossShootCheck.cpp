

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

	// ShootChain�� �ϱ� ���� ��������? -> ShootChain�� State�� Shoot�� ���¿����� ��� ���;��ϰ�, ù��° Shoot�̶� State�� ReadyEnd�̰ų�, ù��° Shoot�� �ƴ϶� Teleport ���¿���
	// ���⼭ True�� �����ؼ� Sibling Node�� CrowBossShootNode�� ���� �Ѵ�
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
