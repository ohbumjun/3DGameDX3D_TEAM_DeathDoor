#include "BossBettyThrowAttackCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BossBettyDataComponent.h"

CBossBettyThrowAttackCheck::CBossBettyThrowAttackCheck()
{
	SetTypeID(typeid(CBossBettyThrowAttackCheck).hash_code());
}

CBossBettyThrowAttackCheck::CBossBettyThrowAttackCheck(const CBossBettyThrowAttackCheck& Node)
{
}

CBossBettyThrowAttackCheck::~CBossBettyThrowAttackCheck()
{
}

NodeResult CBossBettyThrowAttackCheck::OnStart(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool FarThrowAttackEnable = Data->IsThrowFarAttackEnable();

	if (FarThrowAttackEnable)
		return NodeResult::Node_True;
	else
		return NodeResult::Node_False;
}

NodeResult CBossBettyThrowAttackCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossBettyThrowAttackCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
