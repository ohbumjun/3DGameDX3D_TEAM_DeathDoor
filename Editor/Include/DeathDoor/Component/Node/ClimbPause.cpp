
#include "ClimbPause.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Input.h"

CClimbPause::CClimbPause()
{
	SetTypeID(typeid(CClimbPause).hash_code());
}

CClimbPause::CClimbPause(const CClimbPause& Node) :
	CActionNode(Node)
{
}

CClimbPause::~CClimbPause()
{
}

NodeResult CClimbPause::OnStart(float DeltaTime)
{
	CAnimationSequenceInstance* Instance = m_AnimationMeshComp->GetAnimationInstance();
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	const keyState SState = CInput::GetInst()->FindKeyState('S');
	const keyState WState = CInput::GetInst()->FindKeyState('W');

	if (SState.State[KeyState_Up] || WState.State[KeyState_Up])
	{
		if(DataComp->IsClimbingLadder())
			Instance->GetCurrentAnimation()->SetPlayScale(0.f);
	}


	return NodeResult::Node_True;
}

NodeResult CClimbPause::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CClimbPause::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}