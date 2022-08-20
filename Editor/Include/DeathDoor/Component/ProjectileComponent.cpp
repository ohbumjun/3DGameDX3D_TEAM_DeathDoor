#include "ProjectileComponent.h"
#include "GameObject/GameObject.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Component/ParticleComponent.h"
#include "EngineUtil.h"

CProjectileComponent::CProjectileComponent()	:
	// m_Destroy(false),
	m_Destroy(true), // OBJ가 true 로 바꿈
	m_NoUpdate(false),
	m_MoveBazier(false),
	m_SpeedChangeMethod(ProjectTileSpeedChangeMethod::None)
{
	SetTypeID<CProjectileComponent>();
}

CProjectileComponent::CProjectileComponent(const CProjectileComponent& com)	:
	CObjectComponent(com)
{
	m_Root = m_Object->GetRootComponent();
	m_Collider = m_Object->FindComponentFromType<CColliderSphere>();

	if (!m_Collider)
	{
		m_Collider = m_Object->FindComponentFromType<CColliderBox3D>();
	}

	// TODO : End Particle Pool에서 찾기
	if (com.m_EndParticleObject)
	{
	}
}

CProjectileComponent::~CProjectileComponent()
{
}

bool CProjectileComponent::Init()
{
	return true;
}

void CProjectileComponent::Start()
{
	CObjectComponent::Start();

	m_Root = m_Object->GetRootComponent();
	m_Collider = m_Object->FindComponentFromType<CColliderSphere>();

	if (!m_Collider)
	{
		m_Collider = m_Object->FindComponentFromType<CColliderBox3D>();
	}
}

void CProjectileComponent::Update(float DeltaTime)
{
	if (m_NoUpdate)
		return;

	if (m_IsShoot)
	{
		UpdateSpeed();

		m_LifeTimer += DeltaTime;

		CheckDestroy();

		Vector3 Move;

		if (m_IsGravity)
		{
			Move = Vector3(m_Dir.x * m_VelocityXZ * m_LifeTimer,
				(((m_VelocityY * m_LifeTimer) - ((m_Gravity * m_LifeTimer * m_LifeTimer) / 2.f))),
				m_Dir.z * m_VelocityXZ * m_LifeTimer);

			m_Root->SetWorldPos(m_StartPos + Move);
		}
		else if (m_MoveBazier)
		{
			float BazierMoveDist = (m_Dir * m_Speed * DeltaTime).Length();
			m_Root->AddWorldPos(Move);

			const Vector3& CurrentPos = m_Root->GetWorldPos();

			m_BazierMoveCurAccDist += BazierMoveDist;

			// 새롭게 Bazier 에서 뽑아낸다.
			if (m_BazierMoveTargetDist <= m_BazierMoveCurAccDist)
			{
				Vector3 NextPos = m_queueBazierMovePos.front();
				m_queueBazierMovePos.pop();

				Vector3 PrevMoveDir = m_Dir;

				m_Dir = NextPos - CurrentPos;
				m_Dir.Normalize();

				m_BazierMoveTargetDist = NextPos.Distance(CurrentPos);

				m_BazierMoveCurAccDist = 0.f;
			}
		}
		else
		{
			Move = m_Dir * m_Speed * DeltaTime;
			m_Root->AddWorldPos(Move);
		}
	}
}

void CProjectileComponent::PostUpdate(float DeltaTime)
{
}

void CProjectileComponent::PrevRender()
{
}

void CProjectileComponent::Reset()
{
	CObjectComponent::Reset();

	m_IsShoot = false;
	m_LifeTimer = 0.f;
	m_LifeTime = 0.f;
	m_Speed = 0.f;
}

void CProjectileComponent::Render()
{
}

void CProjectileComponent::PostRender()
{
}

CObjectComponent* CProjectileComponent::Clone()
{
	return new CProjectileComponent(*this);
}

bool CProjectileComponent::Save(FILE* File)
{
	CObjectComponent::Save(File);

	return true;
}

bool CProjectileComponent::Load(FILE* File)
{
	CObjectComponent::Load(File);

	return true;
}

bool CProjectileComponent::SaveOnly(FILE* File)
{
	CObjectComponent::SaveOnly(File);

	return true;
}

bool CProjectileComponent::LoadOnly(FILE* File)
{
	CObjectComponent::LoadOnly(File);

	return true;
}

void CProjectileComponent::ShootByTargetPos(const Vector3& StartPos, float Speed, const Vector3& TargetPos, CGameObject* EndParticleObj)
{
	m_LifeTimer = 0.f;
	m_IsShoot = true;
	m_StartPos = StartPos;
	m_Speed = Speed;
	m_TargetPos = TargetPos;
	m_IsGravity = false;
	m_EndParticleObject = EndParticleObj;
	m_Dir = TargetPos - StartPos;
	m_Dir.Normalize();
	m_Root->SetWorldPos(StartPos);
}

