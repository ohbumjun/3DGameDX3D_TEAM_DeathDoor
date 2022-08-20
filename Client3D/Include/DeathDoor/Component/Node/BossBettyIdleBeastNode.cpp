#include "BossBettyIdleBeastNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Collision/Collision.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../BossBettyDataComponent.h"


CBossBettyIdleBeastNode::CBossBettyIdleBeastNode()
{
	SetTypeID(typeid(CBossBettyIdleBeastNode).hash_code());
}

CBossBettyIdleBeastNode::CBossBettyIdleBeastNode(const CBossBettyIdleBeastNode& Node)
{
}

CBossBettyIdleBeastNode::~CBossBettyIdleBeastNode()
{
}

NodeResult CBossBettyIdleBeastNode::OnStart(float DeltaTime)
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	if (m_AnimationMeshComp)
	{
		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("IdleBeast");

		m_Owner->SetCurrentNode(this);
	}

	CObjectDataComponent* Data = dynamic_cast<CObjectDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	return NodeResult::Node_True;
}

NodeResult CBossBettyIdleBeastNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossBettyIdleBeastNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

