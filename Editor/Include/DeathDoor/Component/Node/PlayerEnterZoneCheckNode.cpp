
#include "PlayerEnterZoneCheckNode.h"
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "Component/ColliderBox3D.h"

CPlayerEnterZoneCheckNode::CPlayerEnterZoneCheckNode()
{
	SetTypeID(typeid(CPlayerEnterZoneCheckNode).hash_code());
}

CPlayerEnterZoneCheckNode::CPlayerEnterZoneCheckNode(const CPlayerEnterZoneCheckNode& Node) :
	CConditionNode(Node)
{
}

CPlayerEnterZoneCheckNode::~CPlayerEnterZoneCheckNode()
{
}

NodeResult CPlayerEnterZoneCheckNode::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool EnterZone = Data->IsPlayerEnterZone();

	if (EnterZone)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CPlayerEnterZoneCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CPlayerEnterZoneCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
