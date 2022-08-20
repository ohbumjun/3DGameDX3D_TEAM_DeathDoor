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
	// 던지기 전까지 Player 방향으로 회전할 수 있도록 한다.
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
	
	// 급증하는 효과 주기 
	// switch (m_SpeedChangeMethod)
	// {
	// case ParticleSpeedChangeMethod::Exponential:
	// {
	// 	m_ParticleMoveSpeed = CEngineUtil::CalculateRealTimeSpeedUsingExponentialWithBottom(m_ParticleMoveSpeedBottom, m_ParticleMoveAccTime, m_ParticleMoveInitSpeed);
	// }
	// break;
	// }
	
	// 위치 이동
	float BazierMoveDist = (m_ParticleMoveDir * m_ParticleMoveSpeed * DeltaTime).Length();
	
	m_CurrentThrowBall->AddWorldPos(m_ParticleMoveDir * m_ParticleMoveSpeed * DeltaTime);
	
	m_BazierMoveCurDist += BazierMoveDist;
	
	// 목표 위치로 거의 이동했다면, 다음 위치를 뽑아서 해당 위치로 이동한다.
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
	
			// >> 이동 방향에 따라, Rotation 적용해준다.
			// 1. 실제 Particle Component 가 바라보는 방향은, (0, 0, -1) 이다.
			// 2. 하지만, 실제 처음 Particle Component 들이 향하는 방향은, y 축 1 방향 (0, 1, 0)
			// - 따라서, 기본적으로 m_ParticleMoveDir 에 대해서, X 축 기준, 90도 회전을 기본적으로 해줘야 한다.
			// 3. 뿐만 아니라, Particle 각각에 대한 Rot Angle 이 있다. 이것이 마치 Offset 각도 처럼 동작할 것이다.
			// 4. 이전 Dir, 현재 Dir 간의 Angle 을 구하고, 이것만큼 Particle Dir 을 회전시킬 것이다.
			// - 예를 들어, X 축 기준 오른쪽으로 가다가, Y 축 기준 위쪽으로 간다는 것은, 실제 Angle 이 Z 축 기준 90 도 회전
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

// Snow Ball Attack Obj 만들기 -> Socket 형태로 달아놓기
void CBossBettyThrowNode::MakeSnowBallAttackObj()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// 아래와 같은, Component 를 들고 있는, 특정 Object 를 만들어낼 것이다.
	// - Project Tile
	// - Mesh
	// - Particle
	// - Collider Component
	// - 그것을, 가져올 것이다. (해당 정보는 BossBettyDataComponent 에 들고 있게 할 것이다)
	CScene* CurrentScene = CSceneManager::GetInst()->GetScene();

	m_CurrentThrowBall = CObjectPool::GetInst()->GetProjectile("BossBettySnowAttackParticle", CurrentScene);
	
	// Debug 용
	if (m_CurrentThrowBall == nullptr)
	{
		m_CurrentThrowBall = CObjectPool::GetInst()->GetProjectile("BossBettySnowAttackParticle", CurrentScene);
		return;
	}

	// Data->SetBettyThrowBallObject(m_CurrentThrowBall);

	m_CurrentThrowBall->Enable(true);

	if (m_CurrentThrowBall->GetLifeSpan() > 0.f)
		assert(false);
		
	// Particle Component 를 찾아서, Bazier 이동을 시킨다.
	CParticleComponent* ParticleComp = m_CurrentThrowBall->FindComponentFromType<CParticleComponent>();

	// Betty 바로 앞에 생성한다.
	Vector3 ZLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
	Vector3 YLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Y);

	const Vector3& InitBallPos = m_Object->GetWorldPos() + ZLookDir * 7.f + YLookDir * 6.f;

	m_CurrentThrowBall->SetWorldPos(InitBallPos);

	// Bazier 에 따라 이동할 수 있게 세팅한다.
	const Vector3& D2 = m_Object->GetWorldPos() + ZLookDir * 9.f + YLookDir * 10.f;
	const Vector3& D3 = m_Object->GetWorldPos() + ZLookDir * 3.f + YLookDir * 16.f;
	const Vector3& D4 = m_Object->GetWorldPos() - ZLookDir * 2.f + YLookDir * 14.f;

	m_ParticleMoveSpeed = 27.f;
	
	m_BazierMoveEffect = true;

	CEngineUtil::CalculateBazierTargetPoses(InitBallPos, D2, D3, D4, m_queueBazierMovePos, 50);

	// 처음 한개를 뽑아둔다.
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

// Snow Ball Attack Obj 를 Player 방향으로 던지기 
void CBossBettyThrowNode::ThrowSnowBallAttackObj()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// BossBettyDataComponent 로부터, BossThrowBall Object 를 가져오고
	// 거기에 적절한 설정들을 여기에 해줄 것이다.
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
		
	// Throw Attack Enable 을 다시 False 로 바꿔준다
	Data->SetThrowAttackEnable(false);

	// 더이상 ThrowBall 을 ThrowNode 내에서 움직이지 않게 한다.
	m_CurrentThrowBall = nullptr;

	// Sound
	Data->OnBossBettyThrowBallSound();
}
