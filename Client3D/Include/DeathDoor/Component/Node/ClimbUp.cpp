
#include "ClimbUp.h"
#include "Component/AnimationMeshComponent.h"

CClimbUp::CClimbUp()
{
	SetTypeID(typeid(CClimbUp).hash_code());
}

CClimbUp::CClimbUp(const CClimbUp& Node) :
	CActionNode(Node)
{
}

CClimbUp::~CClimbUp()
{
}

NodeResult CClimbUp::OnStart(float DeltaTime)
{
	CAnimationSequenceInstance* Instance = m_AnimationMeshComp->GetAnimationInstance();
	Instance->ChangeAnimation("PlayerLadderUp");

	if (Instance->CheckCurrentAnimation("PlayerLadderUp"))
		Instance->GetCurrentAnimation()->SetPlayScale(1.f);

	m_Object->AddWorldPos(0.f, 3.f * DeltaTime, 0.f);

	return NodeResult::Node_True;
}

NodeResult CClimbUp::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CClimbUp::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}