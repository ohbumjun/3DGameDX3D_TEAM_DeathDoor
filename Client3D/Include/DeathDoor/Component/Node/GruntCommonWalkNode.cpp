#include "GruntCommonWalkNode.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../HeadRollerDataComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../GruntCommonDataComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

CGruntCommonWalkNode::CGruntCommonWalkNode():
	m_RotateChangeTimeMax(5.f)
{
	SetTypeID(typeid(CGruntCommonWalkNode).hash_code());
}

CGruntCommonWalkNode::CGruntCommonWalkNode(const CGruntCommonWalkNode& Node)
{
}

CGruntCommonWalkNode::~CGruntCommonWalkNode()
{
}

void CGruntCommonWalkNode::Init()
{
	CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	std::string AnimName = "WalkAgressive";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnInActiveMeleeAttackCollider);
	AnimInst->AddNotify(AnimName, "DisableLook", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "EnableZMove", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableMoveZ);
}

NodeResult CGruntCommonWalkNode::OnStart(float DeltaTime)
{
	m_Owner->GetAnimationMeshComp()->GetAnimationInstance()->ChangeAnimation("WalkAgressive");

	return NodeResult::Node_True;
}

NodeResult CGruntCommonWalkNode::OnUpdate(float DeltaTime)
{
	// 일정 간격마다 이동 방향을 바꾼다.
	m_RotateChangeTime += DeltaTime;

	if (m_RotateChangeTime >= m_RotateChangeTimeMax)
	{
		m_RotateChangeTime = 0.f;

		CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

		Data->SetRotateRandom();
	}

	return NodeResult::Node_True;
}

NodeResult CGruntCommonWalkNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
