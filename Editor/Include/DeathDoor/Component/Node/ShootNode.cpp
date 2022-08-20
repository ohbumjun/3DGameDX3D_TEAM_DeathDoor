
#include "ShootNode.h"
#include "RotateAttackDirectionNode.h"
#include "Component/AnimationMeshComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Component/BehaviorTree.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../PlayerDataComponent.h"
#include "../PlayerHookComponent.h"
#include "../PlayerBowComponent.h"
#include "../PlayerBombComponent.h"
#include "Component/Node/CompositeNode.h"
#include "../GameStateComponent.h"

CShootNode::CShootNode()	:
	m_InRestoreCam(false),
	m_Throw(false)
{
	SetTypeID(typeid(CShootNode).hash_code());
}

CShootNode::CShootNode(const CShootNode& Node) :
	CActionNode(Node)
{
}

CShootNode::~CShootNode()
{
}

NodeResult CShootNode::OnStart(float DeltaTime)
{
	m_CallStart = true;

	// ī�޶� ���� ����
	CCameraComponent* CurCam = m_Object->GetScene()->GetCameraManager()->GetCurrentCamera();
	CurCam->SetMoveReverse(true);
	CurCam->SetMoveFreeze(false);

	return NodeResult::Node_True;
}

NodeResult CShootNode::OnUpdate(float DeltaTime)
{
	CPlayerDataComponent* Comp = m_Object->FindObjectComponentFromType<CPlayerDataComponent>();

	Player_Ability Ability = Comp->GetPlayerAbility();

	CRotateAttackDirectionNode* Node = (CRotateAttackDirectionNode*)((CCompositeNode*)(m_Parent->GetParent()->GetParent()))->FindChildByType<CRotateAttackDirectionNode>();
	Vector3 Point = Node->GetPickingPoint();

	Vector3 ObjectPos = m_Object->GetWorldPos();
	Vector3 Dir = Vector3(Point.x, 0.f, Point.z) - Vector3(ObjectPos.x, 0.f, ObjectPos.z);
	Dir.Normalize();

	CCameraComponent* CurCam = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	bool IsCamMoving = CurCam->IsMoving();

	if (Ability == Player_Ability::Hook)
	{
		// ī�޶� ������
		if (m_InRestoreCam)
		{
			CurCam->SetMoveFreeze(false);

			// ī�޶� ���Ͱ� ������ ��忡�� ��������
			if (!IsCamMoving)
			{
				m_IsEnd = true;
				m_CallStart = false;
				m_Owner->SetCurrentNode(nullptr);
				m_InRestoreCam = false;

				return NodeResult::Node_True;
			}

			return NodeResult::Node_Running;
		}

		CPlayerHookComponent* Comp = m_Object->FindComponentFromType<CPlayerHookComponent>();
		bool HookCollision = false;
		if (Comp)
		{
			HookResult Result = Comp->ShootHook(Dir, DeltaTime);

			if (Result == HookResult::NoCollision)
			{
				// �浹���� ���� ��� ī�޶� ����
				CurCam->SetMoveReverse(true);
				m_InRestoreCam = true;
				return NodeResult::Node_Running;
			}

			else if (Result == HookResult::Collision)
			{
				// ���ư��� �� ī�޶� ��� �� �ڸ��� ������
				m_Owner->SetCurrentNode(this);
				CurCam->SetMoveReverse(false);
				CurCam->SetMoveFreeze(true);
				return NodeResult::Node_Running;
				// �浹�� �������� �÷��̾ ���ư��� �ִ���
			}

			else if(Result == HookResult::OnShoot)
			{
				// �߻��� ī�޶� ������
				m_Owner->SetCurrentNode(this);
				CurCam->SetMoveReverse(false);
				CurCam->SetMoveFreeze(true);
				return NodeResult::Node_Running;
			}

			else if (Result == HookResult::CollisionEnd)
			{
				// �浹�ϸ� ī�޶� ���ͽ�Ű�� �ʴ´�.
				m_Owner->SetCurrentNode(nullptr);
				CurCam->StopMove();
				CurCam->SetRelativePos(0.f, 0.f, 0.f);
				return NodeResult::Node_True;
			}

		}
		m_CallStart = true;

		return NodeResult::Node_True;
	}

	else if (Ability == Player_Ability::Arrow)
	{
		if (!IsCamMoving)
		{
			m_IsEnd = true;
			m_CallStart = false;
			m_Owner->SetCurrentNode(nullptr);
			m_InRestoreCam = false;
			m_Throw = false;

			return NodeResult::Node_True;
		}

		else
		{
			CPlayerBowComponent* BowComp = m_Object->FindComponentFromType<CPlayerBowComponent>();

			if (BowComp && !m_Throw)
			{
				BowComp->ShootArrow(Dir);

				m_Owner->SetCurrentNode(this);

				BowComp->HideBow();

				m_Object->GetScene()->GetResource()->SoundPlay("ArrowFire");

				m_Throw = true;
			}
		}

		m_CallStart = true;

		return NodeResult::Node_True;
	}

	else if (Ability == Player_Ability::Bomb)
	{
		if (!IsCamMoving)
		{
			m_IsEnd = true;
			m_CallStart = false;
			m_Owner->SetCurrentNode(nullptr);
			m_InRestoreCam = false;
			m_Throw = false;

			return NodeResult::Node_True;
		}

		else
		{
			CPlayerBombComponent* BombComp = m_Object->FindObjectComponentFromType<CPlayerBombComponent>();

			// ReadyToShoot ��忡�� Bomb Lift�� ���� �ȳ����µ� RButton�� ���� BombComponent�� Bomb Object�� reset ��Ű�� 
			// BombComponent�� m_IsClearBomb�� true�� ������ش�
			if (BombComp->IsClearBomb())
			{
				BombComp->SetClearBomb(false);

			}

			else if(!BombComp->IsBeforeLift() && !m_Throw)
			{
				BombComp->ShootBomb(Dir);
				m_Throw = true;
			}

			m_InRestoreCam = true;

			if (BombComp->IsBeforeLift())
			{
				m_InRestoreCam = false;
			}

			m_Owner->SetCurrentNode(this);

			m_IsEnd = false;
		}

		m_CallStart = true;

		return NodeResult::Node_True;
	}

	// Warning �����ϱ� ���� (OBJ)�� �߰� 
	return NodeResult::Node_True;
}

NodeResult CShootNode::OnEnd(float DeltaTime)
{
	m_Owner->SetCurrentNode(nullptr);
	m_IsEnd = false;

	return NodeResult::Node_True;
}

NodeResult CShootNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}
