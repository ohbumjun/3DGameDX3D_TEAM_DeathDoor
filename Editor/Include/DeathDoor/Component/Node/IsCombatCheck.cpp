#include "IsCombatCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"
#include "Scene/Scene.h"

CIsCombatCheck::CIsCombatCheck()
{
	SetTypeID(typeid(CIsCombatCheck).hash_code());
}

CIsCombatCheck::CIsCombatCheck(const CIsCombatCheck& Node)	:
	CConditionNode(Node)
{
}

CIsCombatCheck::~CIsCombatCheck()
{
}

NodeResult CIsCombatCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsCombatStart = Data->IsCombat();

	if (IsCombatStart)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CIsCombatCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CIsCombatCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
