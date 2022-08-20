#include "MeleeRangeCheckNode.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"

CMeleeRangeCheckNode::CMeleeRangeCheckNode()
{
	SetTypeID(typeid(CMeleeRangeCheckNode).hash_code());
}

CMeleeRangeCheckNode::CMeleeRangeCheckNode(const CMeleeRangeCheckNode& Node)	:
	CConditionNode(Node)
{
}

CMeleeRangeCheckNode::~CMeleeRangeCheckNode()
{
}

NodeResult CMeleeRangeCheckNode::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool InRange = Data->IsPlayerInMeleeAttackRange();

	if (InRange)
	{
		return NodeResult::Node_True;
	}

	else
	{
		Data->SetPahtFindExecute(false);
		return NodeResult::Node_False;
	}
}

NodeResult CMeleeRangeCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMeleeRangeCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMeleeRangeCheckNode::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}
