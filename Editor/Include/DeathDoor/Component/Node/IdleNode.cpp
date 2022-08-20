
#include "IdleNode.h"
#include "Component/AnimationMeshComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Component/BehaviorTree.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CIdleNode::CIdleNode()
{
	SetTypeID(typeid(CIdleNode).hash_code());
}

CIdleNode::CIdleNode(const CIdleNode& Node)	:
	CActionNode(Node)
{
}

CIdleNode::~CIdleNode()
{
}

NodeResult CIdleNode::OnStart(float DeltaTime)
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	if (m_AnimationMeshComp)
	{
		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Idle");
	}

	if (m_Object->GetObjectType() == Object_Type::Player)
	{
		CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

		CAnimationMeshComponent* Sword = Data->GetSword();
		if(Sword)
			Sword->GetAnimationInstance()->ChangeAnimation("Idle");
	}


	return NodeResult::Node_True;
}

NodeResult CIdleNode::OnUpdate(float DeltaTime)
{
	m_IsEnd = true;

	return NodeResult::Node_True;
}

NodeResult CIdleNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CIdleNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}


