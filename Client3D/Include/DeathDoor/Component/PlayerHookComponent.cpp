
#include "PlayerHookComponent.h"
#include "GameObject/GameObject.h"
#include "Component/ColliderHalfLine.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Component/AnimationMeshComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Navigation3DManager.h"

CPlayerHookComponent::CPlayerHookComponent()	:
	m_CurrentHookIndex(0),
	m_ClearHookIndex(0),
	m_AccTime(0.f),
	m_InFlying(false),
	m_AccLastHookDelayTime(0.f),
	m_PlayFireSound(false),
	m_PlayInShootSound(false)
{
	SetTypeID<CPlayerHookComponent>();
	m_ComponentType = Component_Type::SceneComponent;

	SetWorldScale(Vector3(0.f, 0.f, 0.f));
	m_Render = false;

	m_UnitSize = Vector2(0.9f, 0.9f);
}

CPlayerHookComponent::CPlayerHookComponent(const CPlayerHookComponent& com)	:
	CStaticMeshComponent(com)
{
}

CPlayerHookComponent::~CPlayerHookComponent()
{
}

void CPlayerHookComponent::Start()
{
	CStaticMeshComponent::Start();
}

bool CPlayerHookComponent::Init()
{
	if (!CStaticMeshComponent::Init())
		return false;

	return true;
}

void CPlayerHookComponent::Update(float DeltaTime)
{
	CStaticMeshComponent::Update(DeltaTime);
}

void CPlayerHookComponent::PostUpdate(float DeltaTime)
{
	CStaticMeshComponent::PostUpdate(DeltaTime);
}

void CPlayerHookComponent::PrevRender()
{
	m_Render = false;
	CStaticMeshComponent::PrevRender();
}

void CPlayerHookComponent::Render()
{
	CStaticMeshComponent::Render();
}

void CPlayerHookComponent::PostRender()
{
	CStaticMeshComponent::PostRender();
}

