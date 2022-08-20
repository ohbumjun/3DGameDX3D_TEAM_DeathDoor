
#include "RollInputCheck.h"
#include "Input.h"
#include "../../DDFlag.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"

CRollInputCheck::CRollInputCheck()	:
	m_FrameCount(0)
{
	SetTypeID(typeid(CRollInputCheck).hash_code());
}

CRollInputCheck::CRollInputCheck(const CRollInputCheck& Node)	:
	CConditionNode(Node)
{
}

CRollInputCheck::~CRollInputCheck()
{
}

NodeResult CRollInputCheck::OnStart(float DeltaTime)
{
	const keyState SpaceState = CInput::GetInst()->FindKeyState(VK_SPACE);
	const keyState WState = CInput::GetInst()->FindKeyState('W');
	const keyState AState = CInput::GetInst()->FindKeyState('A');
	const keyState SState = CInput::GetInst()->FindKeyState('S');
	const keyState DState = CInput::GetInst()->FindKeyState('D');

	DDPlayerRollDirection Dir = DDPlayerRollDirection::None;

	if (AState.State[KeyState_Down] || AState.State[KeyState_Push])
	{
		Dir = static_cast<DDPlayerRollDirection>((int)Dir | (int)DDPlayerRollDirection::Left);
	}

	if (DState.State[KeyState_Down] || DState.State[KeyState_Push])
	{
		Dir = static_cast<DDPlayerRollDirection>((int)Dir | (int)DDPlayerRollDirection::Right);
	}

	if (WState.State[KeyState_Down] || WState.State[KeyState_Push])
	{
		Dir = static_cast<DDPlayerRollDirection>((int)Dir | (int)DDPlayerRollDirection::Up);
	}

	if (SState.State[KeyState_Down] || SState.State[KeyState_Push])
	{
		Dir = static_cast<DDPlayerRollDirection>((int)Dir | (int)DDPlayerRollDirection::Down);
	}

	++m_FrameCount;

	if (m_FrameCount >= 10 && SpaceState.State[KeyState_Down] || SpaceState.State[KeyState_Push])
	{
		bool IsPlaying = m_AnimationMeshComp->GetAnimationInstance()->CheckCurrentAnimation("PlayerRoll");

		if (IsPlaying)
		{
			return NodeResult::Node_False;
		}

		m_FrameCount = 0;
		CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

		DataComp->SetRollDirection(Dir);

		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}

}

NodeResult CRollInputCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CRollInputCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
