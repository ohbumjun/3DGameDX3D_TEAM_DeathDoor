#include "BossKnightIdleNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Scene/Scene.h"
#include "../KnightDataComponent.h"

CBossKnightIdleNode::CBossKnightIdleNode()
{
	SetTypeID(typeid(CBossKnightIdleNode).hash_code());
}

CBossKnightIdleNode::CBossKnightIdleNode(const CBossKnightIdleNode& Node)	:
	CActionNode(Node)
{
}

CBossKnightIdleNode::~CBossKnightIdleNode()
{
}

void CBossKnightIdleNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CBossKnightIdleNode::OnStart(float DeltaTime)
{
	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("IdleEngaged");

	return NodeResult::Node_True;
}

NodeResult CBossKnightIdleNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightIdleNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
