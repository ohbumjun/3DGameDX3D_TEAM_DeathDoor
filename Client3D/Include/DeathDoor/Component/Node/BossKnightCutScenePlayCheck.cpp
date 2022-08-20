#include "BossKnightCutScenePlayCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../KnightDataComponent.h"

CBossKnightCutScenePlayCheck::CBossKnightCutScenePlayCheck()
{
	SetTypeID(typeid(CBossKnightCutScenePlayCheck).hash_code());
}

CBossKnightCutScenePlayCheck::CBossKnightCutScenePlayCheck(const CBossKnightCutScenePlayCheck& Node)	:
	CConditionNode(Node)
{
}

CBossKnightCutScenePlayCheck::~CBossKnightCutScenePlayCheck()
{
}

NodeResult CBossKnightCutScenePlayCheck::OnStart(float DeltaTime)
{
	CKnightDataComponent* Data = dynamic_cast<CKnightDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool CutScenePlaying = Data->IsCutScenePlaying();

	if (CutScenePlaying)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CBossKnightCutScenePlayCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossKnightCutScenePlayCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
