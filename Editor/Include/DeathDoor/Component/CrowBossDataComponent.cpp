
#include "CrowBossDataComponent.h"
#include "../DataManager.h"
#include "MonsterNavAgent.h"
#include "Component/AnimationMeshComponent.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneResource.h"
#include "Component/StaticMeshComponent.h"
#include "Component/ColliderHalfLine.h"
#include "ObjectPool.h"
#include "Component/ColliderBox3D.h"
#include "PlayerDataComponent.h"
#include "TinyCrowDataComponent.h"
#include "Component/ParticleComponent.h"

CCrowBossDataComponent::CCrowBossDataComponent()	:
	m_StartFlying(false),
	m_InFlying(false),
	m_StartStomp(false),
	m_Phase(-1),
	m_PhasePick(false),
	m_StartJump(false),
	m_CurrentShootCount(0),
	m_ShootAccTime(0.f),
	m_CurrentHookIndex(0),
	m_ClearHookIndex(0),
	m_HookChainTotal(115),
	m_AfterShoot(false),
	m_SpittingStart(false),
	m_SpittingAccTime(0.f),
	m_CurrentTinyCrowIndex(0),
	m_ShootDirFixed(false),
	m_FeatherParticle(nullptr),
	m_FlySoundPlayed(false),
	m_ShootSoundPlayed(false),
	m_StepSoundPlayed(false)
{
	SetTypeID<CCrowBossDataComponent>();


	m_ShootState = CrowBossShootState::Ready;
	m_UnitSize = Vector2(0.6f, 0.6f);

	m_ComponentType = Component_Type::ObjectComponent;
}

CCrowBossDataComponent::CCrowBossDataComponent(const CCrowBossDataComponent& com)	:
	CMonsterDataComponent(com)
{
}

CCrowBossDataComponent::~CCrowBossDataComponent()
{
}

void CCrowBossDataComponent::Start()
{
	CMonsterDataComponent::Start();

	m_HitBox->Enable(true);
	if(m_MeleeAttackCollider)
		m_MeleeAttackCollider->Enable(false);

	m_Data = CDataManager::GetInst()->GetObjectData("CrowBoss");

	if(m_MonsterNavAgent)
		m_MonsterNavAgent->SetMoveSpeed(m_Data.MoveSpeed);

	m_AnimMesh->GetAnimationInstance()->AddNotify<CCrowBossDataComponent>("GuidedBullet", "ScreamSoundPlay", 25, this, &CCrowBossDataComponent::OnScreamSoundPlay);
	m_AnimMesh->GetAnimationInstance()->AddNotify<CCrowBossDataComponent>("CutScene", "CutSceneScreamSoundPlay", 64, this, &CCrowBossDataComponent::OnScreamSoundPlay);
	m_AnimMesh->GetAnimationInstance()->AddNotify<CCrowBossDataComponent>("Sliding", "FlySoundPlay", 1, this, &CCrowBossDataComponent::OnFlySoundPlay);
	m_AnimMesh->GetAnimationInstance()->AddNotify<CCrowBossDataComponent>("Run", "StepSoundPlay", 1, this, &CCrowBossDataComponent::OnStepSoundPlay);


	m_AnimMesh->GetAnimationInstance()->SetEndFunction<CCrowBossDataComponent>("Jump", this, &CCrowBossDataComponent::OnEndCrowBossJump);

	for (int i = 0; i < 100; ++i)
	{
		m_PhaseQueue.push(4);
		m_PhaseQueue.push(3);
		m_PhaseQueue.push(2);
		m_PhaseQueue.push(2);
		m_PhaseQueue.push(5);
		m_PhaseQueue.push(2);
	}

	if(m_MeleeAttackCollider)
		m_MeleeAttackCollider->AddCollisionCallback<CCrowBossDataComponent>(Collision_State::Begin, this, &CCrowBossDataComponent::OnCollision);


	m_FeatherParticle = m_Object->FindComponentFromType<CParticleComponent>();
	m_FeatherParticle->Enable(false);

	m_Object->GetScene()->GetResource()->SoundStop("LockstoneBGM");
}

