#include "MeleeAttackNode.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Component/BehaviorTree.h"	
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"

CMeleeAttackNode::CMeleeAttackNode()
{
	SetTypeID(typeid(CMeleeAttackNode).hash_code());
}

CMeleeAttackNode::CMeleeAttackNode(const CMeleeAttackNode& Node)	:
	CActionNode(Node)
{
}

CMeleeAttackNode::~CMeleeAttackNode()
{
}

void CMeleeAttackNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CMeleeAttackNode::OnStart(float DeltaTime)
{
	if (m_Owner->GetCurrentNode() != this)
	{
		m_Owner->SetCurrentNode(this);

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

		AnimInst->ChangeAnimation("MeleeAttack");
	}

	//CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	//Data->SetPahtFindExecute(true);

	return NodeResult::Node_True;
}

NodeResult CMeleeAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CMeleeAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
