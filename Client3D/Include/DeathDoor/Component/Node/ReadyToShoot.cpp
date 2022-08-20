
#include "ReadyToShoot.h"
#include "RotateAttackDirectionNode.h"
#include "Component/Node/CompositeNode.h"
#include "Component/AnimationMeshComponent.h"
#include "../../Component/PlayerDataComponent.h"
#include "../../Component/PlayerBowComponent.h"
#include "../../Component/PlayerBombComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "../GameStateComponent.h"
#include "Component/BehaviorTree.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Input.h"


CReadyToShoot::CReadyToShoot() :
	m_CameraMoveEnd(false),
	m_CameraMoveSpeed(50.f),
	m_CameraMoveTime(0.f),
	m_LiftStart(false)
{
	SetTypeID(typeid(CReadyToShoot).hash_code());
}

CReadyToShoot::CReadyToShoot(const CReadyToShoot& Node) :
	CActionNode(Node)
{
}

CReadyToShoot::~CReadyToShoot()
{
}

NodeResult CReadyToShoot::OnStart(float DeltaTime)
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	m_PlayerDataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	Player_Ability Ability = m_PlayerDataComp->GetPlayerAbility();

	std::string ObjectName = m_Object->GetName();
	std::string SequenceName;

	if (Ability == Player_Ability::Arrow)
	{
		SequenceName = ObjectName + "Arrow";
	}

	else if (Ability == Player_Ability::Hook)
	{
		SequenceName = ObjectName + "Hook";
	}

	else if (Ability == Player_Ability::Bomb)
	{
		CPlayerBombComponent* BombComp = m_Object->FindComponentFromType<CPlayerBombComponent>();

		// OBJ가 임시 추가 -> Bomb가 없는데도 실행하려고 해서 계속 런타임 에러
		if (!BombComp)
		{
			return NodeResult::Node_True;;
		}

		BombComp->SetBeforeLift(true);

		CGameObject* Bomb = BombComp->GetBomb();

		// 이미 Lift & Shoot을 한번 이상 했는데 BombComp의 m_Bomb가 nullptr가 아니면
		// 그 m_Bomb는 이미 폭탄 이펙트가 진행중이라는 의미이므로 Lift & Shoot을 하지 않는다
		if (!BombComp->IsShootFirstTime() && Bomb)
		{
			m_IsEnd = true;
			BombComp->SetCancleAction(true);

			CProjectileComponent* Proj = Bomb->FindObjectComponentFromType<CProjectileComponent>();
			if (Proj)
			{
				Bomb->Reset();
				BombComp->ResetInfo();
			}

			return NodeResult::Node_True;
		}

		else
		{
			SequenceName = ObjectName + "Bomb";
			BombComp->SetCancleAction(false);
		}
	}

	if (m_AnimationMeshComp && Ability != Player_Ability::None)
	{
		CAnimationSequenceInstance* Instance = m_AnimationMeshComp->GetAnimationInstance();

		// AnimationSequenceInstance에서 m_ChangeTime이 있기 때문에, 바로 애니메이션이 바뀌는게 아니라 m_ChangeTime에 걸쳐서 애니메이션이 바뀌는데
		// CurrentAnimation은 ReadyToShoot이라서 ChangeAnimation해도 아무것도 안하고 바로 return되고 ChangeAnimation은 Idle같은 시퀀스로 설정되어 있는 상태에서
		// 이 상태에서 OnUpdate로 들어가고 Idle로 완전히 Sequence가 바뀐 상태에서 ReadyToShoot::OnUpdate에서 CurrentAnimation은 ReadyToShoot이라고 생각하지만
		// CurrentAnimation은 결국 Idle로 될 것이고, ChangeAnimation은 nullptr가 될 것이므로 OnUpdate의 else에만 계속 들어오게 되는 문제가 생기므로
		// 이런 상황에서는 KeepAnimation을 호출해서 ChangeAnimation을 지우고 CurrentAnimation은 Attack으로 유지시켜준다
		if (Instance->GetChangeAnimation() && Instance->GetCurrentAnimation()->GetName() == SequenceName)
			Instance->KeepCurrentAnimation();

		else
			Instance->ChangeAnimation(SequenceName);
	}

	//m_Object->SetNoInterrupt(true);
	m_CallStart = true;

	m_PlayerDataComp->GetSword()->GetAnimationInstance()->ChangeAnimation(SequenceName);

	CRotateAttackDirectionNode* Node = (CRotateAttackDirectionNode*)((CCompositeNode*)m_Parent->GetParent()->GetParent())->FindChildByType<CRotateAttackDirectionNode>();

	Vector3 PickingPoint = Node->GetPickingPoint();

	CCameraComponent* CurrentCamera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	// 카메라 이동중이지 않을 때만 새로운 카메라 이동을 수행한다.
	bool IsMoving = CurrentCamera->IsMoving();

	if (!IsMoving)
	{
		Vector3 ObjectPos = m_Object->GetWorldPos();
		Vector3 OriginCamPos = CurrentCamera->GetWorldPos();
		CSceneManager::GetInst()->GetScene()->SetOriginCamPos(OriginCamPos);

		m_CameraDestPos =  OriginCamPos + (PickingPoint - ObjectPos) / 2.f;
		m_CameraDestPos.y = OriginCamPos.y;
		
		float Dist = OriginCamPos.Distance(m_CameraDestPos);
		m_CameraMoveTime = Dist / m_CameraMoveSpeed;

		m_CameraMoveDir = PickingPoint - ObjectPos;
		m_CameraMoveDir.y = 0.f;
		m_CameraMoveDir.Normalize();

		CurrentCamera->StartMove(OriginCamPos, m_CameraDestPos, (float)m_CameraMoveTime, false, true);
	}

	return NodeResult::Node_True;
}

