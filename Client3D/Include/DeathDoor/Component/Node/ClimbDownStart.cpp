
#include "ClimbDownStart.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Scene/Scene.h"


CClimbDownStart::CClimbDownStart()
{
	SetTypeID(typeid(CClimbDownStart).hash_code());
}

CClimbDownStart::CClimbDownStart(const CClimbDownStart& Node) :
	CActionNode(Node)
{
}

CClimbDownStart::~CClimbDownStart()
{
}

NodeResult CClimbDownStart::OnStart(float DeltaTime)
{

	CPlayerDataComponent* DataComp = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	DataComp->SetClimbingLadder(true);

	Vector3 PlayerPos = m_Object->GetWorldPos();

	// �÷��̾ ��ٸ��� �ٶ󺸵��� Rotation, ��ٸ� �߾ӿ� ������ x�� ����,  
	CGameObject* Ladder = DataComp->GetAdjLadder();

	if (Ladder)
	{
		// �÷��̾ �ٶ󺸴� ����
		Vector3 PlayerLookDir = m_Object->GetWorldAxis(AXIS_Z);
		PlayerLookDir *= -1.f;
		PlayerLookDir.y = 0.f;

		// ��ٸ��� �������� �ٶ󺸴� ����
		Vector3 LadderLookDir = Ladder->GetWorldAxis(AXIS_Z);
		LadderLookDir.y = 0.f;
		LadderLookDir *= -1.f;

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
		//m_Object->SetWorldPos(LadderPos.x - PlayerLookDir.x, PlayerPos.y * 0.58f + LadderPos.y * 0.42f, LadderPos.z - PlayerLookDir.z + 0.2f);
		m_Object->SetWorldPos(LadderPos.x, PlayerPos.y, LadderPos.z - PlayerLookDir.z / 2.f);
	}


	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("PlayerLadderDown");
	DataComp->GetSword()->GetAnimationInstance()->ChangeAnimation("PlayerLadderDown");

	return NodeResult::Node_True;
}

NodeResult CClimbDownStart::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CClimbDownStart::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}