HookResult CPlayerHookComponent::ShootHook(const Vector3& ShootDir, float DeltaTime)
{
	// 네비메쉬 밖을 피킹할 경우 예외 처리
	if (ShootDir.x == 0.f && ShootDir.y == 0.f && ShootDir.z == 0.f)
	{
		return HookResult::NoCollision;
	}

	if (m_vecHookChain.size() == 0)
	{
		const PathInfo* Info = CPathManager::GetInst()->FindPath(MESH_PATH);

		char FullPath[MAX_PATH] = {};
		strcpy_s(FullPath, Info->PathMultibyte);
		strcat_s(FullPath, "Chain.msh");

		char FullPath2[MAX_PATH] = {};
		strcpy_s(FullPath2, Info->PathMultibyte);
		strcat_s(FullPath2, "HookHead.msh");

		m_Scene->GetResource()->LoadMeshFullPathMultibyte(Mesh_Type::Static, "HookChain", FullPath);
		m_Scene->GetResource()->LoadMeshFullPathMultibyte(Mesh_Type::Static, "HookHead", FullPath2);

		for (size_t i = 0; i < 21; ++i)
		{
			CStaticMeshComponent* HookChain = m_Object->CreateComponent<CStaticMeshComponent>("Hook");
			AddChild(HookChain);
			HookChain->SetWorldPos(Vector3(FLT_MAX, FLT_MAX, FLT_MAX));

			if (i == 20)
			{
				HookChain->SetMesh("HookHead");

			}

			else
			{
				HookChain->SetMesh("HookChain");
			}

			HookChain->SetWorldScale(Vector3(0.05f, 0.05f, 0.05f));
			HookChain->SetEmissiveColor(0.f, 0.6f, 0.f, 1.f);

			m_vecHookChain.push_back(HookChain);
		}
		Vector3 HookHeadPos = GetWorldPos();

		m_Collider = m_Object->CreateComponent<CColliderHalfLine>("HookChain");
		AddChild(m_Collider);
		m_Collider->SetWorldPos(Vector3(HookHeadPos.x, HookHeadPos.y + 1.f, HookHeadPos.z));
		m_Collider->SetWorldRotationY(-90.f);
		m_Collider->SetWorldScale(0.f, 0.f, 0.f);
		m_Collider->SetRender(true);
		m_Collider->SetCollisionProfile("PlayerAttack");
		m_Collider->Start();
		m_Collider->AddCollisionCallback<CPlayerHookComponent>(Collision_State::Begin, this, &CPlayerHookComponent::OnHookCollision);

		m_Render = false;
	}

	if (m_InFlying)
	{
		Vector3 FlyDir = m_ShootDestPoint - m_Object->GetWorldPos();
		FlyDir.Normalize();
		m_Object->AddWorldPos(FlyDir.x * 20.f * DeltaTime, 0.f, FlyDir.z * 20.f * DeltaTime);

		Vector3 CurrentPos = m_Object->GetWorldPos();

		float Dist = Vector3(CurrentPos.x, 0.f, CurrentPos.z).Distance(Vector3(m_ShootDestPoint.x, 0.f, m_ShootDestPoint.z));
		if (Dist < 2.2f)
		{
			CAnimationMeshComponent* Comp = m_Object->FindComponentFromType<CAnimationMeshComponent>();

			if (Comp)
			{
				Comp->GetAnimationInstance()->ChangeAnimation("Idle");
			}
			
			m_InFlying = false;

			size_t Count = m_vecHookChain.size();

			for (size_t i = 0; i < Count; ++i)
			{
				m_vecHookChain[i]->GetTransform()->SetFixTransform(false);
			}
			m_Collider->GetTransform()->SetFixTransform(false);

			CNavigation3DManager* Manager = CSceneManager::GetInst()->GetScene()->GetNavigation3DManager();
			float Height = 0.f;
			Manager->SetPlayerPolyIndex(-1);
			bool Valid = Manager->CheckPlayerNavMeshPoly(Height);

			if (Valid)
			{
				m_Object->SetWorldPos(CurrentPos.x, Height, CurrentPos.z);
			}

			ClearHookChain();

			return HookResult::CollisionEnd;
		}

		size_t Count = m_vecHookChain.size();
		for (size_t i = m_ClearHookIndex; i < Count; ++i)
		{
			Vector3 ChainPos = m_vecHookChain[i]->GetWorldPos();

			if (Vector3(ChainPos.x, 0.f, ChainPos.z).Distance(Vector3(CurrentPos.x, 0.f, CurrentPos.z)) < 1.f)
			{
				for (size_t j = 0; j <= i; ++j)
				{
					m_vecHookChain[j]->SetRender(false);
					m_vecHookChain[j]->Enable(false);
					m_vecHookChain[j]->SetWorldScale(Vector3(0.f, 0.f, 0.f));
					m_vecHookChain[j]->SetWorldPos(FLT_MAX, FLT_MAX, FLT_MAX);
					m_vecHookChain[j]->GetTransform()->ForceUpdateMat();
				}
				++m_ClearHookIndex;
			}
		}


		return HookResult::OnShoot;
	}

	m_AccTime += DeltaTime;

	if (m_CurrentHookIndex < 21)
	{
		if (m_CurrentHookIndex == 0 && !m_PlayFireSound)
		{
			m_Object->GetScene()->GetResource()->SoundPlay("HookShotFire");
			m_PlayFireSound = true;
		}

		else if (m_CurrentHookIndex > 0 &&  m_CurrentHookIndex % 7 == 0)
		{
			//m_Object->GetScene()->GetResource()->SoundStop("HookShotBeginReturn");
			m_Object->GetScene()->GetResource()->SoundPlay("HookShotBeginReturn");
		}

		if (m_AccTime > 0.02f)
		{
			// 방향에 맞게 HookChain 회전시켜준다
			Vector3 ChainZAxis = GetWorldAxis(AXIS::AXIS_Z);
			float Angle = ShootDir.Angle(ChainZAxis);

			Vector3 Scale = m_Collider->GetWorldScale();
			Vector3 ObjectPos = m_Object->GetWorldPos();
			Vector3 HookHeadPos = GetWorldPos();
			Vector3 HookPos = Vector3(ObjectPos.x + (m_CurrentHookIndex + 1) * ShootDir.x * m_UnitSize.x, HookHeadPos.y + 1.f,
				ObjectPos.z + (m_CurrentHookIndex + 1) * ShootDir.z * m_UnitSize.y);

			m_Collider->Enable(true);
			m_Collider->SetRender(true);

			m_Collider->SetWorldScale((Scale.x + 0.9f), 1.f, 1.f);

			m_vecHookChain[m_CurrentHookIndex]->SetRender(true);
			m_vecHookChain[m_CurrentHookIndex]->Enable(true);
			m_vecHookChain[m_CurrentHookIndex]->SetWorldPos(HookPos);

			if(m_CurrentHookIndex != 20)
				m_vecHookChain[m_CurrentHookIndex]->SetWorldScale(Vector3(0.05f, 0.05f, 0.05f));
			else
				m_vecHookChain[m_CurrentHookIndex]->SetWorldScale(Vector3(5.f, 5.f, 5.f));

			Vector3 CrossVec = ShootDir.Cross(ChainZAxis);

			if (CrossVec.y > 0.f)
				Angle *= -1.f;

			m_vecHookChain[m_CurrentHookIndex]->SetWorldRotationY(Angle);
			m_Collider->SetWorldRotationY(-90.f + Angle);

			// Rotation해서 Start/End Pos정보가 갱신됐을 수도 있으니 Refresh해준다
			m_Collider->RefreshInfo();

			++m_CurrentHookIndex;

			m_ShootDir = ShootDir;

			m_AccTime = 0.f;

			return HookResult::OnShoot;
		}
	}

	else
	{
		m_AccLastHookDelayTime += DeltaTime;

		if (m_AccLastHookDelayTime > 0.3f)
		{
			if (m_CurrentHookIndex == m_vecHookChain.size())
			{
				ClearHookChain();

				m_AccLastHookDelayTime = 0.f;

				m_PlayFireSound = false;
				m_PlayInShootSound = false;

				return HookResult::NoCollision;
			}
		}

		return HookResult::OnShoot;
	}

	return HookResult::OnShoot;
}

