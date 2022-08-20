
#include "BatRecognizeEndCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../BatDataComponent.h"

CBatRecognizeEndCheck::CBatRecognizeEndCheck()
{
	SetTypeID(typeid(CBatRecognizeEndCheck).hash_code());
}

CBatRecognizeEndCheck::CBatRecognizeEndCheck(const CBatRecognizeEndCheck& Node) :
	CConditionNode(Node)
{
}

CBatRecognizeEndCheck::~CBatRecognizeEndCheck()
{
}

NodeResult CBatRecognizeEndCheck::OnStart(float DeltaTime)
{
	CBatDataComponent* Data = dynamic_cast<CBatDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool End = Data->IsRecognizeEnd();

	if (End)
		return NodeResult::Node_True;

	else
		return NodeResult::Node_False;
}

NodeResult CBatRecognizeEndCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBatRecognizeEndCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
