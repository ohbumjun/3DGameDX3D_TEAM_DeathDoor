
#include "HPCheck.h"

CHPCheck::CHPCheck()
{
	SetTypeID(typeid(CHPCheck).hash_code());
}

CHPCheck::CHPCheck(const CHPCheck& Node)	:
	CConditionNode(Node)
{
}

CHPCheck::~CHPCheck()
{
}

NodeResult CHPCheck::OnStart(float DeltaTime)
{
	//CLoLDataComponent* DataComp = m_Object->FindObjectComponentFromType<CLoLDataComponent>();

	//if()

	return NodeResult();
}

NodeResult CHPCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CHPCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CHPCheck::Invoke(float DeltaTime)
{
	return NodeResult();
}