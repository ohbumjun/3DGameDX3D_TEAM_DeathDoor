
#include "CrowBossCutScenePlayCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"

CCrowBossCutScenePlayCheck::CCrowBossCutScenePlayCheck()
{
	SetTypeID(typeid(CCrowBossCutScenePlayCheck).hash_code());
}

CCrowBossCutScenePlayCheck::CCrowBossCutScenePlayCheck(const CCrowBossCutScenePlayCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossCutScenePlayCheck::~CCrowBossCutScenePlayCheck()
{
}

NodeResult CCrowBossCutScenePlayCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

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

NodeResult CCrowBossCutScenePlayCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossCutScenePlayCheck::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
