#include "BossKnightContinueAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Scene/Scene.h"
#include "../KnightDataComponent.h"

CBossKnightContinueAttackNode::CBossKnightContinueAttackNode()
{
	SetTypeID(typeid(CBossKnightContinueAttackNode).hash_code());
}

CBossKnightContinueAttackNode::CBossKnightContinueAttackNode(const CBossKnightContinueAttackNode& Node)	:
	CActionNode(Node)
{
}

CBossKnightContinueAttackNode::~CBossKnightContinueAttackNode()
{
}

void CBossKnightContinueAttackNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// 애니메이션 콜백 등록
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	AnimInst->AddNotify("SlamContinueFirst", "IncreaseCount", 0, Data, &CKnightDataComponent::IncreaseMeleeAttackCount);
	AnimInst->AddNotify("SlamContinueFirst", "OnLookPlayer", 9, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->AddNotify("SlamContinueFirst", "OnDisableLookPlayer", 9, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
 	AnimInst->AddNotify("SlamContinueFirst", "HitBoxActive", 21, Data, &CKnightDataComponent::OnActiveMeleeAttackCollider);
 	AnimInst->AddNotify("SlamContinueFirst", "HitBoxDisable", 23, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnInActiveMeleeAttackCollider);
 	AnimInst->SetEndFunction("SlamContinueFirst", Data, &CKnightDataComponent::OnEndContinueAttack);

	AnimInst->AddNotify("Slam180CW", "IncreaseCount", 0, Data, &CKnightDataComponent::IncreaseMeleeAttackCount);
	AnimInst->AddNotify("Slam180CW", "OnLookPlayer", 5, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->AddNotify("Slam180CW", "OnDisableLookPlayer", 10, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
 	AnimInst->AddNotify("Slam180CW", "HitBoxActive", 16, Data, &CKnightDataComponent::OnActiveMeleeAttackCollider);
 	AnimInst->AddNotify("Slam180CW", "HitBoxDisable", 18, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnInActiveMeleeAttackCollider);
 	AnimInst->SetEndFunction("Slam180CW", Data, &CKnightDataComponent::OnEndContinueAttack);

	AnimInst->AddNotify("Slam180CCW", "IncreaseCount", 0, Data, &CKnightDataComponent::IncreaseMeleeAttackCount);
	AnimInst->AddNotify("Slam180CCW", "OnLookPlayer", 5, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->AddNotify("Slam180CCW", "OnDisableLookPlayer", 10, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
 	AnimInst->AddNotify("Slam180CCW", "HitBoxActive", 16, Data, &CKnightDataComponent::OnActiveMeleeAttackCollider);
 	AnimInst->AddNotify("Slam180CCW", "HitBoxDisable", 18, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnInActiveMeleeAttackCollider);
 	AnimInst->SetEndFunction("Slam180CCW", Data, &CKnightDataComponent::OnEndContinueAttack);

	m_RotatePerSec = Data->GetRotateSpeed();
}

NodeResult CBossKnightContinueAttackNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		// Melee Attack Count 증가
		CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

		// 공격 회전 방향에 따라 다른 애니메이션 재생
		Knight_Attack_Rot_Type AttackType = Data->GetAttackRotationType();
		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

		switch (AttackType)
		{
		case Knight_Attack_Rot_Type::Front:
			AnimInst->ChangeAnimation("SlamContinueFirst");
			break;
		case Knight_Attack_Rot_Type::CW:
			AnimInst->ChangeAnimation("Slam180CW");
			break;
		case Knight_Attack_Rot_Type::CCW:
			AnimInst->ChangeAnimation("Slam180CCW");
			break;
		}
	}

	return NodeResult::Node_True;
}

NodeResult CBossKnightContinueAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightContinueAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
