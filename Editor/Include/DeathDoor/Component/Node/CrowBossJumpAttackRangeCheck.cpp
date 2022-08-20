
#include "CrowBossJumpAttackRangeCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"
#include "Scene/Scene.h"

CCrowBossJumpAttackRangeCheck::CCrowBossJumpAttackRangeCheck()
{
	SetTypeID(typeid(CCrowBossJumpAttackRangeCheck).hash_code());
}

CCrowBossJumpAttackRangeCheck::CCrowBossJumpAttackRangeCheck(const CCrowBossJumpAttackRangeCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossJumpAttackRangeCheck::~CCrowBossJumpAttackRangeCheck()
{
}

NodeResult CCrowBossJumpAttackRangeCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CGameObject* Player = m_Object->GetScene()->GetPlayerObject();
	Vector3 PlayerPos = Player->GetWorldPos();
	Vector3 MyPos = m_Object->GetWorldPos();

	float JumpAttackRange = Data->GetJumpAttackRange();
	float Dist = PlayerPos.Distance(MyPos);

	if (Dist <= JumpAttackRange)
		return NodeResult::Node_True;
	else
		return NodeResult::Node_False;
}

NodeResult CCrowBossJumpAttackRangeCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CCrowBossJumpAttackRangeCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
