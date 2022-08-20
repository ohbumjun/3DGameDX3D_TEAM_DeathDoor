
#include "ClimbDown.h"
#include "Component/AnimationMeshComponent.h"

CClimbDown::CClimbDown()
{
	SetTypeID(typeid(CClimbDown).hash_code());
}

CClimbDown::CClimbDown(const CClimbDown& Node) :
	CActionNode(Node)
{
}

CClimbDown::~CClimbDown()
{
}

NodeResult CClimbDown::OnStart(float DeltaTime)
{

	return NodeResult::Node_True;
}

NodeResult CClimbDown::OnUpdate(float DeltaTime)
{
	CAnimationSequenceInstance* Instance = m_AnimationMeshComp->GetAnimationInstance();
	Instance->ChangeAnimation("PlayerLadderDown");

	if (Instance->CheckCurrentAnimation("PlayerLadderDown"))
		Instance->GetCurrentAnimation()->SetPlayScale(1.f);


	m_Object->AddWorldPos(0.f, -3.f * DeltaTime, 0.f);

	return NodeResult::Node_True;
}

NodeResult CClimbDown::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}