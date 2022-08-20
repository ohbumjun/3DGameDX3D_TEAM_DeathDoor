
#include "ConditionNode.h"

CConditionNode::CConditionNode()
{
	m_NodeType = Node_Type::Condition;
}

CConditionNode::CConditionNode(const CConditionNode& Node)	:
	CNode(Node)
{
}

CConditionNode::~CConditionNode()
{
}

NodeResult CConditionNode::Invoke(float DeltaTime)
{
	NodeResult Result =	OnStart(DeltaTime);

	if (Result == NodeResult::Node_False)
		return NodeResult::Node_False;

	else if (Result == NodeResult::Node_True)
		return NodeResult::Node_True;

	else if (Result == NodeResult::Node_Running)
		return NodeResult::Node_Running;

	return NodeResult::Node_False;
}
