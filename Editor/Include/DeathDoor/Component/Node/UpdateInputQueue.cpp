
#include "UpdateInputQueue.h"
#include "Input.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CUpdateInputQueue::CUpdateInputQueue()	:
	m_FrameCount(0)
{
	SetTypeID(typeid(CUpdateInputQueue).hash_code());
}

CUpdateInputQueue::CUpdateInputQueue(const CUpdateInputQueue& Node)	:
	CConditionNode(Node)
{
}

CUpdateInputQueue::~CUpdateInputQueue()
{
}

NodeResult CUpdateInputQueue::OnStart(float DeltaTime)
{
	m_DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	++m_FrameCount;
	bool KeyInput = CInput::GetInst()->GetMouseLButtonClick();

	if (m_FrameCount > 10 && KeyInput)
	{
		m_DataComp->PushKeyState(VK_LBUTTON);

		m_FrameCount = 0;
	}

	//Click = CInput::GetInst()->GetMouseRButtonClick();

	//if (m_FrameCount > 15 && Click)
	//{
	//	m_DataComp->PushKeyState(VK_RBUTTON);
	//	m_FrameCount = 0;
	//}

	// 이 노드의 부모가 Selector이므로 키 입력이 있으면 false를 리턴해서 Sibling Node로 가도록 하고, 키 입력이 없으면 가지 않도록 해서 아무런 동작 체크도 하지 않게 한다
	if (KeyInput)
		return NodeResult::Node_False;
	else
		return NodeResult::Node_True;
}

NodeResult CUpdateInputQueue::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CUpdateInputQueue::OnEnd(float DeltaTime)
{
	return NodeResult();
}
