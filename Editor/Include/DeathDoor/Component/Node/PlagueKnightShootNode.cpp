#include "PlagueKnightShootNode.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Component/BehaviorTree.h"	
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"

CPlagueKnightShootNode::CPlagueKnightShootNode()
{
	SetTypeID(typeid(CPlagueKnightShootNode).hash_code());
}

CPlagueKnightShootNode::CPlagueKnightShootNode(const CPlagueKnightShootNode& Node)	:
	CActionNode(Node)
{
}

CPlagueKnightShootNode::~CPlagueKnightShootNode()
{
}

void CPlagueKnightShootNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CPlagueKnightShootNode::OnStart(float DeltaTime)
{
	if (m_Owner->GetCurrentNode() != this)
	{
		m_Owner->SetCurrentNode(this);

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation("StartShoot");
	}

	return NodeResult::Node_True;
}

NodeResult CPlagueKnightShootNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CPlagueKnightShootNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