void CPlayerHookComponent::OnHookCollision(const CollisionResult& Result)
{
	CStaticMeshComponent* HookComp = m_vecHookChain[m_CurrentHookIndex - 1];
	Vector3 LastHookPos = HookComp->GetWorldPos();
	Vector3 LastHookRot = HookComp->GetWorldRot();
	size_t Count = m_vecHookChain.size() - 1;

	m_vecHookChain[Count]->Enable(true);
	m_vecHookChain[Count]->SetRender(true);
	m_vecHookChain[Count]->SetWorldScale(5.f, 5.f, 5.f);
	m_vecHookChain[Count]->SetWorldRotation(LastHookRot);
	m_vecHookChain[Count]->SetWorldPos(LastHookPos);
	m_vecHookChain[Count]->GetTransform()->ForceUpdateMat();
	
	HookComp->SetWorldPos(FLT_MAX, FLT_MAX, FLT_MAX);
	HookComp->SetRender(false);
	HookComp->GetTransform()->ForceUpdateMat();


	m_InFlying = true;
	//m_ShootDestPoint = Result.Dest->GetWorldPos();
	m_ShootDestPoint = m_Collider->GetInfo().EndPos;
	CAnimationMeshComponent* Comp = m_Object->FindComponentFromType<CAnimationMeshComponent>();

	if (Comp)
	{
		Comp->GetAnimationInstance()->ChangeAnimation("PlayerHookFly");
	}

	//m_Object->AddWorldPos(0.f, 3.f, 0.f);

	m_AccTime = 0.f;
	m_Collider->Enable(false);
	m_Collider->SetRender(false);
	m_Collider->SetWorldScale(Vector3(0.f, 0.f, 0.f));

	m_CurrentHookIndex = 0;
	m_ClearHookIndex = 0;

	Count = m_vecHookChain.size();

	for (size_t i = 0; i < Count; ++i)
	{
		m_vecHookChain[i]->GetTransform()->SetFixTransform(true);
	}

	m_Collider->GetTransform()->SetFixTransform(true);

	m_Object->GetScene()->GetResource()->SoundStop("HookShotBeginReturn");
	m_Object->GetScene()->GetResource()->SoundPlay("HookShotCollison");

	m_PlayFireSound = false;
	m_PlayInShootSound = false;
}

