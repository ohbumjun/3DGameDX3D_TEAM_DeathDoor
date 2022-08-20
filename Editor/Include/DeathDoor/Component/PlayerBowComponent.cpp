
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
	// �׺�޽� ���� ��ŷ�� ��� ���� ó��
	if (ShootDir.x == 0.f && ShootDir.y == 0.f && ShootDir.z == 0.f)
	{
		return;
	}

	Vector3 Pos = m_Object->GetWorldPos();
	Vector3 BowPos = Vector3(Pos.x + ShootDir.x, Pos.y + 2.f, Pos.z + ShootDir.z);

	// (1, 0, 0)�� ȭ���� ����
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

	// Particle, Light Component �� ��� False �� ó�����ش�.
	std::vector<CParticleComponent*> vecArrowParticles;
	m_Arrow->FindAllSceneComponentFromType<CParticleComponent>(vecArrowParticles);

	size_t ParticleSize = vecArrowParticles.size();

	for (size_t i = 0; i < ParticleSize; ++i)
	{
		vecArrowParticles[i]->CRef::Enable(false);
	}

	CLightComponent* ArrowLight = m_Arrow->FindComponentFromType<CLightComponent>();
	ArrowLight->CRef::Enable(false);

	// Arrow �߻�
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
	// ArrowCollisionFireCollider �� ����� ����, Destroy �Ǵ� ���� �ƴ϶�, Fire Particle���� Ȱ��ȭ ���Ѿ� �Ѵ�.
	if (Result.Dest->GetTypeID() == typeid(CArrowCollisionFireCollider).hash_code())
	{
		CArrowCollisionFireCollider* ArrowFireCollider = dynamic_cast<CArrowCollisionFireCollider*>(Result.Dest);

		if (m_Arrow)
		{
			CArrowComponent* ArrowComp = m_Arrow->FindObjectComponentFromType<CArrowComponent>();

			bool IsArrowOnFire = ArrowComp->IsArrowOnFire();

			// 1. Arrow �� ���� ���� �ʾҴٸ�
			// 1) ���ۿ� ���� �پ����� �ʴٸ� �ƹ��� X
			// 2) ���ۿ� ���� �پ��ٸ�, Arrow �� ���� ���δ�.
			if (IsArrowOnFire == false && ArrowFireCollider->IsFireOnByArrow())
			{
				// 1. ���ۿ� ���� �پ��ִٸ� -> Arrow �� ���� ���� ���̴�.
				m_Arrow->StartParticle(m_Arrow->GetWorldPos());

				// >> Arrow ���� Particle ���� Arrow ���� ���⿡ �°� Y �� ���� ȸ����ų ���̴�.
				const Vector3& BaseDir = Vector3(0.f, 0.f, -1.f);
				const Vector3& LeftDir  = Vector3(-1.f, 0.f, 0.f);

				const Vector3& ArrowDir = ArrowComp->GetMoveDir();

				float YRotAngle = ArrowDir.Angle(BaseDir);
				
				// ȸ�� ���� 180�� �̻�
				if (ArrowDir.Dot(LeftDir) > 0)
				{
					YRotAngle = 180 + (180 - YRotAngle);
				}

				// Particle ���� ȸ�� ������ ���̴�.
				std::vector<CParticleComponent*> vecArrowParticles;
				m_Arrow->FindAllSceneComponentFromType<CParticleComponent>(vecArrowParticles);

				size_t ParticleSize = vecArrowParticles.size();

				for (size_t i = 0; i < ParticleSize; ++i)
				{
					// vecArrowParticles[i]->CRef::Enable(true);
					vecArrowParticles[i]->SetWorldRotationY(YRotAngle);
				}

				// Light Component �� Ȱ��ȭ ������ ���̴�.
				CLightComponent* ArrowLight = m_Arrow->FindComponentFromType<CLightComponent>();

				if (ArrowLight)
					ArrowLight->CRef::Enable(true);

				// >> Arrow Comp �� Fire �� �پ��ٰ� ǥ���ϱ� 
				ArrowComp->SetArrowOnFireEnable(true);
			}
			// �ٷ� Destroy  �����ش�.
			else if (IsArrowOnFire)
			{
				// �ƹ��͵� �����ش�.
				//m_Object->GetScene()->GetCameraManager()->ShakeCamera(0.4f, 1.f);
				//
				//m_Destroy = true;
				//
				//if (m_Arrow)
				//	m_Arrow->Destroy();
			}
			
			// Arrow �� ���� ���� �����̰�
			// ���ۿ��� ���� ���� ���� ���¶��
			// ���ۿ� ���� ���̰� (�̰��� ArrowCollisionFireCollider ���� ���ش�.)
			// Arrow �� Destroy ��Ų��.
			// (�Ʒ��� �ڵ�� ������ �ʴ´�. �ֳ��ϸ� ArrowCollisionFireCollider �� CallBack �� ȣ��� ����, ���� ArrowFireCollider->IsFireOnByArrow() �� true �� �ٲ� ����
			// else if (IsArrowOnFire && ArrowFireCollider->IsFireOnByArrow() == false)
			// {
			// 	m_Object->GetScene()->GetCameraManager()->ShakeCamera(0.4f, 1.f);
			// 
			// 	// OnCollision���� �ٷ� Destroy�ϸ� CCollisionSection::Collision���� m_vecCollider�� size�� ���ڱ� �ٲ� ������ �ǹǷ�
			// 	// m_Destroy = true�� �������ٰ� PrevRender �Լ����� m_Destroy�� true�� Destroy
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

		// OnCollision���� �ٷ� Destroy�ϸ� CCollisionSection::Collision���� m_vecCollider�� size�� ���ڱ� �ٲ� ������ �ǹǷ�
		// m_Destroy = true�� �������ٰ� PrevRender �Լ����� m_Destroy�� true�� Destroy
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
