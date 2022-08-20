
#include "CancleShootNode.h"
#include "Component/AnimationMeshComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Component/BehaviorTree.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../PlayerDataComponent.h"
#include "../PlayerBowComponent.h"
#include "../PlayerBombComponent.h"
#include "../GameStateComponent.h"

CCancleShootNode::CCancleShootNode() :
	m_CameraMoveTime(0.f),
	m_CameraMoveSpeed(50.f)
{
	SetTypeID(typeid(CCancleShootNode).hash_code());
}

CCancleShootNode::CCancleShootNode(const CCancleShootNode& Node) :
	CActionNode(Node)
{
}

CCancleShootNode::~CCancleShootNode()
{
}

NodeResult CCancleShootNode::OnStart(float DeltaTime)
{
	m_CallStart = true;

	m_CurrentCamPos = m_Object->GetScene()->GetCameraManager()->GetCurrentCamera()->GetWorldPos();

	// Shoot방향으로 카메라가 움직이기 전에 ReadyToShoot 노드에서 저장해놓은 원래 카메라 위치
	Vector3 CamOriginPos = m_Object->GetScene()->GetOriginCamPos();
	float Dist = m_CurrentCamPos.Distance(CamOriginPos);

	m_CameraMoveTime = Dist / m_CameraMoveSpeed;

	CCameraComponent* CurCam = m_Object->GetScene()->GetCameraManager()->GetCurrentCamera();
	CurCam->SetMoveReverse(true);
	CurCam->SetMoveFreeze(false);

	return NodeResult::Node_True;
}

NodeResult CCancleShootNode::OnUpdate(float DeltaTime)
{
	CScene* Scene = CSceneManager::GetInst()->GetScene();

	CCameraComponent* CurCam = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();
	bool IsCamMoving = CurCam->IsMoving();

	bool RestoreEnd = Scene->RestoreCamera(m_CameraMoveTime, m_CurrentCamPos, DeltaTime);

	if (!IsCamMoving)
	{
		CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

		Player_Ability Ability = Data->GetPlayerAbility();

		if (Ability == Player_Ability::Bomb)
		{
			CPlayerBombComponent* BombComp = m_Object->FindObjectComponentFromType<CPlayerBombComponent>();

			if (BombComp)
			{
				BombComp->ResetInfo();

				CGameObject* Bomb = BombComp->GetBomb();

				if (Bomb)
				{
					Bomb->Reset();
				}
			}
		}

		else if (Ability == Player_Ability::Arrow)
		{
			CPlayerBowComponent* BowComp = m_Object->FindComponentFromType<CPlayerBowComponent>();

			BowComp->HideBow();
		}

		m_IsEnd = true;
		m_CallStart = false;
		m_Owner->SetCurrentNode(nullptr);

		return NodeResult::Node_True;

	}

 //	if (RestoreEnd)
 //	{
 //		CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
 //
 //		Player_Ability Ability = Data->GetPlayerAbility();
 //
 //		if (Ability == Player_Ability::Bomb)
 //		{
 //			CPlayerBombComponent* BombComp = m_Object->FindObjectComponentFromType<CPlayerBombComponent>();
 //
 //			if (BombComp)
 //			{
 //				BombComp->ResetInfo();
 //
 //				CGameObject* Bomb = BombComp->GetBomb();
 //
 //				if (Bomb)
 //				{
 //					Bomb->Reset();
 //				}
 //			}
 //		}
 //
 //		else if (Ability == Player_Ability::Arrow)
 //		{
 //			CPlayerBowComponent* BowComp = m_Object->FindComponentFromType<CPlayerBowComponent>();
 //
 //			BowComp->HideBow();
 //		}
 //
 //		m_IsEnd = true;
 //		m_CallStart = false;
 //		m_Owner->SetCurrentNode(nullptr);
 //
 //		return NodeResult::Node_True;
 //	}

	else
	{
		m_Owner->SetCurrentNode(this);
		m_CallStart = true;
		return NodeResult::Node_Running;
	}

	return NodeResult::Node_True;
}

NodeResult CCancleShootNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CCancleShootNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}