void CPlayerHookComponent::ClearHookChain()
{
	size_t Count = m_vecHookChain.size();

	for (size_t i = 0; i < Count; ++i)
	{
		m_vecHookChain[i]->SetRender(false);
		m_vecHookChain[i]->Enable(false);
		m_vecHookChain[i]->SetWorldScale(Vector3(0.f, 0.f, 0.f));
		m_vecHookChain[i]->SetWorldRotation(0.f, 0.f, 0.f);
		m_vecHookChain[i]->SetWorldPos(FLT_MAX, FLT_MAX, FLT_MAX);
		m_vecHookChain[i]->GetTransform()->ForceUpdateMat();
	}

	m_Collider->SetRender(false);
	m_Collider->SetWorldScale(Vector3(0.f, 0.f, 0.f));
	m_Collider->SetWorldRotationY(-90.f);

	m_CurrentHookIndex = 0;
	m_AccTime = 0.f;
	m_InFlying = false;

	m_ShootDestPoint = Vector3(0.f, 0.f, 0.f);
	m_ShootDir = Vector3(0.f, 0.f, 0.f);

	m_Object->GetScene()->GetResource()->SoundStop("HookShotBeginReturn");
}


CPlayerHookComponent* CPlayerHookComponent::Clone()
{
	return new CPlayerHookComponent(*this);
}

bool CPlayerHookComponent::Save(FILE* File)
{
	// save하기 전에 계층구조 모두 지워서 새로 load할때마다 계층구조 새로 만들도록 하기
	m_vecChild.clear();

	CStaticMeshComponent::Save(File);
	

	return true;
}

bool CPlayerHookComponent::Load(FILE* File)
{
	CStaticMeshComponent::Load(File);

	m_Render = false;

	//const PathInfo* Info = CPathManager::GetInst()->FindPath(MESH_PATH);

	//char FullPath[MAX_PATH] = {};
	//strcpy_s(FullPath, Info->PathMultibyte);
	//strcat_s(FullPath, "Chain.msh");

	//m_Scene->GetResource()->LoadMeshFullPathMultibyte(Mesh_Type::Static, "HookChain", FullPath);

	//size_t Count = m_vecChild.size();

	//for (size_t i = 0; i < Count; ++i)
	//{
	//	if (m_vecChild[i]->CheckType<CStaticMeshComponent>())
	//	{
	//		m_vecHookChain.push_back((CStaticMeshComponent*)(m_vecChild[i].Get()));
	//		m_vecHookChain[m_vecHookChain.size() - 1]->SetMesh("HookChain");
	//	}

	//	else if (m_vecChild[i]->CheckType<CColliderHalfLine>())
	//	{
	//		m_Collider = (CColliderHalfLine*)(m_vecChild[i].Get());
	//	}
	//}

	return true;
}

bool CPlayerHookComponent::SaveOnly(FILE* File)
{
	return true;
}

bool CPlayerHookComponent::LoadOnly(FILE* File)
{
	return true;
}


void CPlayerHookComponent::SetMesh(CStaticMesh* Mesh)
{
	CStaticMeshComponent::SetMesh(Mesh);
}