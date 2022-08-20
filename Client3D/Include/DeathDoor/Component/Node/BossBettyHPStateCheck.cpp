#include "BossBettyHPStateCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BossBettyDataComponent.h"

CBossBettyHPStateCheck::CBossBettyHPStateCheck()
{
	SetTypeID(typeid(CBossBettyHPStateCheck).hash_code());
}

CBossBettyHPStateCheck::CBossBettyHPStateCheck(const CBossBettyHPStateCheck& Node)
{
}

CBossBettyHPStateCheck::~CBossBettyHPStateCheck()
{
}

NodeResult CBossBettyHPStateCheck::OnStart(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	BossBettyHPState BettyHPState = Data->GetBettyHPState();

	if (BettyHPState == BossBettyHPState::Below30 ||
		BettyHPState == BossBettyHPState::Below60)
	{
		// ChangeAttackDirNode 에서 끝나고 나서, 강제로 HPStateCheck의 Node의 Parent, Parent Node 를 
		// CurrentNode 로 세팅해주고 있다.
		// HPState 를 한번 체크하고 나면 다시 Null 로 되돌려준다w sdddddddddd.
		m_Owner->SetCurrentNode(nullptr);

		return NodeResult::Node_True;
	}
	else
	{
		m_Owner->SetCurrentNode(nullptr);

		return NodeResult::Node_False;
	}
}

NodeResult CBossBettyHPStateCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossBettyHPStateCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