void CCrowBossDataComponent::Update(float DeltaTime)
{
	CMonsterDataComponent::Update(DeltaTime);

	if (m_DeathColorChangeStart)
	{
		ChangeColorBossDeath(DeltaTime);
	}
}

void CCrowBossDataComponent::ClearTinyCrow()
{
	m_TinyCrowList.clear();
}

void CCrowBossDataComponent::OnEndCrowBossJump()
{
	m_AnimMesh->GetAnimationInstance()->ChangeAnimation("Stomp");
	m_StartStomp = true;
}

void CCrowBossDataComponent::SetIsHit(bool Hit)
{
	CMonsterDataComponent::SetIsHit(Hit);

	m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_TakeDamage");
}

void CCrowBossDataComponent::ShootChain(const Vector3& ShootDir, float DeltaTime)
{
	m_ShootState = CrowBossShootState::Shoot;

	if (m_vecHookChain.size() == 0)
	{
		const PathInfo* Info = CPathManager::GetInst()->FindPath(MESH_PATH);

		char FullPath[MAX_PATH] = {};
		strcpy_s(FullPath, Info->PathMultibyte);
		strcat_s(FullPath, "Chain.msh");

		m_Scene->GetResource()->LoadMeshFullPathMultibyte(Mesh_Type::Static, "HookChain", FullPath);

		for (size_t i = 0; i < m_HookChainTotal; ++i)
		{
			CStaticMeshComponent* HookChain = m_Object->CreateComponentAddChild<CStaticMeshComponent>("HookChain");

			HookChain->SetWorldPos(Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
			HookChain->SetMesh("HookChain");
			HookChain->SetWorldScale(Vector3(0.f, 0.f, 0.f));
			HookChain->GetMaterial(0)->SetEmissiveColor(0.f, 0.f, 0.f, 1.f);
			HookChain->SetRender(false);

			m_vecHookChain.push_back(HookChain);
		}
	}

	m_ShootAccTime += DeltaTime;

	if (m_CurrentHookIndex < m_HookChainTotal)
	{
		if (m_ShootAccTime > 0.007f)
		{
			if (m_CurrentHookIndex == 0 && !m_ShootSoundPlayed)
			{
				switch (m_CurrentShootCount)
				{
				case 0:
					m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Chain1");
					break;
				case 1:
					m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Chain2");
					break;
				case 2:
					m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Chain3");
					break;
				case 3:
					m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Chain4");
					break;
				}

				m_ShootSoundPlayed = true;
			}

			// 방향에 맞게 HookChain 회전시켜준다
			Vector3 ChainZAxis = m_vecHookChain[m_CurrentHookIndex]->GetWorldAxis(AXIS::AXIS_Z);
			float Angle = ShootDir.Angle(ChainZAxis);

			Vector3 ObjectPos = m_Object->GetWorldPos();
			ObjectPos.x += ShootDir.x * 4.5f;
			ObjectPos.z += ShootDir.z * 4.5f;

			Vector3 HookPos = Vector3(ObjectPos.x + (m_CurrentHookIndex + 1) * ShootDir.x * m_UnitSize.x, ObjectPos.y + 0.7f,
				ObjectPos.z + (m_CurrentHookIndex + 1) * ShootDir.z * m_UnitSize.y);

			m_vecHookChain[m_CurrentHookIndex]->SetRender(true);
			m_vecHookChain[m_CurrentHookIndex]->Enable(true);
			m_vecHookChain[m_CurrentHookIndex]->SetWorldPos(HookPos);
			m_vecHookChain[m_CurrentHookIndex]->SetWorldScale(Vector3(0.06f, 0.06f, 0.06f));
			m_vecHookChain[m_CurrentHookIndex]->GetTransform()->ForceUpdateMat();


			m_vecHookChain[m_CurrentHookIndex]->GetMaterial(0)->SetBaseColor(0.44f, 0.44f, 0.44f, 1.f);

			Vector3 CrossVec = ShootDir.Cross(ChainZAxis);

			if (CrossVec.y > 0.f)
				Angle *= -1.f;

			//m_vecHookChain[m_CurrentHookIndex]->SetWorldRotation(0.f, Angle, 0.f);

			if (m_CurrentHookIndex % 2 == 0)
			{
				m_vecHookChain[m_CurrentHookIndex]->SetWorldRotation(0.f, Angle, 45.f);
			}

			else
			{
				m_vecHookChain[m_CurrentHookIndex]->SetWorldRotation(0.f, Angle, -45.f);
			}

			++m_CurrentHookIndex;

			m_ShootAccTime = 0.f;
			m_ShootDir = ShootDir;

			return;
		}
	}

	else if (m_CurrentHookIndex == m_HookChainTotal)
	{
		size_t Count = m_vecHookChain.size();

		for (size_t i = 0; i < Count; ++i)
		{
			m_vecHookChain[i]->GetTransform()->SetFixTransform(true);
		}

		m_ShootState = CrowBossShootState::ShootEnd;
		++m_CurrentShootCount;
		m_ShootDirFixed = false;

		m_ShootSoundPlayed = false;
	}
}

void CCrowBossDataComponent::Fly(const Vector3& FlyDir, float DeltaTime)
{
	m_ShootState = CrowBossShootState::Fly;

	float FlySpeed = m_Data.FlySpeed;

	m_Object->AddWorldPos(FlyDir.x * FlySpeed * DeltaTime, 0.f, FlyDir.z * FlySpeed * DeltaTime);

	Vector3 CurrentPos = m_Object->GetWorldPos();


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
				m_vecHookChain[j]->SetWorldPos(Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
				m_vecHookChain[j]->GetTransform()->ForceUpdateMat();
			}

			++m_ClearHookIndex;
		}

		if (m_ClearHookIndex == 0 && m_FeatherParticle)
		{
			//m_FeatherParticle->Enable(true);
			Vector3 CrowPos = m_Object->GetWorldPos();

			const Vector3& ParticleBaseDir = Vector3(0.f, 0.f, -1.f);

			Vector3 ZAxis = m_Object->GetWorldAxis(AXIS_Z);
			ZAxis *= -1.f;

			float DotProduct = ZAxis.Dot(ParticleBaseDir);
			float Angle = 0.f;

			if (DotProduct <= 0.9999999f && DotProduct >= -0.9999999f)
			{
				Angle = RadianToDegree(acosf(DotProduct));

				Vector3 CrossVec = ZAxis.Cross(ParticleBaseDir);

				// 파티클의 y축 회전은 오른손 좌표계?
				if (CrossVec.y < 0.f)
					Angle *= -1.f;
			}


			m_FeatherParticle->SetWorldRotationY(Angle);
			m_FeatherParticle->StartParticle(CrowPos + ZAxis);
		}
	}

	if (m_ClearHookIndex == m_HookChainTotal)
	{
		m_CurrentHookIndex = 0;
		m_ClearHookIndex = 0;
		m_ShootState = CrowBossShootState::Done;
		m_MeleeAttackCollider->Enable(false);

		size_t Count = m_vecHookChain.size();

		for (size_t i = 0; i < Count; ++i)
		{
			m_vecHookChain[i]->GetTransform()->SetFixTransform(false);
			m_vecHookChain[i]->SetWorldRotationY(0.f);
		}

		m_FlySoundPlayed = false;
	}

}

