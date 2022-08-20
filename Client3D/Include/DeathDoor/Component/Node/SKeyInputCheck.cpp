
#include "SKeyInputCheck.h"
#include "Input.h"

CSKeyInputCheck::CSKeyInputCheck()	:
	m_FrameCount(0)
{
	SetTypeID(typeid(CSKeyInputCheck).hash_code());
}

CSKeyInputCheck::CSKeyInputCheck(const CSKeyInputCheck& Node)	:
	CConditionNode(Node)
{
}

CSKeyInputCheck::~CSKeyInputCheck()
{
}

NodeResult CSKeyInputCheck::OnStart(float DeltaTime)
{
	const keyState SState = CInput::GetInst()->FindKeyState('S');

	++m_FrameCount;

	if (SState.State[KeyState_Down] || SState.State[KeyState_Push])
	{
		m_FrameCount = 0;
		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CSKeyInputCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CSKeyInputCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