void CProjectileComponent::ShootByLifeTime(const Vector3& StartPos, const Vector3& Dir, float Speed, float LifeTime, CGameObject* EndParticleObj)
{
	m_LifeTimer = 0.f;
	m_IsShoot = true;
	m_StartPos = StartPos;
	m_Dir = Dir;	
	m_Speed = Speed;
	m_LifeTime = LifeTime;
	m_IsGravity = false;
	m_EndParticleObject = EndParticleObj;
	m_Dir.Normalize();
	m_Root->SetWorldPos(StartPos);
}

void CProjectileComponent::ShootByGravityTargetPos(const Vector3& StartPos, const Vector3& XZDir, float Angle, 
	const Vector3& TargetPos, float Gravity, CGameObject* EndParticleObj)
{
	m_LifeTimer = 0.f;
	m_IsShoot = true;
	m_IsGravity = true;
	m_StartPos = StartPos;
	m_Dir = XZDir;	
	m_TargetPos = TargetPos;
	m_EndParticleObject = EndParticleObj;
	m_Gravity = Gravity;
	m_Dir.Normalize();

	Vector3 StartXZ = Vector3(StartPos.x, 0.f, StartPos.z);
	Vector3 TargetXZ = Vector3(TargetPos.x, 0.f, TargetPos.z);
	float DistanceXZ = StartXZ.Distance(TargetXZ);
	float Velocity = sqrtf((DistanceXZ * Gravity) / sinf(DegreeToRadian(2.f * Angle)));
	m_VelocityXZ = Velocity * cosf(DegreeToRadian(Angle));
	m_VelocityY = Velocity * sinf(DegreeToRadian(Angle));
	m_LifeTime = (2 * Velocity * sinf(DegreeToRadian(Angle))) / Gravity;
	m_LifeTime += 0.5f;
}

void CProjectileComponent::ShootByTargetPosWithBazierMove(const Vector3& StartPos, const Vector3& D2, 
	const Vector3& D3, const Vector3& TargetPos, float InitSpeed, CGameObject* EndParticleObj)
{
	m_LifeTimer = 0.f;
	m_IsShoot = true;
	m_StartPos = StartPos;
	m_Speed = InitSpeed;
	m_TargetPos = TargetPos;
	m_IsGravity = false;
	m_EndParticleObject = EndParticleObj;

	m_Root->SetWorldPos(StartPos);

	m_MoveBazier = true;

	// m_queueBazierMovePos
	CEngineUtil::CalculateBazierTargetPoses(StartPos, D2, D3, TargetPos, m_queueBazierMovePos, 100);

	if (!m_queueBazierMovePos.empty())
	{
		Vector3 NextPos = m_queueBazierMovePos.front();
		m_queueBazierMovePos.pop();

		m_Dir = NextPos - StartPos;
		m_Dir.Normalize();

		m_BazierMoveTargetDist = NextPos.Distance(StartPos);

		m_BazierMoveCurAccDist = 0.f;

		m_BazierMoveAccTime = 0.f;

		// m_InitSpeed = m_Speed;
	}
}


void CProjectileComponent::ClearCollsionCallBack()
{
	if (m_Collider)
	{
		m_Collider->ClearCollisionCallBack();
	}
}

bool CProjectileComponent::CheckDestroy()
{
	// 중력이 적용된 경우
	if (m_IsGravity)
	{
		if (m_Root->GetWorldPos().y < m_TargetPos.y)
		{
			OnEnd();
			return true;
		}
	}
	// Bazier 로 이동시키는 경우
	else if (m_MoveBazier)
	{
		if (m_queueBazierMovePos.empty())
		{
			// 더이상 움직이지 않게 한다.
			m_Dir = Vector3(0.f, 0.f, 0.f);
			OnEnd();
			return true;
		}
	}
	// LifeTime으로 삭제를 관리하는 경우
	else if (m_LifeTime != 0.f)
	{
		if (m_LifeTimer >= m_LifeTime)
		{
			//if (m_NoDestroy)
			//	return false;

			OnEnd();
			return true;
		}
	}
	// Target Position도착 여부로 삭제를 관리하는 경우
	else
	{
		if (!m_Destroy)
			return false;

		Vector3 MyPos = m_Root->GetWorldPos();
		Vector3 ToTarget = m_TargetPos - MyPos;

		float Dot = ToTarget.Dot(m_Dir);

		if (Dot < 0)
		{
			OnEnd();
			return true;
		}
	}

	return false;
}

void CProjectileComponent::OnEnd()
{
	// End Effect가 있는 경우
	if (m_EndParticleObject)
	{
		m_EndParticleObject->StartParticle(m_Root->GetWorldPos());
	}

	if (m_EndCallBack)
	{
		Vector3 Pos = m_Root->GetWorldPos();
		m_EndCallBack(Pos);
	}

	// TODO : Projectile Destroy처리 확정된 이후 변경
	if(m_Destroy)
		m_Object->Destroy();

	m_NoUpdate = false;
}

void CProjectileComponent::UpdateSpeed()
{
	switch (m_SpeedChangeMethod)
	{
	case ProjectTileSpeedChangeMethod::Exponential :
	{
		// m_Speed = CEngineUtil::CalculateRealTimeSpeedUsingExponentialWithSpeed();
	}
	break;

	default:
		break;
	}
}
