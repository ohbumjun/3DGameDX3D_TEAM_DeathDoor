
#include "TriggerBoxData.h"
#include "Component/StaticMeshComponent.h"

CTriggerBoxData::CTriggerBoxData()	:
	m_CurrentActive(false),
	m_BoxOrder(1)
{
	SetTypeID<CTriggerBoxData>();

	m_ComponentType = Component_Type::ObjectComponent;
}

CTriggerBoxData::CTriggerBoxData(const CTriggerBoxData& com) :
	CObjectDataComponent(com)
{
}

CTriggerBoxData::~CTriggerBoxData()
{
}

void CTriggerBoxData::Start()
{
	CObjectDataComponent::Start();

	CStaticMeshComponent* Comp = m_Object->FindComponentFromType<CStaticMeshComponent>();

	if (m_CurrentActive)
	{
		Comp->SetEmissiveColor(Vector4(0.4f, 0.2f, 0.35f, 1.f));
	}

	else
	{
		Comp->SetEmissiveColor(Vector4(0.f, 0.f, 0.f, 1.f));
	}
}

void CTriggerBoxData::Update(float DeltaTime)
{
	CObjectDataComponent::Update(DeltaTime);

	CStaticMeshComponent* Comp = m_Object->FindComponentFromType<CStaticMeshComponent>();

	if (m_CurrentActive)
	{
		Comp->SetEmissiveColor(Vector4(0.4f, 0.2f, 0.35f, 1.f));
	}

	else
	{
		Comp->SetEmissiveColor(Vector4(0.f, 0.f, 0.f, 1.f));
	}
}

bool CTriggerBoxData::Save(FILE* File)
{
	CObjectDataComponent::Save(File);

	fwrite(&m_CurrentActive, sizeof(bool), 1, File);
	fwrite(&m_BoxOrder, sizeof(int), 1, File);

	return true;
}

bool CTriggerBoxData::Load(FILE* File)
{
	CObjectDataComponent::Load(File);

	fread(&m_CurrentActive, sizeof(bool), 1, File);
	fread(&m_BoxOrder, sizeof(int), 1, File);

	return true;
}
