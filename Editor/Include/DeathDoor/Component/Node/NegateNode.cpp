
#include "NegateNode.h"

CNegateNode::CNegateNode()
{
	m_NodeType = Node_Type::Decorator;
	SetTypeID(typeid(CNegateNode).hash_code());
}

CNegateNode::CNegateNode(const CNegateNode& Node)
{
}

CNegateNode::~CNegateNode()
{
}

NodeResult CNegateNode::OnStart(float DeltaTime)
{
	return NodeResult();
}

NodeResult CNegateNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CNegateNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CNegateNode::Invoke(float DeltaTime)
{
	NodeResult Result = m_Child->Invoke(DeltaTime);

	if (Result == NodeResult::Node_True)
		return NodeResult::Node_False;
	else if (Result == NodeResult::Node_False)
		return NodeResult::Node_True;

	return Result;
}