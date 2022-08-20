
#include "CrowBossResetShootState.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../CrowBossDataComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Scene/Scene.h"
#include "../MonsterNavAgent.h"

CCrowBossResetShootState::CCrowBossResetShootState()
{
	SetTypeID(typeid(CCrowBossResetShootState).hash_code());
}

CCrowBossResetShootState::CCrowBossResetShootState(const CCrowBossResetShootState& Node) :
	CActionNode(Node)
{
}

CCrowBossResetShootState::~CCrowBossResetShootState()
{
}

NodeResult CCrowBossResetShootState::OnStart(float DeltaTime)
{

	return NodeResult::Node_True;
}

NodeResult CCrowBossResetShootState::OnUpdate(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	Data->SetShootState(CrowBossShootState::Ready);

	if (Data->GetCurrentShootCount() == 4)
	{
		CGameObject* Player = m_Object->GetScene()->GetPlayerObject();
		Vector3 PlayerPos = Player->GetWorldPos();
		Vector3 MyPos = m_Object->GetWorldPos();
		Vector3 Dir = PlayerPos - MyPos;
		Dir.Normalize();

		Vector3 FaceDir = Data->GetMonsterNavAgent()->GetCurrentFaceDir();

		float DotProduct = FaceDir.Dot(Dir);
		Vector3 CrossVec = FaceDir.Cross(Dir);

		if (DotProduct < 0.999999f && DotProduct > -0.999999f)
		{
			float Degree = RadianToDegree(acosf(DotProduct));

			if (CrossVec.y > 0.f)
				m_Object->AddWorldRotationY(Degree);
			else
				m_Object->AddWorldRotationY(-Degree);
		}
		else
		{
			// 평행이고 반대 방향
			if (DotProduct < -0.999999f)
			{
				if (CrossVec.y > 0)
					m_Object->AddWorldRotationY(180.f);
				else
					m_Object->AddWorldRotationY(-180.f);
			}

		}
		m_Owner->SetCurrentNode(nullptr);
		//Data->PushPhaseQueue(3);
		Data->SetAfterShoot(true);
		Data->SetCurrentShootCount(0);
	}

	return NodeResult::Node_True;
}

NodeResult CCrowBossResetShootState::OnEnd(float DeltaTime)
{
	return NodeResult();
}