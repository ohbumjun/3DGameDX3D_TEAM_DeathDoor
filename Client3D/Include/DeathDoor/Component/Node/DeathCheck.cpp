#include "DeathCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"
#include "Scene/Scene.h"

CDeathCheck::CDeathCheck()
{
	SetTypeID(typeid(CDeathCheck).hash_code());
}

CDeathCheck::CDeathCheck(const CDeathCheck& Node)	:
	CConditionNode(Node)
{
}

CDeathCheck::~CDeathCheck()
{
}

NodeResult CDeathCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = (CMonsterDataComponent*)dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData();

	int HP = Data->GetHP();

	if (HP <= 0)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CDeathCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CDeathCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