void CCrowBossDataComponent::Teleport()
{
	m_ShootState = CrowBossShootState::Teleport;

	CGameObject* Player = m_Scene->GetPlayerObject();
	Vector3 PlayerPos = Player->GetWorldPos();
	Vector3 MyPos = m_Object->GetWorldPos();

	std::vector<Vector3> vecTeleportPos;
	vecTeleportPos.push_back(Vector3(PlayerPos.x, 0.f, PlayerPos.z - 20.f));
	vecTeleportPos.push_back(Vector3(PlayerPos.x, 0.f, PlayerPos.z + 20.f));
	vecTeleportPos.push_back(Vector3(PlayerPos.x + 35.f, 0.f, PlayerPos.z));
	vecTeleportPos.push_back(Vector3(PlayerPos.x - 35.f, 0.f, PlayerPos.z));
	vecTeleportPos.push_back(Vector3(PlayerPos.x + 35.f, 0.f, PlayerPos.z - 20.f));
	vecTeleportPos.push_back(Vector3(PlayerPos.x - 35.f, 0.f, PlayerPos.z - 20.f));
	vecTeleportPos.push_back(Vector3(PlayerPos.x + 35.f, 0.f, PlayerPos.z + 20.f));
	vecTeleportPos.push_back(Vector3(PlayerPos.x - 35.f, 0.f, PlayerPos.z + 20.f));


	int random = rand() % 8;


	m_Object->SetWorldPos(vecTeleportPos[random]);


	PlayerPos = Player->GetWorldPos();
	MyPos = m_Object->GetWorldPos();
	// 플레이어 방향으로 바라보게 하기
	Vector3 Dir = PlayerPos - MyPos;
	Dir.Normalize();
	Vector3 FaceDir = m_MonsterNavAgent->GetCurrentFaceDir();

	float DotProduct = FaceDir.Dot(Dir);
	Vector3 CrossVec = FaceDir.Cross(Dir);

	if (DotProduct < 0.999999f && DotProduct > -0.999999f)
	{
		float Degree = RadianToDegree(acosf(DotProduct));

		if (CrossVec.y > 0.f)
			m_Object->AddWorldRotationY(Degree);
		else
			m_Object->AddWorldRotationY(-Degree);
	}
	else
	{
		// 평행이고 반대 방향
		if (DotProduct < -0.999999f)
		{
			if (CrossVec.y > 0)
				m_Object->AddWorldRotationY(180.f);
			else
				m_Object->AddWorldRotationY(-180.f);
		}

	}
}

