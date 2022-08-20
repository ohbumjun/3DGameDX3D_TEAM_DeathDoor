
#include "HitCheckNode.h"
#include "../ObjectDataComponent.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CHitCheckNode::CHitCheckNode()
{
	SetTypeID(typeid(CHitCheckNode).hash_code());
}

CHitCheckNode::CHitCheckNode(const CHitCheckNode& Node)	:
	CConditionNode(Node)
{
}

CHitCheckNode::~CHitCheckNode()
{
}

NodeResult CHitCheckNode::OnStart(float DeltaTime)
{
	CObjectDataComponent* Data = dynamic_cast<CObjectDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (!Data)
		return NodeResult::Node_False;

	if (Data->GetIsHit())
	{
		Data->SetNoInterrupt(true);
		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CHitCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CHitCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
