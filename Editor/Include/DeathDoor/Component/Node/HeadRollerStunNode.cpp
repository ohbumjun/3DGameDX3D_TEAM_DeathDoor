#include "HeadRollerStunNode.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterNavAgent.h"
#include "../HeadRollerDataComponent.h"
#include "Component/AnimationMeshComponent.h"

CHeadRollerStunNode::CHeadRollerStunNode()
{
	SetTypeID(typeid(CHeadRollerStunNode).hash_code());
}

CHeadRollerStunNode::CHeadRollerStunNode(const CHeadRollerStunNode& Node)	:
	CActionNode(Node)
{
}

CHeadRollerStunNode::~CHeadRollerStunNode()
{
}

void CHeadRollerStunNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CHeadRollerStunNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		CHeadRollerDataComponent* Data = (CHeadRollerDataComponent*)(((CGameStateComponent*)m_Owner->GetOwner())->GetData());
		Data->OnRollEnd();

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation("Stun");
	}

	return NodeResult::Node_True;
}

NodeResult CHeadRollerStunNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CHeadRollerStunNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
