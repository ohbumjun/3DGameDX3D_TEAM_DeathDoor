
#include "HitBackNode.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Scene/Scene.h"

CHitBackNode::CHitBackNode()
{
	SetTypeID(typeid(CHitBackNode).hash_code());
}

CHitBackNode::CHitBackNode(const CHitBackNode& Node)	:
	CActionNode(Node)
{
}

CHitBackNode::~CHitBackNode()
{
}

NodeResult CHitBackNode::OnStart(float DeltaTime)
{
	CPlayerDataComponent* Comp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (!Comp)
		return NodeResult::Node_False;


	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("PlayerHitBack");
	Comp->GetSword()->GetAnimationInstance()->ChangeAnimation("PlayerHitBack");
	Comp->SetFalseOnSlash();

	m_Object->GetScene()->GetCameraManager()->GetCurrentCamera()->Shake(0.2f, 0.2f);

	Comp->SetNoInterrupt(true);

	return NodeResult::Node_True;
}

NodeResult CHitBackNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CHitBackNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
