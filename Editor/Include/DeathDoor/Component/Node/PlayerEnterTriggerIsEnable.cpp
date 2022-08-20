
#include "PlayerEnterTriggerIsEnable.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../MonsterDataComponent.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"
#include "Component/ColliderBox3D.h"

CPlayerEnterTriggerIsEnable::CPlayerEnterTriggerIsEnable()
{
	SetTypeID(typeid(CPlayerEnterTriggerIsEnable).hash_code());
}

CPlayerEnterTriggerIsEnable::CPlayerEnterTriggerIsEnable(const CPlayerEnterTriggerIsEnable& Node) :
	CConditionNode(Node)
{
}

CPlayerEnterTriggerIsEnable::~CPlayerEnterTriggerIsEnable()
{
}

NodeResult CPlayerEnterTriggerIsEnable::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	CColliderBox3D* Trigger = Data->GetPlayerEnterZoneTrigger();

	bool IsEnable = Trigger->IsEnable();

	if (IsEnable)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CPlayerEnterTriggerIsEnable::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CPlayerEnterTriggerIsEnable::OnEnd(float DeltaTime)
{
	return NodeResult();
}