bool CCrowBossDataComponent::Spitting(float DeltaTime)
{
	m_SpittingAccTime += DeltaTime;

	Vector3 FaceDir = m_MonsterNavAgent->GetCurrentFaceDir();
	Vector3 CurrentPos = m_Object->GetWorldPos();

	Vector3 TinyCrowDir = Vector3(0.f, 0.f, 1.f);

	float DotProduct = FaceDir.Dot(TinyCrowDir);
	float Angle = 0.f;
	if (DotProduct < 0.9999999f && DotProduct > -0.99999999f)
	{
		Angle = RadianToDegree(acosf(DotProduct));

		Vector3 CrossVec = FaceDir.Cross(TinyCrowDir);

		if (CrossVec.y > 0)
			Angle *= -1.f;
	}
	else
	{
		if (DotProduct > 0.f)
		{
			// 평행 & 같은 방향
			Angle = 0.f;
		}

		else
		{
			// 평행 & 반대방향
			Angle = 180.f;
		}
	}

	int Random = rand() % 5 - 2;
	int RandomAngle = rand() % 20 - 10;

	if (m_CurrentTinyCrowIndex == 0)
	{
		if (m_SpittingAccTime > 1.f)
		{
			++m_CurrentTinyCrowIndex;
			m_SpittingAccTime = 0.f;
			CGameObject* TinyCrow = CObjectPool::GetInst()->GetMonster("TinyCrow", m_Object->GetScene());

			TinyCrow->SetWorldPos(Vector3(CurrentPos.x + FaceDir.x + Random, 1.f, CurrentPos.z + FaceDir.z));
			TinyCrow->SetWorldRotationY(Angle + RandomAngle);
			TinyCrow->SetWorldScale(0.005f, 0.005f, 0.005f);

			CTinyCrowDataComponent* Comp = TinyCrow->FindObjectComponentFromType<CTinyCrowDataComponent>();

			Comp->SetCrowOwner(m_Object);

			//m_TinyCrowList.push_back(TinyCrow);
		}
	}

	else if (m_SpittingAccTime >= 0.1f)
	{
		if (m_CurrentTinyCrowIndex == 15)
		{
			if (m_SpittingAccTime >= 1.f)
			{
				m_CurrentTinyCrowIndex = 0;
				m_SpittingAccTime = 0.f;

				return true;
			}
			else
				return false;
		}

		++m_CurrentTinyCrowIndex;
		m_SpittingAccTime = 0.f;
		CGameObject* TinyCrow = CObjectPool::GetInst()->GetMonster("TinyCrow", m_Object->GetScene());

		TinyCrow->SetWorldPos(Vector3(CurrentPos.x + FaceDir.x + Random, 1.f, CurrentPos.z + FaceDir.z));
		TinyCrow->SetWorldRotationY(Angle + RandomAngle);
		TinyCrow->SetWorldScale(0.005f, 0.005f, 0.005f);

		CTinyCrowDataComponent* Comp = TinyCrow->FindObjectComponentFromType<CTinyCrowDataComponent>();
		Comp->SetCrowOwner(m_Object);
	}

	return false;
}

