
#include "LurkerHopStartCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../LurkerDataComponent.h"

CLurkerHopStartCheck::CLurkerHopStartCheck()
{
	SetTypeID(typeid(CLurkerHopStartCheck).hash_code());
}

CLurkerHopStartCheck::CLurkerHopStartCheck(const CLurkerHopStartCheck& Node) :
	CConditionNode(Node)
{
}

CLurkerHopStartCheck::~CLurkerHopStartCheck()
{
}

NodeResult CLurkerHopStartCheck::OnStart(float DeltaTime)
{
	CLurkerDataComponent* Data = dynamic_cast<CLurkerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool HopStart = Data->IsHopStart();

	if (HopStart)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CLurkerHopStartCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CLurkerHopStartCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
