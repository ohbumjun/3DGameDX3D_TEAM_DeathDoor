
#include "MouseRButtonUpCheckNode.h"
#include "Input.h"

CMouseRButtonUpCheckNode::CMouseRButtonUpCheckNode()
{
	SetTypeID(typeid(CMouseRButtonUpCheckNode).hash_code());
}

CMouseRButtonUpCheckNode::CMouseRButtonUpCheckNode(const CMouseRButtonUpCheckNode& Node) :
	CConditionNode(Node)
{
}

CMouseRButtonUpCheckNode::~CMouseRButtonUpCheckNode()
{
}

NodeResult CMouseRButtonUpCheckNode::OnStart(float DeltaTime)
{
	bool UpRButton = CInput::GetInst()->GetMouseRButtonUp();

	if (UpRButton)
		return NodeResult::Node_True;

	else
		return NodeResult::Node_False;
}

NodeResult CMouseRButtonUpCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMouseRButtonUpCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMouseRButtonUpCheckNode::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}
