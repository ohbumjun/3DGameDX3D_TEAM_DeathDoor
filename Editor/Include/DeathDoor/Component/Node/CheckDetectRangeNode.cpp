
#include "CheckDetectRangeNode.h"
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "Scene/Scene.h"
#include "../MonsterNavAgent.h"

CCheckDetectRangeNode::CCheckDetectRangeNode() :
	m_DetectRange(0.f)
{
	SetTypeID(typeid(CCheckDetectRangeNode).hash_code());
}

CCheckDetectRangeNode::CCheckDetectRangeNode(const CCheckDetectRangeNode& Node) :
	CConditionNode(Node)
{
}

CCheckDetectRangeNode::~CCheckDetectRangeNode()
{
}

NodeResult CCheckDetectRangeNode::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	m_DetectRange = Data->GetDetectRange();
	m_Player = m_Object->GetScene()->GetPlayerObject();

	if (!m_Player)
	{
		return NodeResult::Node_True;
	}

	Vector3 PlayerPos = m_Player->GetWorldPos();
	Vector3 MyPos = m_Object->GetWorldPos();

	if (PlayerPos.Distance(MyPos) <= m_DetectRange)
	{
		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
	
}

NodeResult CCheckDetectRangeNode::OnUpdate(float DeltaTime)
{

	return NodeResult::Node_True;
}

NodeResult CCheckDetectRangeNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCheckDetectRangeNode::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}
