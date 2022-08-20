
#include "BatRecognizeStartCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BatDataComponent.h"

CBatRecognizeStartCheck::CBatRecognizeStartCheck()
{
	SetTypeID(typeid(CBatRecognizeStartCheck).hash_code());
}

CBatRecognizeStartCheck::CBatRecognizeStartCheck(const CBatRecognizeStartCheck& Node)	:
	CConditionNode(Node)
{
}

CBatRecognizeStartCheck::~CBatRecognizeStartCheck()
{
}

NodeResult CBatRecognizeStartCheck::OnStart(float DeltaTime)
{
	CBatDataComponent* Data = dynamic_cast<CBatDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool Start = Data->IsRecognizeStart();

	if (Start)
		return NodeResult::Node_True;

	else
		return NodeResult::Node_False;
}

NodeResult CBatRecognizeStartCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBatRecognizeStartCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
