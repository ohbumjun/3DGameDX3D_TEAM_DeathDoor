#include "SporeBoomerShootNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../SporeBoomerDataComponent.h"
#include "Scene/Scene.h"
#include "Component/AnimationMeshComponent.h"

CSporeBoomerShootNode::CSporeBoomerShootNode()
{
	SetTypeID(typeid(CSporeBoomerShootNode).hash_code());
}

CSporeBoomerShootNode::CSporeBoomerShootNode(const CSporeBoomerShootNode& Node)	:
	CActionNode(Node)
{
}

CSporeBoomerShootNode::~CSporeBoomerShootNode()
{
}

void CSporeBoomerShootNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CSporeBoomerShootNode::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		std::string AnimName = "Fire";
		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation(AnimName);
	}

	return NodeResult::Node_True;
}

NodeResult CSporeBoomerShootNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CSporeBoomerShootNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
