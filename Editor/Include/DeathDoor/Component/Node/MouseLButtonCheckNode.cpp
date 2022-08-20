
#include "MouseLButtonCheckNode.h"
#include "Input.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CMouseLButtonCheckNode::CMouseLButtonCheckNode()	:
	m_FrameCount(0)
{
	SetTypeID(typeid(CMouseLButtonCheckNode).hash_code());
}

CMouseLButtonCheckNode::CMouseLButtonCheckNode(const CMouseLButtonCheckNode& Node)	:
	CConditionNode(Node)
{
}

CMouseLButtonCheckNode::~CMouseLButtonCheckNode()
{
}

NodeResult CMouseLButtonCheckNode::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (!DataComp->IsKeyStateQueueEmpty())
	{
		char Key = DataComp->GetFrontKeyState();

		if (Key == VK_LBUTTON)
		{
			DataComp->PopKeyState();
			return NodeResult::Node_True;
		}
	}


	return NodeResult::Node_False;
}

NodeResult CMouseLButtonCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMouseLButtonCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMouseLButtonCheckNode::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}