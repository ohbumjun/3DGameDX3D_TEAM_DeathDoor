#include "DodgerPostAttackNode.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Navigation3DManager.h"
#include "Component/BehaviorTree.h"	
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "../MonsterNavAgent.h"

CDodgerPostAttackNode::CDodgerPostAttackNode()
{
	SetTypeID(typeid(CDodgerPostAttackNode).hash_code());
}

CDodgerPostAttackNode::CDodgerPostAttackNode(const CDodgerPostAttackNode& Node)	:
	CActionNode(Node)
{
}

CDodgerPostAttackNode::~CDodgerPostAttackNode()
{
}

void CDodgerPostAttackNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CDodgerPostAttackNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("AttackEnd");
	}

	return NodeResult::Node_True;
}

NodeResult CDodgerPostAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CDodgerPostAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
