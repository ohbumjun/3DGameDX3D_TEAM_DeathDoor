
#include "MoveInputCheckNode.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"
#include "Input.h"

CMoveInputCheckNode::CMoveInputCheckNode()
{
	SetTypeID(typeid(CMoveInputCheckNode).hash_code());
}

CMoveInputCheckNode::CMoveInputCheckNode(const CMoveInputCheckNode& Node)
{
}

CMoveInputCheckNode::~CMoveInputCheckNode()
{
}

NodeResult CMoveInputCheckNode::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (DataComp->IsClimbingLadder())
	{
		return NodeResult::Node_False;
	}

	const keyState WState = CInput::GetInst()->FindKeyState('W');
	const keyState AState = CInput::GetInst()->FindKeyState('A');
	const keyState SState = CInput::GetInst()->FindKeyState('S');
	const keyState DState = CInput::GetInst()->FindKeyState('D');

	if (WState.State[KeyState_Down] || WState.State[KeyState_Push])
	{
		return NodeResult::Node_True;
	}

	if (AState.State[KeyState_Down] || AState.State[KeyState_Push])
	{
		return NodeResult::Node_True;
	}

	if (SState.State[KeyState_Down] || SState.State[KeyState_Push])
	{
		return NodeResult::Node_True;
	}

	if (DState.State[KeyState_Down] || DState.State[KeyState_Push])
	{
		return NodeResult::Node_True;
	}

	else
		return NodeResult::Node_False;
}

NodeResult CMoveInputCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMoveInputCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMoveInputCheckNode::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}
