
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

		// OBJ�� �ӽ� �߰� -> Bomb�� ���µ��� �����Ϸ��� �ؼ� ��� ��Ÿ�� ����
		if (!BombComp)
		{
			return NodeResult::Node_True;;
		}

		BombComp->SetBeforeLift(true);

		CGameObject* Bomb = BombComp->GetBomb();

		// �̹� Lift & Shoot�� �ѹ� �̻� �ߴµ� BombComp�� m_Bomb�� nullptr�� �ƴϸ�
		// �� m_Bomb�� �̹� ��ź ����Ʈ�� �������̶�� �ǹ��̹Ƿ� Lift & Shoot�� ���� �ʴ´�
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

		// AnimationSequenceInstance���� m_ChangeTime�� �ֱ� ������, �ٷ� �ִϸ��̼��� �ٲ�°� �ƴ϶� m_ChangeTime�� ���ļ� �ִϸ��̼��� �ٲ�µ�
		// CurrentAnimation�� ReadyToShoot�̶� ChangeAnimation�ص� �ƹ��͵� ���ϰ� �ٷ� return�ǰ� ChangeAnimation�� Idle���� �������� �����Ǿ� �ִ� ���¿���
		// �� ���¿��� OnUpdate�� ���� Idle�� ������ Sequence�� �ٲ� ���¿��� ReadyToShoot::OnUpdate���� CurrentAnimation�� ReadyToShoot�̶�� ����������
		// CurrentAnimation�� �ᱹ Idle�� �� ���̰�, ChangeAnimation�� nullptr�� �� ���̹Ƿ� OnUpdate�� else���� ��� ������ �Ǵ� ������ ����Ƿ�
		// �̷� ��Ȳ������ KeepAnimation�� ȣ���ؼ� ChangeAnimation�� ����� CurrentAnimation�� Attack���� ���������ش�
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

	// ī�޶� �̵������� ���� ���� ���ο� ī�޶� �̵��� �����Ѵ�.
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

	// ī�޶� �̵��� ���� ���
	if (!IsCamMoving)
	{
		CPlayerBombComponent* BombComp = m_Object->FindComponentFromType<CPlayerBombComponent>();

		if (Ability == Player_Ability::Bomb && BombComp->IsCancleAction())
		{

			m_IsEnd = true;
			// OnStart���� CancleAction�� True�� ��������� ī�޶� �̵��� ���� �ʰ� �ٷ� ����
			// Node_False�� �����ؼ� Sibling Node�� ShootNode�� �Ѿ�� �ʴ´�

			// CancleAction�� �ٽ� ������� false�� ����
			BombComp->SetCancleAction(false);

			return NodeResult::Node_False;
		}

		bool RButtonUp = CInput::GetInst()->GetMouseRButtonUp();

		// ī�޶� ȭ���� ��ǥ �������� ������ �̵��ϱ� ���� ���콺RButton �� ����
		if (RButtonUp)
		{
			if (Ability == Player_Ability::Bomb)
			{
				// ī�޶� �̵��� ���� �ȳ����µ� RButton�� ����
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

			// ī�޶� �̵� ������
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

	// ī�޶� �̵���
	else
	{
		bool RButtonUp = CInput::GetInst()->GetMouseRButtonUp();

		// ī�޶� �̵��� ������ ����ü�� ����� �غ����϶� ���Ϳ��� �¾Ҵٸ�
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

			// ī�޶� �̵� �ݴ��
			CurCam->SetMoveReverse(true);
			CurCam->SetMoveFreeze(false);

		}

		if (RButtonUp)
		{
			if (Ability == Player_Ability::Bomb)
			{
				CPlayerBombComponent* BombComp = m_Object->FindComponentFromType<CPlayerBombComponent>();
				CGameObject* Bomb = BombComp->GetBomb();

				// Bomb Lift�� ���� �ȳ����µ� RButton�� ����
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

			// ī�޶� �̵� �ݴ��
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