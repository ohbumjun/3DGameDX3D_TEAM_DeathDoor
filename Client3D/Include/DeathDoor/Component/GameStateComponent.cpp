
#include "GameStateComponent.h"
#include "GameBehaviorTree.h"
#include "ObjectDataComponent.h"

CGameStateComponent::CGameStateComponent()	:
	m_Data(nullptr)
{
	m_ComponentType = Component_Type::ObjectComponent;

	SetTypeID<CGameStateComponent>();

	m_BehaviorTree = new CGameBehaviorTree;
	m_BehaviorTree->SetOwner(this);
}

CGameStateComponent::CGameStateComponent(const CGameStateComponent& com)	:
	CStateComponent(com)
{
}

CGameStateComponent::~CGameStateComponent()
{
}

void CGameStateComponent::Start()
{
	CStateComponent::Start();
	// TODO : GameStateComponent를 가지고 있는 오브젝트는 반드시 ObjectData 컴포넌트 혹은 ObjectDataComponent를 상속받은
	// 클래스를 ObjectData라는 이름으로 가지고 있어야 함
	m_Data = dynamic_cast<CObjectDataComponent*>(m_Object->FindComponent("ObjectData"));
}

CObjectDataComponent* CGameStateComponent::GetObjectDataComponent() const
{
	return m_Data;
}
