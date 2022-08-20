
#include "MouseRButtonCheckNode.h"
#include "Input.h"

CMouseRButtonCheckNode::CMouseRButtonCheckNode() :
	m_FrameCount(0)
{
	SetTypeID(typeid(CMouseRButtonCheckNode).hash_code());
}

CMouseRButtonCheckNode::CMouseRButtonCheckNode(const CMouseRButtonCheckNode& Node) :
	CConditionNode(Node)
{
}

CMouseRButtonCheckNode::~CMouseRButtonCheckNode()
{
}

NodeResult CMouseRButtonCheckNode::OnStart(float DeltaTime)
{
	++m_FrameCount;
	bool Click = CInput::GetInst()->GetMouseRButtonClick();

	if (m_FrameCount > 20 && Click)
	{
		m_FrameCount = 0;
		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CMouseRButtonCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMouseRButtonCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMouseRButtonCheckNode::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}
