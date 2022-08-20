#include "BossKnightWalkNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../KnightDataComponent.h"

CBossKnightWalkNode::CBossKnightWalkNode()
{
	SetTypeID(typeid(CBossKnightWalkNode).hash_code());
}

CBossKnightWalkNode::CBossKnightWalkNode(const CBossKnightWalkNode& Node)	:
	CActionNode(Node)
{
}

CBossKnightWalkNode::~CBossKnightWalkNode()
{
}

void CBossKnightWalkNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// 애니메이션 콜백 등록
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	AnimInst->AddNotify("Walk", "EnableWalk", 0, Data, &CKnightDataComponent::OnEnableLookAndMove);
	AnimInst->AddNotify("Walk", "DisableWalk", 5, Data, &CKnightDataComponent::OnDisableLookAndMove);
	AnimInst->AddNotify("Walk", "EnableWalk", 14, Data, &CKnightDataComponent::OnEnableLookAndMove);
	AnimInst->AddNotify("Walk", "DisableWalk", 24, Data, &CKnightDataComponent::OnDisableLookAndMove);
	AnimInst->AddNotify("Walk", "EnableWalk", 32, Data, &CKnightDataComponent::OnEnableLookAndMove);
	AnimInst->AddNotify("Walk", "DisableWalk", 41, Data, &CKnightDataComponent::OnDisableLookAndMove);
}

NodeResult CBossKnightWalkNode::OnStart(float DeltaTime)
{
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
	AnimInst->ChangeAnimation("Walk");

	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	Data->ResetMeleeAttackCount();

	return NodeResult::Node_True;
}

NodeResult CBossKnightWalkNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightWalkNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
