
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

	// 카메라 복귀 시작
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
		// 카메라 복귀중
		if (m_InRestoreCam)
		{
			CurCam->SetMoveFreeze(false);

			// 카메라 복귀가 끝나면 노드에서 빠져나감
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
				// 충돌하지 않은 경우 카메라 복귀
				CurCam->SetMoveReverse(true);
				m_InRestoreCam = true;
				return NodeResult::Node_Running;
			}

			else if (Result == HookResult::Collision)
			{
				// 날아가는 중 카메라 계속 그 자리에 프리즈
				m_Owner->SetCurrentNode(this);
				CurCam->SetMoveReverse(false);
				CurCam->SetMoveFreeze(true);
				return NodeResult::Node_Running;
				// 충돌된 지점까지 플레이어가 날아가고 있는중
			}

			else if(Result == HookResult::OnShoot)
			{
				// 발사중 카메라 프리즈
				m_Owner->SetCurrentNode(this);
				CurCam->SetMoveReverse(false);
				CurCam->SetMoveFreeze(true);
				return NodeResult::Node_Running;
			}

			else if (Result == HookResult::CollisionEnd)
			{
				// 충돌하면 카메라 복귀시키지 않는다.
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

			// ReadyToShoot 노드에서 Bomb Lift가 아직 안끝났는데 RButton을 떼면 BombComponent와 Bomb Object를 reset 시키고 
			// BombComponent의 m_IsClearBomb를 true로 만들어준다
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

	// Warning 제거하기 위해 (OBJ)가 추가 
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
