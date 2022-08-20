
#include "DeathNode.h"
#include "Component/AnimationMeshComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Component/BehaviorTree.h"	
#include "../ObjectDataComponent.h"
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"

CDeathNode::CDeathNode()
{
	SetTypeID(typeid(CDeathNode).hash_code());
}

CDeathNode::CDeathNode(const CDeathNode& Node)	:
	CActionNode(Node)
{
}

CDeathNode::~CDeathNode()
{
}

NodeResult CDeathNode::OnStart(float DeltaTime)
{
	if (m_Owner->GetCurrentNode() != this)
	{
		CObjectDataComponent* Data = dynamic_cast<CObjectDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

		m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
		std::string ObjectName = m_Object->GetName();

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		std::string SequenceName = "Death";

		bool IsDeathAnimExist = AnimInst->IsAnimExist(SequenceName);

		if (IsDeathAnimExist)
		{
			AnimInst->ChangeAnimation(SequenceName);
		}
		else
		{
			dynamic_cast<CMonsterDataComponent*>(Data)->OnDeadAnimEnd();
			AnimInst->Stop();
		}

		m_Owner->SetCurrentNode(this);
		Data->SetNoInterrupt(true);
	}

	return NodeResult::Node_True;
}

NodeResult CDeathNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CDeathNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CDeathNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}

