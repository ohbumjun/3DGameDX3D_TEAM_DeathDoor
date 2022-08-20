#include "HeadRollerRollNode.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterNavAgent.h"
#include "../HeadRollerDataComponent.h"
#include "Component/AnimationMeshComponent.h"

CHeadRollerRollNode::CHeadRollerRollNode()
{
	SetTypeID(typeid(CHeadRollerRollNode).hash_code());
}

CHeadRollerRollNode::CHeadRollerRollNode(const CHeadRollerRollNode& Node)	:
	CActionNode(Node)
{
}

CHeadRollerRollNode::~CHeadRollerRollNode()
{
}

void CHeadRollerRollNode::Init()
{
	m_Data = dynamic_cast<CHeadRollerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
	m_Nav = m_Data->GetMonsterNavAgent();
}

NodeResult CHeadRollerRollNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation("RollStart");
	}

	return NodeResult::Node_True;
}

NodeResult CHeadRollerRollNode::OnUpdate(float DeltaTime)
{
	bool In = m_Nav->IsInNavMesh();

	if (!In)
	{
		m_Data->OnRollEnd();
		return NodeResult::Node_True;
	}
	return NodeResult::Node_True;
}

NodeResult CHeadRollerRollNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
