
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
	// TODO : GameStateComponent�� ������ �ִ� ������Ʈ�� �ݵ�� ObjectData ������Ʈ Ȥ�� ObjectDataComponent�� ��ӹ���
	// Ŭ������ ObjectData��� �̸����� ������ �־�� ��
	m_Data = dynamic_cast<CObjectDataComponent*>(m_Object->FindComponent("ObjectData"));
}

CObjectDataComponent* CGameStateComponent::GetObjectDataComponent() const
{
	return m_Data;
}
