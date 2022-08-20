
#include "NoInterruptNode.h"
#include "Input.h"
#include "Component/BehaviorTree.h"
#include "../ObjectDataComponent.h"
#include "../GameStateComponent.h"

CNoInterruptNode::CNoInterruptNode() 
{
	SetTypeID(typeid(CNoInterruptNode).hash_code());
}

CNoInterruptNode::CNoInterruptNode(const CNoInterruptNode& Node)	:
	CConditionNode(Node)
{
}

CNoInterruptNode::~CNoInterruptNode()
{
}

NodeResult CNoInterruptNode::OnStart(float DeltaTime)
{
	CObjectDataComponent* Data = dynamic_cast<CObjectDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (!Data)
		return NodeResult::Node_False;

	if (Data->IsNoInterrupt())
	{
		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CNoInterruptNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CNoInterruptNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CNoInterruptNode::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}