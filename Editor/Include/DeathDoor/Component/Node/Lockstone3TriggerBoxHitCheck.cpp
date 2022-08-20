
#include "Lockstone3TriggerBoxHitCheck.h"
#include "Component/BehaviorTree.h"
#include "../GameStateComponent.h"
#include "../TriggerBoxData.h"

CLockstone3TriggerBoxHitCheck::CLockstone3TriggerBoxHitCheck()
{
	SetTypeID(typeid(CLockstone3TriggerBoxHitCheck).hash_code());
}

CLockstone3TriggerBoxHitCheck::CLockstone3TriggerBoxHitCheck(const CLockstone3TriggerBoxHitCheck& Node) :
	CConditionNode(Node)
{
}

CLockstone3TriggerBoxHitCheck::~CLockstone3TriggerBoxHitCheck()
{
}

NodeResult CLockstone3TriggerBoxHitCheck::OnStart(float DeltaTime)
{
	CTriggerBoxData* Data = dynamic_cast<CTriggerBoxData*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data && Data->GetIsHit() && Data->IsCurrentActive())
	{
		m_Owner->GetOwner()->SetTreeUpdate(false);
		return NodeResult::Node_True;
	}

	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CLockstone3TriggerBoxHitCheck::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CLockstone3TriggerBoxHitCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CLockstone3TriggerBoxHitCheck::Invoke(float DeltaTime)
{
	return CConditionNode::Invoke(DeltaTime);
}