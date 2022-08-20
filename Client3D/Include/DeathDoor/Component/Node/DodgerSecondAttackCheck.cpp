#include "DodgerSecondAttackCheck.h"
#include "../DodgerDataComponent.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"

CDodgerSecondAttackCheck::CDodgerSecondAttackCheck()
{
	SetTypeID(typeid(CDodgerSecondAttackCheck).hash_code());
}

CDodgerSecondAttackCheck::CDodgerSecondAttackCheck(const CDodgerSecondAttackCheck& Node)	:
	CConditionNode(Node)
{
}

CDodgerSecondAttackCheck::~CDodgerSecondAttackCheck()
{
}

NodeResult CDodgerSecondAttackCheck::OnStart(float DeltaTime)
{
	CDodgerDataComponent* Data = dynamic_cast<CDodgerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	int AttackCount = Data->GetAttackCount();

	if (AttackCount >= 1)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CDodgerSecondAttackCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CDodgerSecondAttackCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
