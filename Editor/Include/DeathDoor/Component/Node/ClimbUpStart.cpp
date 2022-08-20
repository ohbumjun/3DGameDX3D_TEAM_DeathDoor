
#include "ClimbUpStart.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Scene/Scene.h"

CClimbUpStart::CClimbUpStart()
{
	SetTypeID(typeid(CClimbUpStart).hash_code());
}

CClimbUpStart::CClimbUpStart(const CClimbUpStart& Node) :
	CActionNode(Node)
{
}

CClimbUpStart::~CClimbUpStart()
{
}

NodeResult CClimbUpStart::OnStart(float DeltaTime)
{
	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	DataComp->SetClimbingLadder(true);

	Vector3 PlayerPos = m_Object->GetWorldPos();

	CGameObject* Ladder = DataComp->GetAdjLadder();

	if (Ladder)
	{
		// 플레이어가 바라보는 방향
		Vector3 PlayerLookDir = m_Object->GetWorldAxis(AXIS_Z);
		PlayerLookDir *= -1.f;
		PlayerLookDir.y = 0.f;
		
		// 사다리가 벽쪽으로 바라보는 방향
		Vector3 LadderLookDir = Ladder->GetWorldAxis(AXIS_Z);
		LadderLookDir.y = 0.f;

		LadderLookDir *= -1.f;

		PlayerLookDir.Normalize();
		LadderLookDir.Normalize();

		float DotProduct = PlayerLookDir.Dot(LadderLookDir);
		float Degree = 0.f;
		if (DotProduct >= -0.99999999f && DotProduct <= 0.99999999f)
		{
			Degree = RadianToDegree(acosf(DotProduct));
			
			Vector3 CrossVec = PlayerLookDir.Cross(LadderLookDir);

			if (CrossVec.y < 0.f)
				Degree *= -1.f;

			m_Object->AddWorldRotationY(Degree);
		}

		PlayerLookDir = m_Object->GetWorldAxis(AXIS_Z);
		PlayerLookDir *= -1.f;
		PlayerLookDir.y = 0.f;

		Vector3 LadderPos = Ladder->GetWorldPos();
		m_Object->SetWorldPos(LadderPos.x, PlayerPos.y, LadderPos.z);
	}


	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("PlayerLadderUp");
	DataComp->GetSword()->GetAnimationInstance()->ChangeAnimation("PlayerLadderUp");

	return NodeResult::Node_True;
}

NodeResult CClimbUpStart::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CClimbUpStart::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}