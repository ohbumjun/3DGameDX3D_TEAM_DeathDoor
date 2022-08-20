
#include "ArrowComponent.h"
#include "GameObject/GameObject.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderBox3D.h"

CArrowComponent::CArrowComponent()	:
	m_Collider(nullptr),
	m_OnFire(false)
{
	SetTypeID<CArrowComponent>();

	m_ComponentType = Component_Type::ObjectComponent;
}

CArrowComponent::CArrowComponent(const CArrowComponent& com)	:
	CProjectileComponent(com)
{
}

CArrowComponent::~CArrowComponent()
{
}

bool CArrowComponent::Init()
{
	return true;
}

void CArrowComponent::Start()
{
	m_Collider = m_Object->FindComponentFromType<CColliderSphere>();

	if (!m_Collider)
	{
		m_Collider = m_Object->FindComponentFromType<CColliderBox3D>();
	}

	CProjectileComponent::Start();


}

void CArrowComponent::Reset()
{
	CProjectileComponent::Reset();

	m_Object->SetWorldRotation(0.f, 0.f, 0.f);

	m_OnFire = false;
}

void CArrowComponent::Update(float DeltaTime)
{
	CProjectileComponent::Update(DeltaTime);
}

void CArrowComponent::PostUpdate(float DeltaTime)
{
	CProjectileComponent::PostUpdate(DeltaTime);

	m_Collider->SetRelativePos(0.f, 0.f, 0.9f);
}

void CArrowComponent::PrevRender()
{
}

void CArrowComponent::Render()
{
}

void CArrowComponent::PostRender()
{
}

CObjectComponent* CArrowComponent::Clone()
{
	return new CArrowComponent(*this);
}

bool CArrowComponent::Save(FILE* File)
{
	return CProjectileComponent::Save(File);
}

bool CArrowComponent::Load(FILE* File)
{
	return CProjectileComponent::Load(File);
}

bool CArrowComponent::SaveOnly(FILE* File)
{
	return CProjectileComponent::SaveOnly(File);
}

bool CArrowComponent::LoadOnly(FILE* File)
{
	return CProjectileComponent::LoadOnly(File);
}

