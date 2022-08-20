
#include "CrowBossSpinNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../CrowBossDataComponent.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"
#include "Component/ColliderBox3D.h"
#include "DeathNode.h"

CCrowBossSpinNode::CCrowBossSpinNode()	:
	m_AccRotation(0.f),
	m_SpinDegree(0.f),
	m_AccSlidingTime(0.f),
	m_RotateCW(true)
{
	SetTypeID(typeid(CCrowBossSpinNode).hash_code());
}

CCrowBossSpinNode::CCrowBossSpinNode(const CCrowBossSpinNode& Node) :
	CActionNode(Node)
{
}

CCrowBossSpinNode::~CCrowBossSpinNode()
{
}

void CCrowBossSpinNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();
}

NodeResult CCrowBossSpinNode::OnStart(float DeltaTime)
{
	m_CallStart = true;

	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->GetHP() <= 0)
	{
		Data->ClearPhaseQueue();

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Death");

		m_Owner->GetOwner()->SetTreeUpdate(false);

		m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Death");

		return NodeResult::Node_False;
	}

	Vector3 MyOriginPos = Data->GetMyOriginPos();
	Vector3 PlayerOriginPos = Data->GetPlayerOriginPos();
	CNavAgent* Agent = Data->GetMonsterNavAgent();
	Vector3 FaceDir = Agent->GetCurrentFaceDir();
	Vector3 OriginDir = PlayerOriginPos - MyOriginPos;

	OriginDir.Normalize();

	Vector3 MyCurrentPos = m_Object->GetWorldPos();
	Vector3 PlayerCurrentPos = m_Object->GetScene()->GetPlayerObject()->GetWorldPos();
	
	Vector3 CurrentDir = PlayerCurrentPos - MyCurrentPos;
	CurrentDir.Normalize();

	Vector3 Cross = FaceDir.Cross(CurrentDir);

	m_RotateCW = true;
	m_SpinDegree = 0.f;
	if (Cross.y < 0)
	{
		m_RotateCW = false;
	}

	if (m_RotateCW)
		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("RightSpin");
	else
		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("LeftSpin");

	Vector3 SpinDir = PlayerCurrentPos - MyCurrentPos;
	SpinDir.Normalize();

	float DotProduct = FaceDir.Dot(SpinDir);

	if (DotProduct < 0.9999f && DotProduct > -0.9999f)
	{
		m_SpinDegree = RadianToDegree(acosf(DotProduct));

		Vector3 CrossVec = FaceDir.Cross(SpinDir);

		if (CrossVec.y < 0.f)
			m_SpinDegree *= -1.f;
	}

	else
	{
		if (m_RotateCW)
			m_SpinDegree = 180.f;
		else
			m_SpinDegree = -180.f;
	}

	Data->GetMeleeAttackCollider()->Enable(true);

	return NodeResult::Node_True;
}

NodeResult CCrowBossSpinNode::OnUpdate(float DeltaTime)
{
	m_Owner->SetCurrentNode(this);

	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->GetHP() <= 0)
	{
		Data->ClearPhaseQueue();

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Death");

		m_Owner->GetOwner()->SetTreeUpdate(false);

		return NodeResult::Node_False;
	}

	Vector3 MyOriginPos = Data->GetMyOriginPos();
	Vector3 PlayerOriginPos = Data->GetPlayerOriginPos();
	Vector3 OriginDir = PlayerOriginPos - MyOriginPos;
	OriginDir.Normalize();

	if (abs(m_AccRotation - m_SpinDegree) < 2.f)
	{
		if (m_AccSlidingTime > 1.f)
		{
			m_AccRotation = 0.f;
			m_AccSlidingTime = 0.f;
			m_Owner->SetCurrentNode(nullptr);
			m_CallStart = false;

			m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Run");

			return NodeResult::Node_True;
		}

		else
		{
			m_AccSlidingTime += DeltaTime;
			m_Object->AddWorldPos(OriginDir * 4.f * DeltaTime);
		}

		return NodeResult::Node_True;
	}

	m_Object->AddWorldPos(OriginDir * 6.f * DeltaTime);

	CNavAgent* Agent = Data->GetMonsterNavAgent();
	Vector3 FaceDir = Agent->GetCurrentFaceDir();

	if (m_SpinDegree > 0.f)
	{
		m_Object->AddWorldRotationY(180.f * DeltaTime);
		m_AccRotation += 180.f * DeltaTime;
	}

	else
	{
		m_Object->AddWorldRotationY(-180.f * DeltaTime);
		m_AccRotation += -180.f * DeltaTime;
	}
	

	return NodeResult::Node_True;
}

NodeResult CCrowBossSpinNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
