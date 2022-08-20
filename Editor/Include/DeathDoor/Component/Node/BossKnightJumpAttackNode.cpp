#include "BossKnightJumpAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Scene/Scene.h"
#include "../KnightDataComponent.h"

CBossKnightJumpAttackNode::CBossKnightJumpAttackNode()
{
	SetTypeID(typeid(CBossKnightJumpAttackNode).hash_code());
}

CBossKnightJumpAttackNode::CBossKnightJumpAttackNode(const CBossKnightJumpAttackNode& Node)	:
	CActionNode(Node)
{
}

CBossKnightJumpAttackNode::~CBossKnightJumpAttackNode()
{
}

void CBossKnightJumpAttackNode::Init()
{
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	std::string AnimName = "DashSlam";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
	AnimInst->AddNotify(AnimName, "IncreaseMeleeCount", 0, Data, &CKnightDataComponent::IncreaseMeleeAttackCount);
	AnimInst->AddNotify(AnimName, "OnWalkSoundPlay", 2, Data, &CKnightDataComponent::OnPlayWalkSound);
	AnimInst->AddNotify(AnimName, "OnWalkSoundPlay", 4, Data, &CKnightDataComponent::OnPlayWalkSound);
	AnimInst->AddNotify(AnimName, "OnWalkSoundPlay", 8, Data, &CKnightDataComponent::OnPlayWalkSound);
	AnimInst->AddNotify(AnimName, "OnWalkSoundPlay", 10, Data, &CKnightDataComponent::OnPlayWalkSound);
	AnimInst->AddNotify(AnimName, "OnStartJumpAttackMove", 0, Data, &CKnightDataComponent::OnStartJumpAttackMove);
	AnimInst->AddNotify(AnimName, "OnEndJumpAttackMove", 11, Data, &CKnightDataComponent::OnEndJumpAttackMove);
	AnimInst->AddNotify(AnimName, "OnEndJumpAttack", 19, Data, &CKnightDataComponent::OnEndJumpAttack);
	AnimInst->AddNotify(AnimName, "OnHitBoxActive", 20, Data, &CKnightDataComponent::OnActiveMeleeAttackCollider);
	AnimInst->AddNotify(AnimName, "OnHitBoxDisable", 22, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnInActiveMeleeAttackCollider);
	AnimInst->SetEndFunction(AnimName, Data, &CKnightDataComponent::OnEndAnimJudgeContinueAttack);
}

NodeResult CBossKnightJumpAttackNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		std::string AnimName = "DashSlam";
		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation(AnimName);
	}

	return NodeResult::Node_True;
}

NodeResult CBossKnightJumpAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightJumpAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
