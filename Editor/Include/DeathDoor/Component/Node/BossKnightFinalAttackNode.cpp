#include "BossKnightFinalAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Scene/Scene.h"
#include "../KnightDataComponent.h"

CBossKnightFinalAttackNode::CBossKnightFinalAttackNode()
{
	SetTypeID(typeid(CBossKnightFinalAttackNode).hash_code());
}

CBossKnightFinalAttackNode::CBossKnightFinalAttackNode(const CBossKnightFinalAttackNode& Node) :
	CActionNode(Node)
{
}

CBossKnightFinalAttackNode::~CBossKnightFinalAttackNode()
{
}

void CBossKnightFinalAttackNode::Init()
{
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	std::string AnimName = "SlamContinueJump";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
	AnimInst->AddNotify(AnimName, "OnTracePlayer", 6,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnDisableTracePlayer", 14,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnHitBoxActive", 21, Data, &CKnightDataComponent::OnActiveMeleeAttackCollider);
	AnimInst->AddNotify(AnimName, "OnHitBoxDisable", 23, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnInActiveMeleeAttackCollider);
	AnimInst->SetEndFunction(AnimName, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEndAnimPostAttackDelayOn);

}

NodeResult CBossKnightFinalAttackNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		CResourceManager::GetInst()->SoundPlay("BossKnightJump");

		std::string AnimName = "SlamContinueJump";
		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation(AnimName);

		// 공격 카운트 리셋
		CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
		Data->ResetMeleeAttackCount();
	}

	return NodeResult::Node_True;
}

NodeResult CBossKnightFinalAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightFinalAttackNode::OnEnd(float DeltaTime)
{
	m_CallStart = false;

	return NodeResult::Node_True;
}
