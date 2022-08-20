#include "DodgerSecondAttackNode.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Navigation3DManager.h"
#include "Component/BehaviorTree.h"	
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "../MonsterNavAgent.h"

CDodgerSecondAttackNode::CDodgerSecondAttackNode()
{
	SetTypeID(typeid(CDodgerSecondAttackNode).hash_code());
}

CDodgerSecondAttackNode::CDodgerSecondAttackNode(const CDodgerSecondAttackNode& Node)	:
	CActionNode(Node)
{
}

CDodgerSecondAttackNode::~CDodgerSecondAttackNode()
{
}

void CDodgerSecondAttackNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CDodgerSecondAttackNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("AttackReady2");
	}

	return NodeResult::Node_True;
}

NodeResult CDodgerSecondAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CDodgerSecondAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
