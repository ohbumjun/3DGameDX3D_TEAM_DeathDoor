#include "BossKnightJumpAttackRangeCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"
#include "Scene/Scene.h"

CBossKnightJumpAttackRangeCheck::CBossKnightJumpAttackRangeCheck()
{
	SetTypeID(typeid(CBossKnightJumpAttackRangeCheck).hash_code());
}

CBossKnightJumpAttackRangeCheck::CBossKnightJumpAttackRangeCheck(const CBossKnightJumpAttackRangeCheck& Node)	:
	CConditionNode(Node)
{
}

CBossKnightJumpAttackRangeCheck::~CBossKnightJumpAttackRangeCheck()
{
}

NodeResult CBossKnightJumpAttackRangeCheck::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	float JumpAttackRange = Data->GetJumpAttackRange();

	CGameObject* Player = m_Owner->GetOwner()->GetScene()->GetPlayerObject();

	if (!Player)
	{
		return NodeResult::Node_False;
	}

	CGameObject* MyObj = m_Owner->GetOwner()->GetGameObject();

	Vector3 MyPos = MyObj->GetWorldPos();
	Vector3 PlayerPos = Player->GetWorldPos();

	float Dist = MyPos.Distance(PlayerPos);

	if (Dist <= JumpAttackRange)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CBossKnightJumpAttackRangeCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossKnightJumpAttackRangeCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
