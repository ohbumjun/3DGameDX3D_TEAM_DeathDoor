
#include "ClimbUpEnd.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Scene/Scene.h"
#include "Scene/Navigation3DManager.h"
#include "Component/ColliderComponent.h"
#include "../LadderCollider.h"

CClimbUpEnd::CClimbUpEnd()
{
	SetTypeID(typeid(CClimbUpEnd).hash_code());
}

CClimbUpEnd::CClimbUpEnd(const CClimbUpEnd& Node) :
	CActionNode(Node)
{
}

CClimbUpEnd::~CClimbUpEnd()
{
}

NodeResult CClimbUpEnd::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	DataComp->SetClimbingLadder(false);

	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("PlayerLadderFinish");

	Vector3 PlayerPos = m_Object->GetWorldPos();
	Vector3 PlayerFaceDir = m_Object->GetWorldAxis(AXIS_Z);

	PlayerFaceDir = Vector3(PlayerFaceDir.x *-1.f, PlayerFaceDir.y * -1.f, PlayerFaceDir.z * -1.f);

	float Height = 0.f;
	int PolyIndex = 0;

	/*bool Result1 = m_Object->GetScene()->GetNavigation3DManager()->CheckNavMeshPoly(Vector3(PlayerPos.x + PlayerFaceDir.x * 2.f, PlayerPos.y + 5.f, PlayerPos.z + PlayerFaceDir.z * 2.f), Height, PolyIndex);
	if (Result1 && Height + 1.5f >= PlayerPos.y)
	{

		PlayerPos = Vector3(PlayerPos.x + PlayerFaceDir.x * 2.f, Height, PlayerPos.z + PlayerFaceDir.z * 2.f);
		m_Object->SetWorldPos(PlayerPos);
		m_Object->GetScene()->GetNavigation3DManager()->RefreshPlayerNavMeshPoly(PlayerPos);

		DataComp->SetLadderUpEnable(false);

		CColliderComponent* Body = (CColliderComponent*)m_Object->FindComponent("Body");

		bool Collision = Body->CheckPrevCollisionFromType<CLadderCollider>();

		if (!Collision)
			DataComp->SetLadderDownEnable(false);

		DataComp->SetClimbingLadder(false);

		return NodeResult::Node_True;
	}*/

	bool Result2 = m_Object->GetScene()->GetNavigation3DManager()->CheckNavMeshPoly(Vector3(PlayerPos.x + PlayerFaceDir.x * 3.f, PlayerPos.y + 5.f, PlayerPos.z + PlayerFaceDir.z * 3.f), Height, PolyIndex);

	if (Result2 && Height + 1.5f >= PlayerPos.y)
	{
		PlayerPos = Vector3(PlayerPos.x + PlayerFaceDir.x * 3.f, Height, PlayerPos.z + PlayerFaceDir.z * 3.f);
		m_Object->SetWorldPos(PlayerPos);
		m_Object->GetScene()->GetNavigation3DManager()->RefreshPlayerNavMeshPoly(PlayerPos);

		DataComp->SetLadderUpEnable(false);

		CColliderComponent* Body = (CColliderComponent*)m_Object->FindComponent("Body");

		bool Collision = Body->CheckPrevCollisionFromType<CLadderCollider>();

		if (!Collision)
			DataComp->SetLadderDownEnable(false);

		DataComp->SetClimbingLadder(false);

		return NodeResult::Node_True;
	}


	bool Result3 = m_Object->GetScene()->GetNavigation3DManager()->CheckNavMeshPoly(Vector3(PlayerPos.x + PlayerFaceDir.x * 4.f, PlayerPos.y + 5.f, PlayerPos.z + PlayerFaceDir.z * 4.f), Height, PolyIndex);

	if (Result3 && Height + 1.5f >= PlayerPos.y)
	{
		PlayerPos = Vector3(PlayerPos.x + PlayerFaceDir.x * 4.f, Height, PlayerPos.z + PlayerFaceDir.z * 4.f);
		m_Object->SetWorldPos(PlayerPos);
		m_Object->GetScene()->GetNavigation3DManager()->RefreshPlayerNavMeshPoly(PlayerPos);

		DataComp->SetLadderUpEnable(false);

		CColliderComponent* Body = (CColliderComponent*)m_Object->FindComponent("Body");

		bool Collision = Body->CheckPrevCollisionFromType<CLadderCollider>();

		if (!Collision)
			DataComp->SetLadderDownEnable(false);

		DataComp->SetClimbingLadder(false);

		return NodeResult::Node_True;
	}

	else
	{
		// TODO: 사다리를 다 올라가고 나온 평지에 네비메쉬와 사다리까지의 거리가 너무 멀 경우 예외처리 필요
		assert(false);
	}

	return NodeResult::Node_True;
}

NodeResult CClimbUpEnd::OnUpdate(float DeltaTime)
{
	


	return NodeResult::Node_True;
}

NodeResult CClimbUpEnd::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}