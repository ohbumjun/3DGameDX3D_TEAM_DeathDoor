#include "BossBettyIdleAnimCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BossBettyDataComponent.h"

CBossBettyIdleAnimCheck::CBossBettyIdleAnimCheck()
{
	SetTypeID(typeid(CBossBettyIdleAnimCheck).hash_code());
}

CBossBettyIdleAnimCheck::CBossBettyIdleAnimCheck(const CBossBettyIdleAnimCheck& Node)
{
}

CBossBettyIdleAnimCheck::~CBossBettyIdleAnimCheck()
{
}

NodeResult CBossBettyIdleAnimCheck::OnStart(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsInitIdle = Data->IsInitIdle();

	if (IsInitIdle)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CBossBettyIdleAnimCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossBettyIdleAnimCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
