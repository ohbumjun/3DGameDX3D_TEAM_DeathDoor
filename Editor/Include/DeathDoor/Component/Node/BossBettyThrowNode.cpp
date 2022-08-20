#include "BossBettyThrowNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../ProjectileComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/ColliderBox3D.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "EngineUtil.h"
#include "ObjectPool.h"
#include "../BossBettyDataComponent.h"

CBossBettyThrowNode::CBossBettyThrowNode()
{
	SetTypeID(typeid(CBossBettyThrowNode).hash_code());
}

CBossBettyThrowNode::CBossBettyThrowNode(const CBossBettyThrowNode& Node)
{
}

CBossBettyThrowNode::~CBossBettyThrowNode()
{
}

void CBossBettyThrowNode::Init()
{
	m_CallStart = true;

	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	std::string AnimName = "Throw";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// Start
	// ������ ������ Player �������� ȸ���� �� �ֵ��� �Ѵ�.
	AnimInst->AddNotify(AnimName, "OnTracePlayer", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);

	AnimInst->AddNotify(AnimName, "DisableZMove", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableMoveZ);

	// Middle
	AnimInst->AddNotify(AnimName, "MakeSnowBallAttackObj", 5, this, &CBossBettyThrowNode::MakeSnowBallAttackObj);

	// End
	AnimInst->AddNotify(AnimName, "ThrowSnowBallAttackObj", 26, this, &CBossBettyThrowNode::ThrowSnowBallAttackObj);

	AnimInst->AddNotify(AnimName, "DisableLookPlayer", 26,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);

	// AnimInst->AddNotify(AnimName, "EnableCloseAttackChangeAnim", 0,
	// 	Data, &CBossBettyDataComponent::OnBossBettyEnableCloseAttackChangeAnim);

	AnimInst->SetEndFunction(AnimName, 
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::SetCurrentNodeNull);
}

NodeResult CBossBettyThrowNode::OnStart(float DeltaTime)
{
	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Throw");

	m_Owner->SetCurrentNode(this);

	return NodeResult::Node_True;
}

NodeResult CBossBettyThrowNode::OnUpdate(float DeltaTime)
{
	if (!m_CurrentThrowBall)
		return  NodeResult::Node_True;
	
	m_ParticleMoveAccTime += DeltaTime;
	
	// �����ϴ� ȿ�� �ֱ� 
	// switch (m_SpeedChangeMethod)
	// {
	// case ParticleSpeedChangeMethod::Exponential:
	// {
	// 	m_ParticleMoveSpeed = CEngineUtil::CalculateRealTimeSpeedUsingExponentialWithBottom(m_ParticleMoveSpeedBottom, m_ParticleMoveAccTime, m_ParticleMoveInitSpeed);
	// }
	// break;
	// }
	
	// ��ġ �̵�
	float BazierMoveDist = (m_ParticleMoveDir * m_ParticleMoveSpeed * DeltaTime).Length();
	
	m_CurrentThrowBall->AddWorldPos(m_ParticleMoveDir * m_ParticleMoveSpeed * DeltaTime);
	
	m_BazierMoveCurDist += BazierMoveDist;
	
	// ��ǥ ��ġ�� ���� �̵��ߴٸ�, ���� ��ġ�� �̾Ƽ� �ش� ��ġ�� �̵��Ѵ�.
	const Vector3& CurrentWorldPos = m_CurrentThrowBall->GetWorldPos();
	
	if (m_BazierMoveTargetDist <= m_BazierMoveCurDist)
	{
		if (!m_queueBazierMovePos.empty())
		{
			Vector3 NextPos = m_queueBazierMovePos.front();
			m_queueBazierMovePos.pop();
			m_ParticleNextMovePos = NextPos;
	
			Vector3 PrevMoveDir = m_ParticleMoveDir;
	
			m_ParticleMoveDir = m_ParticleNextMovePos - m_CurrentThrowBall->GetWorldPos();
			m_ParticleMoveDir.Normalize();
	
			m_BazierMoveTargetDist = m_ParticleNextMovePos.Distance(CurrentWorldPos);
	
			m_BazierMoveCurDist = 0.f;
	
			// >> �̵� ���⿡ ����, Rotation �������ش�.
			// 1. ���� Particle Component �� �ٶ󺸴� ������, (0, 0, -1) �̴�.
			// 2. ������, ���� ó�� Particle Component ���� ���ϴ� ������, y �� 1 ���� (0, 1, 0)
			// - ����, �⺻������ m_ParticleMoveDir �� ���ؼ�, X �� ����, 90�� ȸ���� �⺻������ ����� �Ѵ�.
			// 3. �Ӹ� �ƴ϶�, Particle ������ ���� Rot Angle �� �ִ�. �̰��� ��ġ Offset ���� ó�� ������ ���̴�.
			// 4. ���� Dir, ���� Dir ���� Angle �� ���ϰ�, �̰͸�ŭ Particle Dir �� ȸ����ų ���̴�.
			// - ���� ���, X �� ���� ���������� ���ٰ�, Y �� ���� �������� ���ٴ� ����, ���� Angle �� Z �� ���� 90 �� ȸ��
			// float RotAngle = m_ParticleMoveDir.Angle(PrevMoveDir);
			// const Vector3& RotAxis = PrevMoveDir.Cross(m_ParticleMoveDir);
			// 
			// if (XMVector3Equal(RotAxis.Convert(), XMVectorZero()) == false)
			// {
			// 	XMVECTOR Qut = XMQuaternionRotationAxis(RotAxis.Convert(), RotAngle);
			// 
			// 	const Vector3& EulerRotAngle = CEngineUtil::QuarternionToEulerAngles(Qut);
			// 
			// 	AddWorldRotation(EulerRotAngle);
			// }
		}
		else
		{
			m_ParticleMoveDir = Vector3(0.f, 0.f, 0.f);
		}
	}

	return NodeResult::Node_True;
}

