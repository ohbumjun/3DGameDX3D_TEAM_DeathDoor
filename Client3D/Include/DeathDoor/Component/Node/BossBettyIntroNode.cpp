#include "BossBettyIntroNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Collision/Collision.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../BossBettyDataComponent.h"
#include "BossBettyChangeAttackDirNode.h"

CBossBettyIntroNode::CBossBettyIntroNode()
{
	SetTypeID(typeid(CBossBettyIntroNode).hash_code());
}

CBossBettyIntroNode::CBossBettyIntroNode(const CBossBettyIntroNode& Node)
{
}

CBossBettyIntroNode::~CBossBettyIntroNode()
{
}

void CBossBettyIntroNode::Init()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	// 1) Slash Left
	std::string AnimName = "Intro";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->AddNotify(AnimName, "IntroSound", 0,
		Data, &CBossBettyDataComponent::OnBossBettyIntroStartSound);

	// >> End
	AnimInst->AddNotify(AnimName, "SetCurrentNodeNull", 151,
		Data, &CBossBettyDataComponent::OnBossBettySetCurrentNodeNullPtr);

	AnimInst->SetEndFunction(AnimName,
		Data, &CBossBettyDataComponent::OnBossBettyEndIntroAndStartGame);
}

NodeResult CBossBettyIntroNode::OnStart(float DeltaTime)
{
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// Spin 중이라면, Animation 을 바꿔서는 안된다.
	AnimInst->ChangeAnimation("Intro");

	// m_Owner->SetCurrentNode(this);

	return NodeResult::Node_True;
}

NodeResult CBossBettyIntroNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossBettyIntroNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
