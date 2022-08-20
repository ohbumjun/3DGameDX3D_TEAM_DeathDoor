
#include "CrowBossMoveShootSpot.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../CrowBossDataComponent.h"
#include "Component/AnimationMeshComponent.h"

CCrowBossMoveShootSpot::CCrowBossMoveShootSpot()
{
	SetTypeID(typeid(CCrowBossMoveShootSpot).hash_code());
}

CCrowBossMoveShootSpot::CCrowBossMoveShootSpot(const CCrowBossMoveShootSpot& Node) :
	CActionNode(Node)
{
}

CCrowBossMoveShootSpot::~CCrowBossMoveShootSpot()
{
}

NodeResult CCrowBossMoveShootSpot::OnStart(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCrowBossMoveShootSpot::OnUpdate(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	Data->Teleport();
	
	return NodeResult::Node_True;
}

NodeResult CCrowBossMoveShootSpot::OnEnd(float DeltaTime)
{
	return NodeResult();
}