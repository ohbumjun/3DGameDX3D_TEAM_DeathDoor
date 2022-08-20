
#include "LurkerHop.h"
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"

CLurkerHop::CLurkerHop()
{
	SetTypeID(typeid(CLurkerHop).hash_code());
}

CLurkerHop::CLurkerHop(const CLurkerHop& Node) :
	CActionNode(Node)
{
}

CLurkerHop::~CLurkerHop()
{
}

NodeResult CLurkerHop::OnStart(float DeltaTime)
{

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->ChangeAnimation("Hop");


	return NodeResult::Node_True;
}

NodeResult CLurkerHop::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CLurkerHop::OnEnd(float DeltaTime)
{
	return NodeResult();
}