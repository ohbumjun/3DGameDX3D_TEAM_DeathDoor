
#include "ActionNode.h"
#include "../BehaviorTree.h"

CActionNode::CActionNode()
{
	SetTypeID(typeid(CActionNode).hash_code());
	m_NodeType = Node_Type::Action;
}

CActionNode::CActionNode(const CActionNode& Node)	:
	CNode(Node)
{
}

CActionNode::~CActionNode()
{
}

NodeResult CActionNode::Invoke(float DeltaTime)
{
	if(!m_CallStart)
		OnStart(DeltaTime);	

	NodeResult Result = OnUpdate(DeltaTime);

	if (m_IsEnd)
		OnEnd(DeltaTime);

	//if (Result == NodeResult::Node_Running)
	//{
	//	m_Owner->SetCurrentNode(this);
	//}

	return Result;

}

