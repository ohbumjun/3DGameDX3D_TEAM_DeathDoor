
#include "CrowBossJumpStartCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"

CCrowBossJumpStartCheck::CCrowBossJumpStartCheck()
{
	SetTypeID(typeid(CCrowBossJumpStartCheck).hash_code());
}

CCrowBossJumpStartCheck::CCrowBossJumpStartCheck(const CCrowBossJumpStartCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossJumpStartCheck::~CCrowBossJumpStartCheck()
{
}

NodeResult CCrowBossJumpStartCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->GetStartJump())
		return NodeResult::Node_True;
	else
		return NodeResult::Node_False;
}

NodeResult CCrowBossJumpStartCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CCrowBossJumpStartCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