void CCrowBossDataComponent::OnCollision(const CollisionResult& Result)
{
	CGameObject* DestObject = Result.Dest->GetGameObject();

	if (DestObject->GetObjectType() == Object_Type::Player)
	{
		CPlayerDataComponent* Data = (CPlayerDataComponent*)DestObject->FindComponent("ObjectData");

		if (!Data->IsUnbeatable())
		{
			Data->DecreaseHP(1);
		}
	}
}

void CCrowBossDataComponent::OnScreamSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Scream");
}

void CCrowBossDataComponent::OnFlySoundPlay()
{
	if (m_FlySoundPlayed)
		return;

	switch (m_CurrentShootCount)
	{
	case 1:
		m_Object->GetScene()->GetResource()->SoundPlay("OldCrowChainDash1");
		break;
	case 2:
		m_Object->GetScene()->GetResource()->SoundPlay("OldCrowChainDash2");
		break;
	case 3:
		m_Object->GetScene()->GetResource()->SoundPlay("OldCrowChainDash3");
		break;
	case 4:
		m_Object->GetScene()->GetResource()->SoundPlay("OldCrowChainDash4");
		break;
	}
	
	m_FlySoundPlayed = true;
}

void CCrowBossDataComponent::OnStepSoundPlay()
{

	m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Step");
}


void CCrowBossDataComponent::OnDeadAnimStart()
{
	CMonsterDataComponent::OnDeadAnimStart();

	size_t Count = m_vecHookChain.size();
	for (size_t i = 0; i < Count; ++i)
	{
		for (size_t j = 0; j <= i; ++j)
		{
			m_vecHookChain[j]->SetRender(false);
			m_vecHookChain[j]->Enable(false);
			m_vecHookChain[j]->SetWorldScale(Vector3(0.f, 0.f, 0.f));
			m_vecHookChain[j]->SetWorldPos(Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
			m_vecHookChain[j]->GetTransform()->ForceUpdateMat();
		}
	}


	m_AnimMesh->GetAnimationInstance()->GetCurrentAnimation()->SetPlayScale(0.25f);
	m_DeathColorChangeTimeMax = m_AnimMesh->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationPlayTime() * 0.5f;


}

void CCrowBossDataComponent::OnDeadPaperBurnEnd()
{
	CMonsterDataComponent::OnDeadPaperBurnEnd();

	// TODO : Boss Knight - 페이퍼번 완료되면 Portal On

	m_Object->GetScene()->GetResource()->SoundStop("CrowBossBGM");
}

void CCrowBossDataComponent::OnEndCutScene()
{
	CMonsterDataComponent::OnEndCutScene();

	m_Object->GetScene()->GetResource()->SoundPlay("CrowBossBGM");
}
