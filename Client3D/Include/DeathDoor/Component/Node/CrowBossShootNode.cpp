
#include "CrowBossShootNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../CrowBossDataComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"
#include "DeathNode.h"

CCrowBossShootNode::CCrowBossShootNode()
{
	SetTypeID(typeid(CCrowBossShootNode).hash_code());
}

CCrowBossShootNode::CCrowBossShootNode(const CCrowBossShootNode& Node) :
	CActionNode(Node)
{
}

CCrowBossShootNode::~CCrowBossShootNode()
{
}

NodeResult CCrowBossShootNode::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->GetHP() <= 0)
	{
		Data->ClearPhaseQueue();

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Death");

		m_Owner->GetOwner()->SetTreeUpdate(false);

		m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Death");

		return NodeResult::Node_False;
	}

	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("SlidingReady");

	return NodeResult::Node_True;
}

NodeResult CCrowBossShootNode::OnUpdate(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->GetHP() <= 0)
	{
		Data->ClearPhaseQueue();

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Death");

		m_Owner->GetOwner()->SetTreeUpdate(false);

		m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Death");

		return NodeResult::Node_False;
	}

	Vector3 FaceDir = Data->GetMonsterNavAgent()->GetCurrentFaceDir();


	if (!Data->GetShootDirFixed())
	{
		CGameObject* Player = m_Object->GetScene()->GetPlayerObject();
		Vector3 PlayerPos = Player->GetWorldPos();
		Vector3 MyPos = m_Object->GetWorldPos();
		Vector3 Dir = PlayerPos - MyPos;
		Dir.Normalize();

		float DotProduct = Dir.Dot(FaceDir);

		if (DotProduct > -0.99999f && DotProduct < 0.99999f)
		{
			float Degree = RadianToDegree(acosf(DotProduct));
			if (Degree > 90.f || Degree < -90.f)
			{
				Vector3 CrossVec = FaceDir.Cross(Dir);

				if (CrossVec.y > 0.f)
				{
					m_Object->AddWorldRotationY(Degree);
				}

				else
				{
					m_Object->AddWorldRotationY(-Degree);
				}

				Data->GetMonsterNavAgent()->ForceUpdateFaceDir();
			}

		}

		Data->SetShootDirFixed(true);
	}

	// 회전했을수도 있으니 FaceDir 새로 갱신
	FaceDir = Data->GetMonsterNavAgent()->GetCurrentFaceDir();
	Data->SetShootState(CrowBossShootState::Shoot);
	Data->ShootChain(FaceDir, DeltaTime);

	return NodeResult::Node_True;
}

NodeResult CCrowBossShootNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}