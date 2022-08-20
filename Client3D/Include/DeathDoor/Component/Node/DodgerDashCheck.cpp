#include "DodgerDashCheck.h"
#include "../DodgerDataComponent.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"

CDodgerDashCheck::CDodgerDashCheck()
{
	SetTypeID(typeid(CDodgerDashCheck).hash_code());
}

CDodgerDashCheck::CDodgerDashCheck(const CDodgerDashCheck& Node)	:
	CConditionNode(Node)
{
}

CDodgerDashCheck::~CDodgerDashCheck()
{
}

NodeResult CDodgerDashCheck::OnStart(float DeltaTime)
{
	CDodgerDataComponent* Data = dynamic_cast<CDodgerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool DashReady = Data->IsDashReady();

	if (DashReady)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CDodgerDashCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CDodgerDashCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
