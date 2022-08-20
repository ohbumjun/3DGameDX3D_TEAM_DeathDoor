#include "StraightPathCheck.h"
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"

CStraightPathCheck::CStraightPathCheck()
{
	SetTypeID(typeid(CStraightPathCheck).hash_code());
}

CStraightPathCheck::CStraightPathCheck(const CStraightPathCheck& Node)	:
	CConditionNode(Node)
{
}

CStraightPathCheck::~CStraightPathCheck()
{
}

NodeResult CStraightPathCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CMonsterNavAgent* Nav = Data->GetMonsterNavAgent();

	CGameObject* PlayerObj = m_Object->GetScene()->GetPlayerObject();

	if (!PlayerObj)
	{
		return NodeResult::Node_False;
	}

	Vector3 MyPos = m_Object->GetWorldPos();
	Vector3 PlayerPos = PlayerObj->GetWorldPos();

	std::vector<Vector3> vecPath;
	bool PathExit = Nav->CheckStraightPath(MyPos, PlayerPos, vecPath);

	if (PathExit)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CStraightPathCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CStraightPathCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
