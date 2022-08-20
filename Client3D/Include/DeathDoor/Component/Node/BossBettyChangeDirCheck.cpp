#include "BossBettyChangeDirCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BossBettyDataComponent.h"


CBossBettyChangeDirCheck::CBossBettyChangeDirCheck()
{
	SetTypeID(typeid(CBossBettyChangeDirCheck).hash_code());
}

CBossBettyChangeDirCheck::CBossBettyChangeDirCheck(const CBossBettyChangeDirCheck& Node)
{
}

CBossBettyChangeDirCheck::~CBossBettyChangeDirCheck()
{

}

NodeResult CBossBettyChangeDirCheck::OnStart(float DeltaTime)
{	// Spin 이면 true 를 리턴한다.
	CBossBettyDataComponent* BossBettyData = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	CMonsterDataComponent* MonsterData = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	float LimitAngle = BossBettyData->GetBossBettyChangeDirLimitAngle();
	float PlayerAngle = MonsterData->GetAnglePlayer();

	if (LimitAngle < PlayerAngle)
		return NodeResult::Node_True;
	else
		return NodeResult::Node_False;
}

NodeResult CBossBettyChangeDirCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossBettyChangeDirCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
