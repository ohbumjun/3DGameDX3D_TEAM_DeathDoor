
#include "WKeyInputCheck.h"
#include "Input.h"

CWKeyInputCheck::CWKeyInputCheck()	:
	m_FrameCount(0)
{
	SetTypeID(typeid(CWKeyInputCheck).hash_code());
}

CWKeyInputCheck::CWKeyInputCheck(const CWKeyInputCheck& Node)	:
	CConditionNode(Node)
{
}

CWKeyInputCheck::~CWKeyInputCheck()
{
}

NodeResult CWKeyInputCheck::OnStart(float DeltaTime)
{
	const keyState WState = CInput::GetInst()->FindKeyState('W');

	++m_FrameCount;

	if (WState.State[KeyState_Down] || WState.State[KeyState_Push])
	{
		m_FrameCount = 0;
		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CWKeyInputCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CWKeyInputCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
