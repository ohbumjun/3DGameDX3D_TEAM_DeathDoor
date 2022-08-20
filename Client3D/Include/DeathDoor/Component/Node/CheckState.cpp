
#include "CheckState.h"
#include "Component/StateComponent.h"

CCheckState::CCheckState()
{
}

CCheckState::CCheckState(const CCheckState& Node)	:
	CConditionNode(Node)
{
}

CCheckState::~CCheckState()
{
}

NodeResult CCheckState::OnStart(float DeltaTime)
{
	CStateComponent* State = m_Object->FindComponentFromType<CStateComponent>();

	if (!State)
		return NodeResult::Node_False;

	size_t Count = State->GetStateListSize();

	if (Count == 0)
		return NodeResult::Node_False;

	else
		return NodeResult::Node_True;
}

NodeResult CCheckState::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CCheckState::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CCheckState::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}
