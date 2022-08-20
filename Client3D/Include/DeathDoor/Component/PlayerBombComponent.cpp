
#include "PlayerBombComponent.h"
#include "ObjectPool.h"
#include "Scene/SceneManager.h"
#include "EngineUtil.h"
#include "Component/StaticMeshComponent.h"
#include "Component/LightComponent.h"
#include "Render/RenderManager.h"
#include "ProjectileComponent.h"
#include "Component/ColliderSphere.h"
#include "MonsterDataComponent.h"
#include "PlayerDataComponent.h"

CPlayerBombComponent::CPlayerBombComponent()	:
	m_Bomb(nullptr),
	m_CollisionLifeTime(0.5f),
	m_AccCollisionLifeTime(0.f),
	m_Collision(false),
	m_LiftSpeed(0.f),
	m_ShootFirstTime(true),
	m_CancleAction(false),
	m_IsClearBomb(false),
	Count(0)
{
	SetTypeID<CPlayerBombComponent>();

	m_ComponentType = Component_Type::ObjectComponent;
	m_ShootSpeed = 70.f;


	m_BeforeLift = true;
}

CPlayerBombComponent::CPlayerBombComponent(const CPlayerBombComponent& com)	:
	CObjectComponent(com)
{
}

CPlayerBombComponent::~CPlayerBombComponent()
{
}

void CPlayerBombComponent::ResetCount()
{
	Count = 0;
}

void CPlayerBombComponent::SetBomb(CGameObject* Bomb)
{
	m_Bomb = Bomb;
}

const Vector3& CPlayerBombComponent::GetBombPos() const
{
	return m_Bomb->GetWorldPos();
}

CGameObject* CPlayerBombComponent::GetBomb() const
{
	return m_Bomb;
}

void CPlayerBombComponent::OnBombProjectileDestroy(const Vector3& Pos)
{
	m_Bomb = nullptr;

	while (!m_LiftBombPathQueue.empty())
		m_LiftBombPathQueue.pop();
}

void CPlayerBombComponent::ClearLiftPathQueue()
{
	while (!m_LiftBombPathQueue.empty())
		m_LiftBombPathQueue.pop();
}

void CPlayerBombComponent::Start()
{
	CObjectComponent::Start();

}

bool CPlayerBombComponent::Init()
{

	return true;
}

void CPlayerBombComponent::Update(float DeltaTime)
{
	if (!m_Bomb)
		return;

	if (m_Collision)
	{
		m_AccCollisionLifeTime += DeltaTime;
		m_Bomb->SetWorldScale(Vector3(0.05f, 0.05f, 0.05f));

		if (m_AccCollisionLifeTime >= m_CollisionLifeTime)
		{
			ResetInfo();
		}
	}

	else if (!m_LiftBombPathQueue.empty())
	{
		Vector3 CurrentPos = m_Bomb->GetWorldPos();
		Vector3 NextPos = m_LiftBombPathQueue.front();

		if (CurrentPos.Distance(NextPos) < 0.5f)
		{
			m_LiftBombPathQueue.pop();
		}

		Vector3 MoveDir = NextPos - CurrentPos;
		MoveDir.Normalize();

		m_Bomb->AddWorldPos(MoveDir * m_LiftSpeed);

		CStaticMeshComponent* Comp = (CStaticMeshComponent*)m_Bomb->GetRootComponent();

		if (Comp)
		{
			Vector4 Emmisive = Comp->GetMaterial(0)->GetEmissiveColor();
			Comp->GetMaterial(0)->SetEmissiveColor(Emmisive.x, Emmisive.y, Emmisive.z + DeltaTime * 10.f, 1.f);
		}
	}
}

void CPlayerBombComponent::PostUpdate(float DeltaTime)
{

}

void CPlayerBombComponent::PrevRender()
{

}

void CPlayerBombComponent::Render()
{

}

void CPlayerBombComponent::PostRender()
{

}

void CPlayerBombComponent::ResetInfo()
{
	m_AccCollisionLifeTime = 0.f;
	m_Collision = false;

	if(m_Light)
		m_Light->Enable(false);

	if (m_Bomb)
	{
		CProjectileComponent* Proj = m_Bomb->FindObjectComponentFromType<CProjectileComponent>();

		// NoUpdate를 true로 안하면 Reset에서 아무리 Bomb Object를 안보이는 곳에 배치해도 다시 Projectile에서 Move로 보이는곳에 옮겨놓기 때문에 NoUpdate를 True로 해주고
		// LiftBomb 함수에서 다시 NoUpdate를 false로 되돌린다
		Proj->SetNoUpdate(true);

		m_Bomb->SetWorldScale(0.005f, 0.005f, 0.005f);
		m_Bomb->Destroy();
		m_Bomb = nullptr;
	}

	ClearLiftPathQueue();
}

CPlayerBombComponent* CPlayerBombComponent::Clone()
{
	return new CPlayerBombComponent(*this);
}

