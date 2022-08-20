
#include "PathFindExecuteCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"

CPathFindExecuteCheck::CPathFindExecuteCheck()
{
	SetTypeID(typeid(CPathFindExecuteCheck).hash_code());
}

CPathFindExecuteCheck::CPathFindExecuteCheck(const CPathFindExecuteCheck& Node)	:
	CConditionNode(Node)
{
}

CPathFindExecuteCheck::~CPathFindExecuteCheck()
{
}



NodeResult CPathFindExecuteCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool Execute = Data->IsPathFindExecute();

	if (Execute)
		return NodeResult::Node_True;
	else
		return NodeResult::Node_False;
}

NodeResult CPathFindExecuteCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CPathFindExecuteCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
