
#include "PlayerBowComponent.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "PlayerDataComponent.h"
#include "ObjectPool.h"
#include "ArrowComponent.h"
#include "Component/ColliderSphere.h"
#include "Component/ParticleComponent.h"
#include "MonsterDataComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "ArrowCollisionFireCollider.h"
#include "../DDUtil.h"

CPlayerBowComponent::CPlayerBowComponent()	:
	m_PlayerData(nullptr),
	m_Arrow(nullptr),
	m_Destroy(false),
	m_ShowBow(false)
{
	m_ComponentType = Component_Type::SceneComponent;
	SetTypeID<CPlayerBowComponent>();
}

CPlayerBowComponent::CPlayerBowComponent(const CPlayerBowComponent& com)	:
	CStaticMeshComponent(com)
{
}

CPlayerBowComponent::~CPlayerBowComponent()
{
}

void CPlayerBowComponent::Start()
{
	CStaticMeshComponent::Start();

	if (!m_Mesh)
	{
		const PathInfo* Info = CPathManager::GetInst()->FindPath(MESH_PATH);

		char FullPath[MAX_PATH] = {};
		strcpy_s(FullPath, Info->PathMultibyte);
		strcat_s(FullPath, "BOW.msh");

		m_Scene->GetResource()->LoadMeshFullPathMultibyte(Mesh_Type::Static, "Bow", FullPath);

		SetMesh("Bow");

		SetWorldPos(Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
		SetWorldScale(Vector3(0.f, 0.f, 0.f));
		SetWorldRotation(90.f, 0.f, -180.f);

		m_Render = false;
	}

	m_PlayerData = m_Object->FindObjectComponentFromType<CPlayerDataComponent>();

	CAnimationSequenceInstance* AnimInst = dynamic_cast<CAnimationMeshComponent*>(m_Object->GetRootComponent())->GetAnimationInstance();

	float FrameTime = AnimInst->GetAnimationFrameTime("PlayerArrow");
	float FrameLength = AnimInst->GetAnimationFrameLength("PlayerArrow");

	m_EmvMaxTime = FrameTime * FrameLength;
}

bool CPlayerBowComponent::Init()
{
	if (!CStaticMeshComponent::Init())
		return false;

	return true;
}

void CPlayerBowComponent::Update(float DeltaTime)
{
	CStaticMeshComponent::Update(DeltaTime);

	if (m_Render)
	{
		m_EmvTimer += DeltaTime;

		if (m_EmvTimer >= m_EmvMaxTime)
		{
			m_EmvTimer = m_EmvMaxTime;
		}

		Vector4 Emv = CDDUtil::LerpColor(Vector4(0.f, 0.f, 0.f, 1.f), Vector4(1.f, 0.f, 0.f, 1.f), m_EmvTimer, m_EmvMaxTime);
		SetEmissiveColor(Emv);
	}
}

void CPlayerBowComponent::PostUpdate(float DeltaTime)
{
	CStaticMeshComponent::PostUpdate(DeltaTime);
}

void CPlayerBowComponent::PrevRender()
{
	CStaticMeshComponent::PrevRender();

	if (m_Destroy)
	{
		if(m_Arrow)
			m_Arrow->Destroy();
		m_Destroy = false;
	}
}

void CPlayerBowComponent::Render()
{
	CStaticMeshComponent::Render();
}

void CPlayerBowComponent::PostRender()
{
	CStaticMeshComponent::PostRender();
}

CPlayerBowComponent* CPlayerBowComponent::Clone()
{
	return new CPlayerBowComponent(*this);
}

void CPlayerBowComponent::ShowBow(const Vector3& ShootDir)
{
	// 네비메쉬 밖을 피킹할 경우 예외 처리
	if (ShootDir.x == 0.f && ShootDir.y == 0.f && ShootDir.z == 0.f)
	{
		return;
	}

	Vector3 Pos = m_Object->GetWorldPos();
	Vector3 BowPos = Vector3(Pos.x + ShootDir.x, Pos.y + 2.f, Pos.z + ShootDir.z);

	// (1, 0, 0)은 화살의 방향
	float DotProduct = ShootDir.Dot(Vector3(1.f, 0.f, 0.f));
	float YDegree = 0.f;

	if (DotProduct < 0.999999999f && DotProduct > -0.999999999f)
	{
		float Degree = RadianToDegree(acosf(DotProduct));
		YDegree = Degree;

		if (ShootDir.Cross(Vector3(1.f, 0.f, 0.f)).y > 0.f)
			YDegree *= -1.f;
	}

	else
	{
		if (ShootDir.x > 0.f)
			YDegree = 0.f;
		else
			YDegree = 180.f;
	}

	SetWorldPos(BowPos);
	SetWorldScale(Vector3(0.06f, 0.06f, 0.06f));
	SetWorldRotation(90.f, YDegree, -180.f);

	m_Render = true;
	m_ShowBow = true;
}

void CPlayerBowComponent::ShootArrow(const Vector3& ShootDir)
{
	Vector3 MyPos = m_Object->GetWorldPos();

	m_Arrow = CObjectPool::GetInst()->GetProjectile("PlayerArrow", m_Scene);

	if (!m_Arrow)
		return;

	// Particle, Light Component 는 모두 False 로 처리해준다.
	std::vector<CParticleComponent*> vecArrowParticles;
	m_Arrow->FindAllSceneComponentFromType<CParticleComponent>(vecArrowParticles);

	size_t ParticleSize = vecArrowParticles.size();

	for (size_t i = 0; i < ParticleSize; ++i)
	{
		vecArrowParticles[i]->CRef::Enable(false);
	}

	CLightComponent* ArrowLight = m_Arrow->FindComponentFromType<CLightComponent>();
	ArrowLight->CRef::Enable(false);

	// Arrow 발사
	m_Arrow->SetWorldPos(MyPos.x + ShootDir.x, MyPos.y + 2.f, MyPos.z + ShootDir.z);

	CArrowComponent* Comp = m_Arrow->FindObjectComponentFromType<CArrowComponent>();

	Vector3 InitialDir = Vector3(0.f, 0.f, 1.f);

	float DotProduct = InitialDir.Dot(ShootDir);
	
	if (DotProduct >= -0.999999f && DotProduct <= 0.999999f)
	{
		float Degree = RadianToDegree(acosf(DotProduct));

		Vector3 CrossVector = InitialDir.Cross(ShootDir);

		if (CrossVector.y < 0.f)
			Degree *= -1.f;

		m_Arrow->SetWorldRotationY(Degree);

	}

	Vector3 ArrowStartPos = m_Arrow->GetWorldPos();
	//Comp->ShootByLifeTime(MyPos, ShootDir, 20.f, 2.5f);
	Comp->ClearCollsionCallBack();
	Comp->ShootByLifeTimeCollision<CPlayerBowComponent>(this, &CPlayerBowComponent::OnCollision, Collision_State::Begin, 
		ArrowStartPos, ShootDir, 60.f, 2.5f);
	Comp->SetDestroy(true);

	m_EmvTimer = 0.f;
}

void CPlayerBowComponent::HideBow()
{
	m_Render = false;

	m_ShowBow = false;
}

void CPlayerBowComponent::OnCollision(const CollisionResult& Result)
{
	// ArrowCollisionFireCollider 를 통과할 때는, Destroy 되는 것이 아니라, Fire Particle들을 활성화 시켜야 한다.
	if (Result.Dest->GetTypeID() == typeid(CArrowCollisionFireCollider).hash_code())
	{
		CArrowCollisionFireCollider* ArrowFireCollider = dynamic_cast<CArrowCollisionFireCollider*>(Result.Dest);

		if (m_Arrow)
		{
			CArrowComponent* ArrowComp = m_Arrow->FindObjectComponentFromType<CArrowComponent>();

			bool IsArrowOnFire = ArrowComp->IsArrowOnFire();

			// 1. Arrow 가 불이 붙지 않았다면
			// 1) 장작에 불이 붙어있지 않다면 아무일 X
			// 2) 장작에 불이 붙었다면, Arrow 에 불을 붙인다.
			if (IsArrowOnFire == false && ArrowFireCollider->IsFireOnByArrow())
			{
				// 1. 장작에 불이 붙어있다면 -> Arrow 에 불을 붙일 것이다.
				m_Arrow->StartParticle(m_Arrow->GetWorldPos());

				// >> Arrow 들의 Particle 들을 Arrow 진행 방향에 맞게 Y 축 기준 회전시킬 것이다.
				const Vector3& BaseDir = Vector3(0.f, 0.f, -1.f);
				const Vector3& LeftDir  = Vector3(-1.f, 0.f, 0.f);

				const Vector3& ArrowDir = ArrowComp->GetMoveDir();

				float YRotAngle = ArrowDir.Angle(BaseDir);
				
				// 회전 각도 180도 이상
				if (ArrowDir.Dot(LeftDir) > 0)
				{
					YRotAngle = 180 + (180 - YRotAngle);
				}

				// Particle 들을 회전 시켜줄 것이다.
				std::vector<CParticleComponent*> vecArrowParticles;
				m_Arrow->FindAllSceneComponentFromType<CParticleComponent>(vecArrowParticles);

				size_t ParticleSize = vecArrowParticles.size();

				for (size_t i = 0; i < ParticleSize; ++i)
				{
					// vecArrowParticles[i]->CRef::Enable(true);
					vecArrowParticles[i]->SetWorldRotationY(YRotAngle);
				}

				// Light Component 를 활성화 시켜줄 것이다.
				CLightComponent* ArrowLight = m_Arrow->FindComponentFromType<CLightComponent>();

				if (ArrowLight)
					ArrowLight->CRef::Enable(true);

				// >> Arrow Comp 가 Fire 이 붙었다고 표시하기 
				ArrowComp->SetArrowOnFireEnable(true);
			}
			// 바로 Destroy  시켜준다.
			else if (IsArrowOnFire)
			{
				// 아무것도 안해준다.
				//m_Object->GetScene()->GetCameraManager()->ShakeCamera(0.4f, 1.f);
				//
				//m_Destroy = true;
				//
				//if (m_Arrow)
				//	m_Arrow->Destroy();
			}
			
			// Arrow 에 불이 붙은 상태이고
			// 장작에는 불이 붙지 않은 상태라면
			// 장작에 불을 붙이고 (이것은 ArrowCollisionFireCollider 에서 해준다.)
			// Arrow 는 Destroy 시킨다.
			// (아래의 코드는 먹히지 않는다. 왜냐하면 ArrowCollisionFireCollider 의 CallBack 이 호출된 상태, 따라서 ArrowFireCollider->IsFireOnByArrow() 가 true 로 바뀐 상태
			// else if (IsArrowOnFire && ArrowFireCollider->IsFireOnByArrow() == false)
			// {
			// 	m_Object->GetScene()->GetCameraManager()->ShakeCamera(0.4f, 1.f);
			// 
			// 	// OnCollision에서 바로 Destroy하면 CCollisionSection::Collision에서 m_vecCollider의 size가 갑자기 바뀌어서 문제가 되므로
			// 	// m_Destroy = true로 만들어줬다가 PrevRender 함수에서 m_Destroy가 true면 Destroy
			// 	m_Destroy = true;
			// 
			// 	if (m_Arrow)
			// 		m_Arrow->Destroy();
			// }
		}
	}
	else
	{
		m_Object->GetScene()->GetCameraManager()->ShakeCamera(0.4f, 1.f);

		// OnCollision에서 바로 Destroy하면 CCollisionSection::Collision에서 m_vecCollider의 size가 갑자기 바뀌어서 문제가 되므로
		// m_Destroy = true로 만들어줬다가 PrevRender 함수에서 m_Destroy가 true면 Destroy
		m_Destroy = true;

		CGameObject* DestObject = Result.Dest->GetGameObject();

		CMonsterDataComponent* Data = (CMonsterDataComponent*)DestObject->FindObjectComponent("ObjectData");

		if (DestObject->GetObjectType() == Object_Type::Monster && Data)
		{
			Data->SetIsHit(true);
			Data->DecreaseHP(5);
			Data->SetIsHit(false);

			m_Object->GetScene()->GetResource()->SoundPlay("ArrowHitEnemy");
		}

		else
		{
			m_Object->GetScene()->GetResource()->SoundPlay("ArrowHitWall");
		}

		if (m_Arrow)
			m_Arrow->Destroy();
	}

	
}

bool CPlayerBowComponent::Save(FILE* File)
{
	return CStaticMeshComponent::Save(File);
}

bool CPlayerBowComponent::Load(FILE* File)
{
	return CStaticMeshComponent::Load(File);
}

bool CPlayerBowComponent::SaveOnly(FILE* File)
{
	return true;
}

bool CPlayerBowComponent::LoadOnly(FILE* File)
{
	return true;
}
