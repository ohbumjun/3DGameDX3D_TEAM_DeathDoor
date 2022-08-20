#include "DodgerDashNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../DodgerDataComponent.h"
#include "Scene/Scene.h"
#include "Component/AnimationMeshComponent.h"

CDodgerDashNode::CDodgerDashNode()
{
	SetTypeID(typeid(CDodgerDashNode).hash_code());
}

CDodgerDashNode::CDodgerDashNode(const CDodgerDashNode& Node)	:
	CActionNode(Node)
{
}

CDodgerDashNode::~CDodgerDashNode()
{
}

void CDodgerDashNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CDodgerDashNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

		AnimInst->ChangeAnimation("Dash");
	}

	return NodeResult::Node_True;
}

NodeResult CDodgerDashNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CDodgerDashNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