NodeResult CReadyToShoot::OnUpdate(float DeltaTime)
{
	Player_Ability Ability = m_PlayerDataComp->GetPlayerAbility();

	if (Ability == Player_Ability::None)
		return NodeResult::Node_True;

	if (Ability == Player_Ability::Arrow)
	{
		CPlayerBowComponent* BowComp = m_Object->FindComponentFromType<CPlayerBowComponent>();

		if (BowComp && !BowComp->GetShowBow())
		{
			BowComp->ShowBow(m_CameraMoveDir);

			m_Object->GetScene()->GetResource()->SoundPlay("ArrowCharge");
			BowComp->SetShowBow(true);
		}
	}

	CCameraComponent* CurCam = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	bool IsCamMoving = CurCam->IsMoving();

	// 카메라 이동이 끝난 경우
	if (!IsCamMoving)
	{
		CPlayerBombComponent* BombComp = m_Object->FindComponentFromType<CPlayerBombComponent>();

		if (Ability == Player_Ability::Bomb && BombComp->IsCancleAction())
		{

			m_IsEnd = true;
			// OnStart에서 CancleAction을 True로 만들었으면 카메라 이동도 하지 않고 바로 리턴
			// Node_False를 리턴해서 Sibling Node인 ShootNode로 넘어가지 않는다

			// CancleAction을 다시 원래대로 false로 복귀
			BombComp->SetCancleAction(false);

			return NodeResult::Node_False;
		}

		bool RButtonUp = CInput::GetInst()->GetMouseRButtonUp();

		// 카메라가 화살쏘는 목표 지점으로 완전히 이동하기 전에 마우스RButton 을 때면
		if (RButtonUp)
		{
			if (Ability == Player_Ability::Bomb)
			{
				// 카메라 이동이 아직 안끝났는데 RButton을 떼면
				CGameObject* Bomb = BombComp->GetBomb();

				if (Bomb && !BombComp->IsEmptyLiftPathQueue())
				{
					Bomb->Reset();
					BombComp->ResetInfo();
				}
			}

			m_CallStart = false;
			m_Owner->SetCurrentNode(((CCompositeNode*)m_Parent->GetParent())->GetChild(1));

			m_IsEnd = false;

			// 카메라 이동 역으로
			CurCam->SetMoveReverse(true);
			CurCam->SetMoveFreeze(false);

			return NodeResult::Node_True;
		}

		else
		{
			m_Owner->SetCurrentNode(this);

			return NodeResult::Node_Running;
		}
	}

	// 카메라 이동중
	else
	{
		bool RButtonUp = CInput::GetInst()->GetMouseRButtonUp();

		// 카메라 이동은 끝났고 투사체를 쏘려고 준비중일때 몬스터에게 맞았다면
		bool IsHit = m_PlayerDataComp->GetIsHit();

		if (IsHit)
		{
			m_PlayerDataComp->GetAnimationMeshComponent()->GetAnimationInstance()->ChangeAnimation("PlayerHitBack");
			m_PlayerDataComp->SetFalseOnSlash();

			m_Object->GetScene()->GetCameraManager()->GetCurrentCamera()->Shake(0.2f, 0.2f);

			m_PlayerDataComp->SetNoInterrupt(true);

			if (Ability == Player_Ability::Bomb)
			{
				CPlayerBombComponent* BombComp = m_Object->FindComponentFromType<CPlayerBombComponent>();
				CGameObject* Bomb = BombComp->GetBomb();

				Bomb->Reset();
				BombComp->ResetInfo();
			}

			RButtonUp = true;

			// 카메라 이동 반대로
			CurCam->SetMoveReverse(true);
			CurCam->SetMoveFreeze(false);

		}

		if (RButtonUp)
		{
			if (Ability == Player_Ability::Bomb)
			{
				CPlayerBombComponent* BombComp = m_Object->FindComponentFromType<CPlayerBombComponent>();
				CGameObject* Bomb = BombComp->GetBomb();

				// Bomb Lift가 아직 안끝났는데 RButton을 떼면
				if (Bomb && !BombComp->IsEmptyLiftPathQueue())
				{
					Bomb->Reset();
					BombComp->ResetInfo();
					BombComp->SetClearBomb(true);
				}

			}

			m_CallStart = false;
			m_Owner->SetCurrentNode(((CCompositeNode*)m_Parent)->GetChild(1));
			m_IsEnd = false;

			// 카메라 이동 반대로
			CurCam->SetMoveReverse(true);
			CurCam->SetMoveFreeze(false);

			return NodeResult::Node_True;
		}

		else
		{
			m_Owner->SetCurrentNode(this);
			return NodeResult::Node_Running;
		}
	}


}

NodeResult CReadyToShoot::OnEnd(float DeltaTime)
{
	m_Owner->SetCurrentNode(nullptr);
	m_IsEnd = false;
	m_CallStart = false;

	return NodeResult::Node_True;
}

NodeResult CReadyToShoot::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}