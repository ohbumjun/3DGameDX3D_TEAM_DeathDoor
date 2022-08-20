
#include "AddFallingFloorCallbackNode.h"
#include "Component/BehaviorTree.h"
#include "Component/ColliderBox3D.h"
#include "Component/StateComponent.h"
#include "Component/PaperBurnComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Navigation3DManager.h"
#include "Component/NavMeshComponent.h"

CAddFallingFloorCallbackNode::CAddFallingFloorCallbackNode()	:
	m_CollisionStart(false),
	m_AccTime(0.f),
	m_PlayerFall(false),
	m_PrevXVibFactor(15.f),
	m_PrevZVibFactor(15.f),
	m_FrameCount(0)
{
	SetTypeID(typeid(CAddFallingFloorCallbackNode).hash_code());
}

CAddFallingFloorCallbackNode::CAddFallingFloorCallbackNode(const CAddFallingFloorCallbackNode& Node)	:
	CActionNode(Node)
{
}

CAddFallingFloorCallbackNode::~CAddFallingFloorCallbackNode()
{
}

NodeResult CAddFallingFloorCallbackNode::OnStart(float DeltaTime)
{
	m_CallStart = true;

	m_OriginPos = m_Object->GetWorldPos();

	m_BoxCollider = m_Object->FindComponentFromType<CColliderBox3D>();

	if (m_BoxCollider)
	{
		m_BoxCollider->AddCollisionCallback<CAddFallingFloorCallbackNode>(Collision_State::Begin, this, &CAddFallingFloorCallbackNode::Trigger);
	}

	m_PlayerFall = false;

	return NodeResult::Node_True;
}

NodeResult CAddFallingFloorCallbackNode::OnUpdate(float DeltaTime)
{
	if (!m_CollisionStart)
		return NodeResult::Node_True;

	if (m_AccTime > 0.5f && m_AccTime < 1.5f)
	{
		if (!m_VibrateSoundPlayed)
		{
			m_Object->GetScene()->GetResource()->SoundPlay("FallingPlatformTouch");
			m_VibrateSoundPlayed = true;
		}
		Vibrate(DeltaTime);
	}

	if (m_AccTime > 1.5f && m_AccTime <= 2.5f)
	{
		auto iter = m_BoxCollider->GerPrevCollisionList().begin();
		auto iterEnd = m_BoxCollider->GerPrevCollisionList().end();

		for (; iter != iterEnd; ++iter)
		{
			CNavAgent* Agent = (*iter)->GetGameObject()->FindObjectComponentFromType<CNavAgent>();

			if (Agent && (*iter)->GetGameObject()->GetObjectType() == Object_Type::Player)
			{
				Agent->SetApplyNavMesh(false);
				m_PlayerFall = true;
			}

		}

		m_BoxCollider->Enable(false);
		m_Object->SetWorldRotationX(0.f);
		m_Object->SetWorldRotationZ(0.f);
		m_Object->AddWorldPos(0.f, -18.f * DeltaTime, 0.f);

		if (!m_FallingSoundPlayed)
		{
			m_Object->GetScene()->GetResource()->SoundPlay("FallingPlatformFall");
			m_FallingSoundPlayed = true;
		}
	}

	else if (m_AccTime > 2.5f)
	{
		m_Object->SetWorldPos(m_OriginPos);


		if (m_PlayerFall)
		{
			CNavigation3DManager* Manager = CSceneManager::GetInst()->GetScene()->GetNavigation3DManager();
			Vector3 Pos = m_Object->GetScene()->FindObject("PlayerSpawnPos")->GetWorldPos();
			CGameObject* Player = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

			Player->SetWorldPos(Pos);
			CNavAgent* Agent = Player->FindObjectComponentFromType<CNavAgent>();

			if (Agent)
				Agent->SetApplyNavMesh(true);

			Manager->RefreshPlayerNavMeshPoly(Pos);
		}

		// 원래는 PaperBurn 끝나고 EndCallback으로 호출할 함수
		ResetFallingBlock();
	}

	m_AccTime += DeltaTime;


	return NodeResult::Node_True;
}

NodeResult CAddFallingFloorCallbackNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

NodeResult CAddFallingFloorCallbackNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}

void CAddFallingFloorCallbackNode::Trigger(const CollisionResult& Result)
{
	if (m_CollisionStart)
		return;

	m_CollisionStart = true;
}

void CAddFallingFloorCallbackNode::ResetFallingBlock()
{
	m_AccTime = 0.f;
	m_CollisionStart = false;
	m_PlayerFall = false;
	m_VibrateSoundPlayed = false;
	m_FallingSoundPlayed = false;

	m_BoxCollider->Enable(true); 

	CPaperBurnComponent* PaperBurn = m_Object->FindObjectComponentFromType<CPaperBurnComponent>();

	PaperBurn->StartPaperBurn();

	m_Object->GetScene()->GetResource()->SoundPlay("TimedPlatformReturn");
}

void CAddFallingFloorCallbackNode::OnPaperburnEnd()
{
}

void CAddFallingFloorCallbackNode::Vibrate(float DeltaTime)
{
	++m_FrameCount;

	if (m_FrameCount < 4)
		return;

	m_PrevXVibFactor *= -1.f;
	m_PrevZVibFactor *= -1.f;

	m_Object->AddWorldPos(m_PrevXVibFactor * DeltaTime, 0.f, 0.f);
	m_Object->AddWorldPos(0.f, 0.f, m_PrevZVibFactor * DeltaTime);

	m_FrameCount = 0;
}
