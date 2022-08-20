#include "AttackCoolTimeCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"

CAttackCoolTimeCheck::CAttackCoolTimeCheck()
{
	SetTypeID(typeid(CAttackCoolTimeCheck).hash_code());
}

CAttackCoolTimeCheck::CAttackCoolTimeCheck(const CAttackCoolTimeCheck& Node) :
	CConditionNode(Node)
{
}

CAttackCoolTimeCheck::~CAttackCoolTimeCheck()
{
}

NodeResult CAttackCoolTimeCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// AttackCoolTimeEnable �� True ���, ������ ���Ѵٴ� �ǹ�
	bool AttackCoolTimeEnable = Data->IsAttackCoolTimeEnable();

	// AttackCoolTimeEnable �� False ���߸�, Attack �� �����ϰ� �ؾ� �ϹǷ�
	if (AttackCoolTimeEnable == false)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CAttackCoolTimeCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CAttackCoolTimeCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
