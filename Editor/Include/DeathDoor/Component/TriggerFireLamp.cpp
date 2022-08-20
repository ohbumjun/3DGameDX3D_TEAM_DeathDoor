#include "TriggerFireLamp.h"
#include "ArrowCollisionFireCollider.h"
#include "Component/LightComponent.h"
#include "Scene/Scene.h"
#include "../Scene/DDPuzzleSceneMode.h"

// ArrowCollisionFireCollider ���� �����ϴ� Object Data Component
CTriggerFireLamp::CTriggerFireLamp() :
	m_CurrentActive(false)
{
	SetTypeID<CTriggerFireLamp>();
	m_ComponentType = Component_Type::ObjectComponent;
}
CTriggerFireLamp::~CTriggerFireLamp()
{
}

void CTriggerFireLamp::IncActiveCount()
{
	m_NumCurActiveFireLamp += 1;

	if (m_NumCurActiveFireLamp >= m_NumFullActiveFireLamp)
	{
		m_CurrentActive = true;

		// ���⿡�� SceneMode �� �� �����ٰ� �˷��ֱ� 
		CDDPuzzleSceneMode* PuzzleSceneMode = dynamic_cast<CDDPuzzleSceneMode*>(m_Scene->GetSceneMode());
		PuzzleSceneMode->OnClearDungeon();

		//  m_Scene->GetSceneMode() -> OnClearDungeon
	}
}

void CTriggerFireLamp::Start()
{
	CObjectDataComponent::Start();
	
	const std::vector<CSharedPtr<CGameObject>>& VecChildObject = m_Object->GetVecChildObjects();

	size_t ChildCount = m_Object->GetChildObjectCount();

	m_NumFullActiveFireLamp = 0;
	m_NumCurActiveFireLamp = 1; // ó������ �Ѱ��� ���� ���̴�.
	m_CurrentActive = false;

	for (size_t i = 0; i < ChildCount; ++i)
	{
		CArrowCollisionFireCollider* Component = VecChildObject[i]->FindComponentFromType<CArrowCollisionFireCollider>();

		if (Component)
		{
			m_NumFullActiveFireLamp += 1;
		}

		// ù��° Fire �� ������ �Ѵ�.
		// if (i == 0)
		if (VecChildObject[i]->GetName() == "FireBowl")
		{
			Component->SetFirstManagedChildByTriggerLamp(true);
		}
	}

}
