
#include "CrowBossRunNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../CrowBossDataComponent.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"
#include "Component/ColliderBox3D.h"

CCrowBossRunNode::CCrowBossRunNode()
{
	SetTypeID(typeid(CCrowBossRunNode).hash_code());
}

CCrowBossRunNode::CCrowBossRunNode(const CCrowBossRunNode& Node)	:
	CActionNode(Node)
{
}

CCrowBossRunNode::~CCrowBossRunNode()
{
}

void CCrowBossRunNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CCrowBossRunNode::OnStart(float DeltaTime)
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

	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Run");

	return NodeResult::Node_True;
}

NodeResult CCrowBossRunNode::OnUpdate(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->GetHP() <= 0)
	{
		Data->ClearPhaseQueue();

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Death");

		m_Owner->GetOwner()->SetTreeUpdate(false);

		return NodeResult::Node_False;
	}

	Data->GetMeleeAttackCollider()->Enable(false);
	CNavAgent* Agent = Data->GetMonsterNavAgent();
	Vector3 PlayerCurrentPos = m_Object->GetScene()->GetPlayerObject()->GetWorldPos();
	Vector3 MyCurrentPos = m_Object->GetWorldPos();

	Vector3 RunDir = PlayerCurrentPos - MyCurrentPos;
	RunDir.Normalize();

	Vector3 FaceDir = Agent->GetCurrentFaceDir();

	float DotProduct = FaceDir.Dot(RunDir);

	if (DotProduct < 0.999999f && DotProduct > -0.999999f)
	{
		float Degree = RadianToDegree(acosf(DotProduct));

		Vector3 CrossVec = FaceDir.Cross(RunDir);

		if(CrossVec.y > 0.f)
			m_Object->AddWorldRotationY(90.f * DeltaTime);
		else
			m_Object->AddWorldRotationY(-90.f * DeltaTime);
	}
	else
	{
		// 평행이고 같은 방향
		if (DotProduct > 0.999999f)
		{
			m_Object->AddWorldRotationY(120.f * DeltaTime);
		}
	}

	
	float Speed = Data->GetMoveSpeed();

	int Frame = m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationSequence()->GetCurrentFrameIdx();

	if (Frame >= 8 && Frame <= 17)
	{
		Speed *= 2.f;
	}

	m_Object->AddWorldPos(FaceDir * Speed * DeltaTime);

	Data->GetMeleeAttackCollider()->Enable(true);

	return NodeResult::Node_True;
}

NodeResult CCrowBossRunNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
