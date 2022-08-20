
#include "PathFindEnableCheck.h"
#include "../GameStateComponent.h"
#include "../MonsterDataComponent.h"
#include "../MonsterNavAgent.h"

CPathFindEnableCheck::CPathFindEnableCheck()
{
	SetTypeID(typeid(CPathFindEnableCheck).hash_code());
}

CPathFindEnableCheck::CPathFindEnableCheck(const CPathFindEnableCheck& Node)	:
	CConditionNode(Node)
{
}

CPathFindEnableCheck::~CPathFindEnableCheck()
{
}

NodeResult CPathFindEnableCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	CMonsterNavAgent* Agent = Data->GetMonsterNavAgent();

	if (Agent)
	{
		if (Agent->GetPathFindEnable())
			return NodeResult::Node_True;
		else
			return NodeResult::Node_False;
	}

	return NodeResult::Node_False;
}

NodeResult CPathFindEnableCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CPathFindEnableCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CPathFindEnableCheck::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}
