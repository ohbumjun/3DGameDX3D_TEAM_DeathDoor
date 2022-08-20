
#include "Lockstone3TriggerBoxAction.h"
#include "Component/ColliderBox3D.h"
#include "../EyeLaserComponent.h"
#include "../TriggerBoxData.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Component/StaticMeshComponent.h"
#include "../GameStateComponent.h"
#include "../../Scene/DDPuzzleSceneMode.h"

CLockstone3TriggerBoxAction::CLockstone3TriggerBoxAction()
{
	SetTypeID(typeid(CLockstone3TriggerBoxAction).hash_code());
}

CLockstone3TriggerBoxAction::CLockstone3TriggerBoxAction(const CLockstone3TriggerBoxAction& Node) :
	CActionNode(Node)
{
}

CLockstone3TriggerBoxAction::~CLockstone3TriggerBoxAction()
{
}

NodeResult CLockstone3TriggerBoxAction::OnStart(float DeltaTime)
{
	CGameObject* Laser = CSceneManager::GetInst()->GetScene()->FindObject("LaserGenerator");

	if (Laser)
	{
		CEyeLaserComponent* EyeLaserComp = Laser->FindComponentFromType<CEyeLaserComponent>();
		CColliderBox3D* TriggerBox = m_Object->FindComponentFromType<CColliderBox3D>();
		CTriggerBoxData* TriggerBoxData = dynamic_cast<CTriggerBoxData*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
		
		if (TriggerBox)
			TriggerBox->Enable(false);

		if (EyeLaserComp)
		{
			EyeLaserComp->AddTriggerCount();

			CStaticMeshComponent* Comp = m_Object->FindComponentFromType<CStaticMeshComponent>();
			Comp->SetEmissiveColor(Vector4(0.f, 0.f, 0.f, 1.f));

			TriggerBoxData->SetCurrentActive(false);

			int TriggerBoxOrder = TriggerBoxData->GetBoxOrder();

			if (TriggerBoxOrder < 5)
			{
				// 내가 1번 Box면, 2번 Box를 활성화 시켜야하니 Parent Object의 vecChildObject의 1번 Index에 있는 Child를 활성화 해준다
				CGameObject* NextTriggerBox = m_Object->GetParentObject()->GetChildObject(TriggerBoxOrder);

				if (NextTriggerBox)
				{
					CTriggerBoxData* NextTriggerBoxData = NextTriggerBox->FindObjectComponentFromType<CTriggerBoxData>();
					NextTriggerBoxData->SetCurrentActive(true);
				}

			}

			else if (TriggerBoxOrder == 5)
			{
				CDDPuzzleSceneMode* PuzzleScene = (CDDPuzzleSceneMode*)m_Object->GetScene()->GetSceneMode();
				PuzzleScene->OnClearDungeon();
				m_Object->GetScene()->GetResource()->SoundPlay("BlockerUp");
			}

		}

		m_Object->GetScene()->GetCameraManager()->GetCurrentCamera()->Shake(0.2f, 0.3f);

		//m_Object->SetHit(false);
	}

	return NodeResult::Node_True;
}

NodeResult CLockstone3TriggerBoxAction::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CLockstone3TriggerBoxAction::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CLockstone3TriggerBoxAction::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}