NodeResult CBossBettyThrowNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

// Snow Ball Attack Obj ����� -> Socket ���·� �޾Ƴ���
void CBossBettyThrowNode::MakeSnowBallAttackObj()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// �Ʒ��� ����, Component �� ��� �ִ�, Ư�� Object �� ���� ���̴�.
	// - Project Tile
	// - Mesh
	// - Particle
	// - Collider Component
	// - �װ���, ������ ���̴�. (�ش� ������ BossBettyDataComponent �� ��� �ְ� �� ���̴�)
	CScene* CurrentScene = CSceneManager::GetInst()->GetScene();

	m_CurrentThrowBall = CObjectPool::GetInst()->GetProjectile("BossBettySnowAttackParticle", CurrentScene);
	
	// Debug ��
	if (m_CurrentThrowBall == nullptr)
	{
		m_CurrentThrowBall = CObjectPool::GetInst()->GetProjectile("BossBettySnowAttackParticle", CurrentScene);
		return;
	}

	// Data->SetBettyThrowBallObject(m_CurrentThrowBall);

	m_CurrentThrowBall->Enable(true);

	if (m_CurrentThrowBall->GetLifeSpan() > 0.f)
		assert(false);
		
	// Particle Component �� ã�Ƽ�, Bazier �̵��� ��Ų��.
	CParticleComponent* ParticleComp = m_CurrentThrowBall->FindComponentFromType<CParticleComponent>();

	// Betty �ٷ� �տ� �����Ѵ�.
	Vector3 ZLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
	Vector3 YLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Y);

	const Vector3& InitBallPos = m_Object->GetWorldPos() + ZLookDir * 7.f + YLookDir * 6.f;

	m_CurrentThrowBall->SetWorldPos(InitBallPos);

	// Bazier �� ���� �̵��� �� �ְ� �����Ѵ�.
	const Vector3& D2 = m_Object->GetWorldPos() + ZLookDir * 9.f + YLookDir * 10.f;
	const Vector3& D3 = m_Object->GetWorldPos() + ZLookDir * 3.f + YLookDir * 16.f;
	const Vector3& D4 = m_Object->GetWorldPos() - ZLookDir * 2.f + YLookDir * 14.f;

	m_ParticleMoveSpeed = 27.f;
	
	m_BazierMoveEffect = true;

	CEngineUtil::CalculateBazierTargetPoses(InitBallPos, D2, D3, D4, m_queueBazierMovePos, 50);

	// ó�� �Ѱ��� �̾Ƶд�.
	if (!m_queueBazierMovePos.empty())
	{
		Vector3 NextPos = m_queueBazierMovePos.front();
		m_queueBazierMovePos.pop();

		m_ParticleNextMovePos = NextPos;

		m_ParticleMoveDir = m_ParticleNextMovePos - m_CurrentThrowBall->GetWorldPos();
		m_ParticleMoveDir.Normalize();

		const Vector3& CurrentWorldPos = m_CurrentThrowBall->GetWorldPos();

		m_BazierMoveTargetDist = m_ParticleNextMovePos.Distance(CurrentWorldPos);

		m_BazierMoveCurDist = 0.f;

		m_ParticleMoveAccTime = 0.f;

		m_ParticleMoveInitSpeed = m_ParticleMoveSpeed;
	}

	// Sound
	Data->OnBossBettyMakeBallSound();
}

// Snow Ball Attack Obj �� Player �������� ������ 
void CBossBettyThrowNode::ThrowSnowBallAttackObj()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// BossBettyDataComponent �κ���, BossThrowBall Object �� ��������
	// �ű⿡ ������ �������� ���⿡ ���� ���̴�.
	// m_CurrentThrowBall = Data->GetBossBettyThrowObject();

	CProjectileComponent* ProjTileComp = m_CurrentThrowBall->FindComponentFromType<CProjectileComponent>();

	// Attack After Effect
	CGameObject* AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettyAttackAfterEffect", CSceneManager::GetInst()->GetScene());

	AfterEffectParticle->Enable(false);
	
	CColliderBox3D* Collider3D = AfterEffectParticle->FindComponentFromType<CColliderBox3D>();

	Collider3D->AddCollisionCallback(Collision_State::Begin, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnHitMeleeAttack);

	Collider3D->SetExtent(2.5f, 2.f, 2.5f);

	Collider3D->SetEnablePossibleTime(AfterEffectParticle->GetLifeSpan() * 0.2f);

	const Vector3& PlayerPos = CSceneManager::GetInst()->GetScene()->GetPlayerObject()->GetWorldPos();

	// Throw Particlee
	ProjTileComp->ShootByTargetPos(m_CurrentThrowBall->GetWorldPos(), 50.f, PlayerPos + Vector3(0.f, 2.f, 0.f), AfterEffectParticle);

	// CallBack Sound
	ProjTileComp->SetEndCallBack(Data, &CBossBettyDataComponent::OnBossBettyThrowBallLandSound);
		
	// Throw Attack Enable �� �ٽ� False �� �ٲ��ش�
	Data->SetThrowAttackEnable(false);

	// ���̻� ThrowBall �� ThrowNode ������ �������� �ʰ� �Ѵ�.
	m_CurrentThrowBall = nullptr;

	// Sound
	Data->OnBossBettyThrowBallSound();
}
