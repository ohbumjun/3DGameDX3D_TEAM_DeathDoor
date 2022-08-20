#include "BossBettyIntroAnimCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BossBettyDataComponent.h"

CBossBettyIntroAnimCheck::CBossBettyIntroAnimCheck()
{
	SetTypeID(typeid(CBossBettyIntroAnimCheck).hash_code());
}

CBossBettyIntroAnimCheck::CBossBettyIntroAnimCheck(const CBossBettyIntroAnimCheck& Node)
{
}

CBossBettyIntroAnimCheck::~CBossBettyIntroAnimCheck()
{
}

NodeResult CBossBettyIntroAnimCheck::OnStart(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsIntroAnimation = Data->IsIntroAnimation();

	if (IsIntroAnimation)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CBossBettyIntroAnimCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossBettyIntroAnimCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
