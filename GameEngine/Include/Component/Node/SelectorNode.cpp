
#include "SelectorNode.h"
#include "../BehaviorTree.h"

CSelectorNode::CSelectorNode()
{
	SetTypeID(typeid(CSelectorNode).hash_code());
	m_NodeType = Node_Type::Selector;
}

CSelectorNode::CSelectorNode(const CSelectorNode& Node)	:
	CCompositeNode(Node)
{
}

CSelectorNode::~CSelectorNode()
{
}

NodeResult CSelectorNode::Invoke(float DeltaTime)
{
	auto iter = GetChildrenList().begin();
	auto iterEnd = GetChildrenList().end();

	for (; iter != iterEnd; ++iter)
	{
		NodeResult Result = (*iter)->Invoke(DeltaTime);

		// Selector는 하나의 자식이라도 true를 리턴하면 바로 자신도 true를 리턴
		if (Result == NodeResult::Node_True)
			return NodeResult::Node_True;
		else if (Result == NodeResult::Node_Running)
			return NodeResult::Node_True;

	}

	SetChildrenCallStart(false);
	return NodeResult::Node_False;
}

NodeResult CSelectorNode::OnStart(float DeltaTime)
{
	return NodeResult();
}

NodeResult CSelectorNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CSelectorNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

