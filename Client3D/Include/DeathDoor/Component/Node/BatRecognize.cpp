
#include "BatRecognize.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/BehaviorTree.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BatDataComponent.h"

CBatRecognize::CBatRecognize()
{
	SetTypeID(typeid(CBatRecognize).hash_code());
}

CBatRecognize::CBatRecognize(const CBatRecognize& Node) :
	CActionNode(Node)
{
}

CBatRecognize::~CBatRecognize()
{
}

NodeResult CBatRecognize::OnStart(float DeltaTime)
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	if (m_AnimationMeshComp)
	{
		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Recognize");
	}

	CBatDataComponent* Data = dynamic_cast<CBatDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	Data->SetRecognizeStart(true);

	return NodeResult::Node_True;
}

NodeResult CBatRecognize::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBatRecognize::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
