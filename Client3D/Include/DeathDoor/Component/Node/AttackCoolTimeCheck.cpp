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

	// AttackCoolTimeEnable 가 True 라면, 공격을 못한다는 의미
	bool AttackCoolTimeEnable = Data->IsAttackCoolTimeEnable();

	// AttackCoolTimeEnable 가 False 여야만, Attack 이 가능하게 해야 하므로
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
