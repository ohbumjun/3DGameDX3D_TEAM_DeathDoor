#include "DodgerFirstAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../DodgerDataComponent.h"
#include "Scene/Scene.h"
#include "Component/AnimationMeshComponent.h"

CDodgerFirstAttackNode::CDodgerFirstAttackNode()
{
	SetTypeID(typeid(CDodgerFirstAttackNode).hash_code());
}

CDodgerFirstAttackNode::CDodgerFirstAttackNode(const CDodgerFirstAttackNode& Node)	:
	CActionNode(Node)
{	
}

CDodgerFirstAttackNode::~CDodgerFirstAttackNode()
{
}

void CDodgerFirstAttackNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CDodgerFirstAttackNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

		AnimInst->ChangeAnimation("AttackReady");
	}

	return NodeResult::Node_True;
}

NodeResult CDodgerFirstAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CDodgerFirstAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