void CPlayerBombComponent::LiftBomb()
{
	m_BeforeLift = false;

	if (m_Bomb)
	{
		// 여기로 들어오면 폭탄이 충돌돼서 이펙트가 재생되고 있는 중에 또 폭탄을 쏘려 하는 것이므로 return
		m_Bomb->Reset();
		ResetInfo();
		return;
	}

	CScene* CurrentScene = CSceneManager::GetInst()->GetScene();

	m_Bomb = CObjectPool::GetInst()->GetProjectile("PlayerBomb", CurrentScene);

	if (!m_Bomb)
		return;

	m_Light = m_Bomb->FindComponentFromType<CLightComponent>();
	m_Light->Enable(false);

	Vector3 ZLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
	Vector3 YLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Y);
	Vector3 XLookDir = m_Object->GetWorldAxis(AXIS::AXIS_X);

	Vector3 Tmp = m_Object->GetWorldPos() + ZLookDir / 2.f;
	const Vector3& InitBombPos = Vector3(Tmp.x + XLookDir.x / 3.f, Tmp.y, Tmp.z);

	m_Bomb->SetWorldScale(0.005f, 0.005f, 0.005f);
	m_Bomb->SetWorldPos(InitBombPos);
	m_Bomb->FindComponentFromType<CStaticMeshComponent>()->GetMaterial(0)->SetEmissiveColor(0.f, 0.f, 0.f, 0.f);

	// Bazier 에 따라 이동할 수 있게 세팅한다.
	const Vector3& D2 = m_Bomb->GetWorldPos() + ZLookDir * 1.1f + YLookDir + 0.5f;
	const Vector3& D3 = m_Bomb->GetWorldPos() + ZLookDir * 0.7f + YLookDir * 1.7f;
	const Vector3& D4 = m_Bomb->GetWorldPos() + YLookDir * 3.6f;

	CEngineUtil::CalculateBazierTargetPoses(InitBombPos, D2, D3, D4, m_LiftBombPathQueue, 100);

	CProjectileComponent* Proj = m_Bomb->FindObjectComponentFromType<CProjectileComponent>();
	Proj->SetNoUpdate(false);

	CColliderComponent* BombCollider = m_Bomb->FindComponentFromType<CColliderSphere>();
	BombCollider->Enable(false);

	m_Object->GetScene()->GetResource()->SoundPlay("BombCharge");
}

void CPlayerBombComponent::ShootBomb(const Vector3& ShootDir)
{
	if (!m_Bomb)
	{
		m_AccCollisionLifeTime = 0.f;
		m_Collision = false;

		return;
	}

	if (!m_LiftBombPathQueue.empty())
	{
		m_Bomb->Reset();
		ResetInfo();

		return;
	}


	CProjectileComponent* Proj = m_Bomb->FindObjectComponentFromType<CProjectileComponent>();

	Vector3 BombPos = m_Bomb->GetWorldPos();

	Proj->ClearCollsionCallBack();
	Proj->ShootByLifeTimeCollision<CPlayerBombComponent>(this, &CPlayerBombComponent::OnCollision, Collision_State::Begin, 
		BombPos, ShootDir, m_ShootSpeed, 2.f);

	Proj->SetDestroy(true);
	Proj->SetEndCallBack<CPlayerBombComponent>(this, &CPlayerBombComponent::OnBombProjectileDestroy);


	if (m_ShootFirstTime)
		m_ShootFirstTime = false;

	CColliderComponent* BombCollider = m_Bomb->FindComponentFromType<CColliderSphere>();
	BombCollider->Enable(true);

	m_Object->GetScene()->GetResource()->SoundPlay("BombFire");
}

void CPlayerBombComponent::HideBomb()
{
}

void CPlayerBombComponent::OnCollision(const CollisionResult& Result)
{
	m_Collision = true;

	CGameObject* DestObject = Result.Dest->GetGameObject();
	Vector3 DestPos = DestObject->GetWorldPos();

	m_Bomb->FindObjectComponentFromType<CProjectileComponent>()->SetNoUpdate(true);

	m_Bomb->FindComponentFromType<CStaticMeshComponent>()->GetMaterial(0)->SetEmissiveColor(1.f, 1.f, 1.f, 1.f);

	m_Bomb->SetWorldPos(DestPos);

	m_Object->GetScene()->GetCameraManager()->ShakeCamera(0.3f, 1.5f);

	// 폭탄 라이트 on, 쉐이더 변경
	m_Light->Enable(true);
	m_Light->SetAtt1(0.001f);
	m_Light->SetDistance(1000.f);

	CRenderManager::GetInst()->EnableBombEffect(m_CollisionLifeTime);

	CMonsterDataComponent* Data = (CMonsterDataComponent*)DestObject->FindObjectComponent("ObjectData");

	if (DestObject->GetObjectType() == Object_Type::Monster && Data)
	{
		Data->SetIsHit(true);
		Data->DecreaseHP(30);
		Data->SetIsHit(false);
	}

	m_Object->GetScene()->GetResource()->SoundPlay("BombHit");
}

bool CPlayerBombComponent::Save(FILE* File)
{
	return CObjectComponent::Save(File);
}

bool CPlayerBombComponent::Load(FILE* File)
{
	return CObjectComponent::Load(File);
}

bool CPlayerBombComponent::SaveOnly(FILE* File)
{
	return false;
}

bool CPlayerBombComponent::LoadOnly(FILE* File)
{
	return false;
}
