
#include "PathListEmptyCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"
#include "../MonsterNavAgent.h"

CPathListEmptyCheck::CPathListEmptyCheck()
{
	SetTypeID(typeid(CPathListEmptyCheck).hash_code());
}

CPathListEmptyCheck::CPathListEmptyCheck(const CPathListEmptyCheck& Node) :
	CConditionNode(Node)
{
}

CPathListEmptyCheck::~CPathListEmptyCheck()
{
}

NodeResult CPathListEmptyCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CMonsterNavAgent* Agent = Data->GetMonsterNavAgent();


	if (Agent->IsEmptyPathList())
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CPathListEmptyCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CPathListEmptyCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
