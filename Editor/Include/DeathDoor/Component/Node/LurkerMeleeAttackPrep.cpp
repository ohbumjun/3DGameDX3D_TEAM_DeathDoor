
#include "LurkerMeleeAttackPrep.h"
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"

CLurkerMeleeAttackPrep::CLurkerMeleeAttackPrep()
{
	SetTypeID(typeid(CLurkerMeleeAttackPrep).hash_code());
}

CLurkerMeleeAttackPrep::CLurkerMeleeAttackPrep(const CLurkerMeleeAttackPrep& Node) :
	CActionNode(Node)
{
}

CLurkerMeleeAttackPrep::~CLurkerMeleeAttackPrep()
{
}

NodeResult CLurkerMeleeAttackPrep::OnStart(float DeltaTime)
{
	if (m_Owner->GetCurrentNode() != this)
	{
		m_Owner->SetCurrentNode(this);

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

		AnimInst->ChangeAnimation("MeleeAttackprep");
	}

	return NodeResult::Node_True;
}

NodeResult CLurkerMeleeAttackPrep::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CLurkerMeleeAttackPrep::OnEnd(float DeltaTime)
{
	return NodeResult();
}