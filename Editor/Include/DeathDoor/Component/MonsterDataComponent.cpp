
#include "MonsterDataComponent.h"
#include "GameObject/GameObject.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/ColliderBox3D.h"
#include "../Component/GameStateComponent.h"
#include "MonsterNavAgent.h"
#include "Scene/Scene.h"
#include "../Component/PlayerDataComponent.h"
#include "Component/PaperBurnComponent.h"
#include "../DDUtil.h"
#include "PlayerNormalAttackCheckCollider.h"
#include "../Scene/DDSceneMode.h"

CMonsterDataComponent::CMonsterDataComponent() :
	m_AnimMesh(nullptr),
	m_RotateToRandom(false),
	m_HitBox(nullptr),
	m_HitEffectFlag(0),
	m_HitEffectTimer(0.f),
	m_HitEffectMax(0.15f),
	m_IsCombat(false),
	m_LookPlayer(false),
	m_CurMoveSpeed(0.f),
	m_DeathColorStart(Vector4::Red),
	m_DeathColorEnd(Vector4::White),
	m_LeftLookPlayer(false),
	m_RightLookPlayer(false),
	m_CurRotSpeed(0.f),
	m_AttackCoolTimeEnable(false), // OBJ�� �߰� 
	m_AttackCoolDelayTimeMax(0.5f)
{
	SetTypeID<CMonsterDataComponent>();

	m_ComponentType = Component_Type::ObjectComponent;
}

CMonsterDataComponent::CMonsterDataComponent(const CMonsterDataComponent& com)	:
	CObjectDataComponent(com)
{
}

CMonsterDataComponent::~CMonsterDataComponent()
{
}

void CMonsterDataComponent::Start()
{
	m_MonsterNavAgent = m_Object->FindObjectComponentFromType<CMonsterNavAgent>();
	m_State = m_Object->FindObjectComponentFromType<CGameStateComponent>();

	std::vector<CAnimationMeshComponent*> vecAnim;
	std::vector<CColliderBox3D*> vecCollider;

	m_Object->FindAllSceneComponentFromType<CAnimationMeshComponent>(vecAnim);
	m_Object->FindAllSceneComponentFromType<CColliderBox3D>(vecCollider);

	if (vecAnim.size() == 1)
	{
		m_AnimMesh = vecAnim[0];
	}
	else
	{
		size_t Size = vecAnim.size();

		for (size_t i = 0; i < Size; ++i)
		{
			if ("AnimComp" == vecAnim[i]->GetName())
			{
				m_AnimMesh = vecAnim[i];
				break;
			}
		}
	}

	if (vecCollider.size() == 1)
	{
		m_HitBox = vecCollider[0];
	}
	else
	{
		m_HitBox = vecCollider[0];
		size_t Size = vecCollider.size();
		
		for (size_t i = 0; i < Size; ++i)
		{
			if ("HitBox" == vecCollider[i]->GetName())
			{
				m_HitBox = vecCollider[i];
				break;
			}
		}
	}

	// Hit Effect�� ���� ���� �÷� ����
	if (m_AnimMesh)
	{
		m_MeshMatSize = m_AnimMesh->GetMaterialSlotCount();
		m_vecOriginDiffuse.resize(m_MeshMatSize);
		m_vecOriginAmbient.resize(m_MeshMatSize);
		m_vecOriginSpecluar.resize(m_MeshMatSize);
		m_vecOriginEmissive.resize(m_MeshMatSize);

		for (int i = 0; i < m_MeshMatSize; ++i)
		{
			m_vecOriginDiffuse[i] = m_AnimMesh->GetMaterial(i)->GetBaseColor();
			m_vecOriginAmbient[i] = m_AnimMesh->GetMaterial(i)->GetAmbientColor();
			m_vecOriginSpecluar[i] = m_AnimMesh->GetMaterial(i)->GetSpecularColor();
			m_vecOriginEmissive[i] = m_AnimMesh->GetMaterial(i)->GetEmissiveColor();
		}
	}

	// Melee Attack Collider CallBack
	m_MeleeAttackCollider = (CColliderBox3D*)m_Object->FindComponent("MeleeAttackCollider");

	if (m_MeleeAttackCollider)
	{
		m_MeleeAttackCollider->Enable(false);
		m_MeleeAttackCollider->AddCollisionCallback(Collision_State::Begin, this, &CMonsterDataComponent::OnHitMeleeAttack);
	}
	
	// PaperBurn �� Death
	m_PaperBurn = m_Object->FindComponentFromType<CPaperBurnComponent>();
	
	if (m_PaperBurn)
	{
		m_PaperBurn->SetFinishCallback(this, &CMonsterDataComponent::OnDeadPaperBurnEnd);
	}

	// Blood Particle
	m_BloodParticle = (CParticleComponent*)m_Object->FindComponent("BloodParticle");

	if (m_BloodParticle)
	{
		// BloodParticle ���� Particle ��, Particle Component �� WorldPos �� ���󰡰� �ؾ� �Ѵ�.
		m_BloodParticle->GetCBuffer()->SetFollowRealTimeParticleComponentPos(true);

		// ó������ Enable False ó���� ���� ���̴�.
		m_BloodParticle->Enable(false);

		// BillBoard ȿ���� ���� ���� ���̴� (Rotation ������ �ޱ� ������)
		m_BloodParticle->SetBillBoardEffect(false);

		// Ȥ�� �𸣴� WorldRot �� ��� 0���� �����ش�.
		m_BloodParticle->SetWorldRotation(0.f, 0.f, 0.f);

		// Blood �� �� �ѹ��� ������ �� �ְ� �����Ѵ�. 
		m_BloodParticle->GetCBuffer()->SetDisableNewAlive(true);

		// m_BloodParticle �� Rot �� ���� ���� ���̴�. Rot �� �ִ� ���� ����� ��߷����� �Ǳ� �����̴�.
		m_BloodParticle->SetInheritRotX(false);
		m_BloodParticle->SetInheritRotY(false);
		m_BloodParticle->SetInheritRotZ(false);
	}

	// Spawn Particle
	m_SpawnParticle = (CParticleComponent*)m_Object->FindComponent("SpawnParticle");

	if (m_SpawnParticle)
	{
		m_SpawnParticle->GetCBuffer()->SetFollowRealTimeParticleComponentPos(true);

		// ó������ Enable False ó���� ���� ���̴�.
		m_SpawnParticle->Enable(false);

		// BillBoard ȿ���� �ٰ��̴�.
		m_SpawnParticle->SetBillBoardEffect(true);

		// Ȥ�� �𸣴� WorldRot �� ��� 0���� �����ش�.
		m_SpawnParticle->SetWorldRotation(0.f, 0.f, 0.f);

		// Blood �� �� �ѹ��� ������ �� �ְ� �����Ѵ�. 
		m_SpawnParticle->GetCBuffer()->SetDisableNewAlive(true);

		// m_BloodParticle �� Rot �� ���� ���� ���̴�. Rot �� �ִ� ���� ����� ��߷����� �Ǳ� �����̴�.
		m_SpawnParticle->SetInheritRotX(false);
		m_SpawnParticle->SetInheritRotY(false);
		m_SpawnParticle->SetInheritRotZ(false);
	}

	if (m_AnimMesh)
	{
		CAnimationSequenceInstance* AnimInst = m_AnimMesh->GetAnimationInstance();
		AnimInst->AddNotify("Death", "DeathStart", 0, this, &CMonsterDataComponent::OnDeadAnimStart);
		AnimInst->SetEndFunction("Death", this, &CMonsterDataComponent::OnDeadAnimEnd);
	}

	// CutScene ���� ( Enter Trigger, CutScene Cam, Collider CallBack)
	m_PlayerEnterZoneTrigger = (CColliderBox3D*)m_Object->FindComponent("PlayerEnterTrigger");

	m_CutSceneCam = m_Object->FindComponentFromType<CCameraComponent>();

	if (m_PlayerEnterZoneTrigger)
	{
		m_PlayerEnterZoneTrigger->AddCollisionCallback(Collision_State::Begin, this, &CMonsterDataComponent::OnPlayerEnterZone);
	}

	// MonsterNavAgent ����
	m_MonsterNavAgent = m_Object->FindObjectComponentFromType<CMonsterNavAgent>();

	// Player Data�� �̸� ��� �ִ´�
	CGameObject* Player = m_Scene->GetPlayerObject();

	if (Player)
	{
		m_PlayerData = Player->FindObjectComponentFromType<CPlayerDataComponent>();
	}

	size_t Size = m_AnimMesh->GetMaterialSlotCount();

	m_OriginEmissive.clear();

	m_OriginEmissive.resize(Size);
	for (int i = 0; i < Size; ++i)
	{
		m_OriginEmissive[i] = m_AnimMesh->GetMaterial(i)->GetEmissiveColor();
	}
}

void CMonsterDataComponent::Update(float DeltaTime)
{
	CObjectDataComponent::Update(DeltaTime);

	if (m_Data.HP <= 0)
	{
		if (m_State)
		{
			SetCurrentNodeNull();
		}

		m_MoveZ = false;
		m_LookPlayer = false;
		m_RightLookPlayer = false;
		m_LeftLookPlayer = false;
	}

	if (m_HitEffectStart)
	{
		ActiveHitEffect(DeltaTime);
	}

	if (m_LookPlayer)
	{
		LookPlayer(DeltaTime);
	}
	else
	{
		if (m_RightLookPlayer)
		{
			// ���������� ���ƾ� �ϴµ� �������� ���� ������ true ��� ����
			if (m_LeftLookPlayer)
				assert(false);

			RightLookPlayer(DeltaTime);
		}
		else  if (m_LeftLookPlayer)
		{
			// ���������� ���ƾ� �ϴµ� �������� ���� ������ true ��� ����
			if (m_RightLookPlayer)
				assert(false);

			LeftLookPlayer(DeltaTime);
		}
	}

	if (m_MoveZEnableMaxTime > 0.f)
	{
		m_MoveZ = true;
		m_MoveZEnableMaxTime -= DeltaTime;

		if (m_MoveZEnableMaxTime < 0.f)
		{
			m_MoveZ = false;
			m_MoveZEnableMaxTime = 0.f;
		}
	}

	if (m_MoveZ)
	{
		MoveZ(DeltaTime);
	}

	if (m_RotateToRandom)
	{
		RotateRandomly(DeltaTime);
	}
}

void CMonsterDataComponent::PostUpdate(float DeltaTime)
{
	CObjectDataComponent::PostUpdate(DeltaTime);

	if (m_AttackCoolTimeEnable)
	{
		m_AttackCoolDelayTime -= DeltaTime;

		if (m_AttackCoolDelayTime < 0.f)
		{
			m_AttackCoolTimeEnable = false;
		}
	}
}

void CMonsterDataComponent::Reset()
{
	if (!m_AnimMesh)
	{
		return;
	}

	size_t Size = m_AnimMesh->GetMaterialSlotCount();

	for (int i = 0; i < Size; ++i)
	{
		m_AnimMesh->GetMaterial(i)->SetEmissiveColor(m_OriginEmissive[i]);
	}
}

CMonsterDataComponent* CMonsterDataComponent::Clone()
{
	return new CMonsterDataComponent(*this);
}

void CMonsterDataComponent::LookPlayer(float DeltaTime)
{
	// �÷��̾� �������� ȸ�� ������ŭ ȸ��
	float AnglePlayer = GetAnglePlayer();

	CGameObject* MyObj = m_Object;

	float RotSpeed = m_Data.RotateSpeedPerSec;

	// ���� m_CurRotSpeed �� ������ ������ ���¶��
	if (m_CurRotSpeed != 0.f)
	{
		RotSpeed = m_CurRotSpeed;
	}

	// (OBJ) ���������� �̼��ϰ� ������ ����
	// if (abs(AnglePlayer) < m_Data.RotateSpeedPerSec * DeltaTime)
	if (abs(AnglePlayer) < 6.f)
	{
		// MyObj->AddWorldRotationY(AnglePlayer * DeltaTime);
	}
	else 
	{
		bool IsLeft = IsPlayerLeftBasedInLookDir();

		if (IsLeft)
		{
			MyObj->AddWorldRotationY(RotSpeed * DeltaTime);
		}
		else
		{
			MyObj->AddWorldRotationY(-1.f * RotSpeed * DeltaTime);
		}
	}
}

void CMonsterDataComponent::RightLookPlayer(float DeltaTime)
{
	m_Object->AddWorldRotationY(-1.f * m_Data.RotateSpeedPerSec * DeltaTime);
}

void CMonsterDataComponent::LeftLookPlayer(float DeltaTime)
{
	m_Object->AddWorldRotationY(m_Data.RotateSpeedPerSec * DeltaTime);
}

void CMonsterDataComponent::MoveZ(float DeltaTime)
{
	m_Object->AddWorldPosByLocalAxis(AXIS::AXIS_Z, -m_CurMoveSpeed * DeltaTime);
}

void CMonsterDataComponent::ChangeColorBossDeath(float DeltaTime)
{
	if (m_DeathColorChangeStart)
	{
		m_DeathTimer += DeltaTime;

		// �÷� ���� ����
		Vector4 Color = CDDUtil::LerpColor(m_DeathColorStart, m_DeathColorEnd, m_DeathTimer, m_DeathColorChangeTimeMax);

		for (size_t i = 0; i < m_MeshMatSize; ++i)
		{
			m_AnimMesh->SetMetallic(false, 0);

			// Base
			// m_AnimMesh->SetBaseColor(Color, i);

			// Ambient
			// m_AnimMesh->SetAmbientColor(Color, i);

			// Specular
			m_AnimMesh->SetSpecularColor(Vector4::Black, (int)i);

			// Emmisive
			m_AnimMesh->SetEmissiveColor(Color, (int)i);
		}

		if (m_DeathTimer >= m_DeathColorChangeTimeMax)
		{
			m_DeathColorChangeStart = false;
		}
	}
}

void CMonsterDataComponent::OnEndAnimPostAttackDelayOff()
{
	m_PostAttackDelaying = false;

	SetCurrentNodeNull();
}

void CMonsterDataComponent::OnDeadPaperBurnEnd()
{
 	m_Object->Destroy();

	CDDSceneMode* SceneMode = dynamic_cast<CDDSceneMode*>(m_Scene->GetSceneMode());

	if (SceneMode)
	{
		SceneMode->OnDieMonster();
	}
}

void CMonsterDataComponent::OnDeadAnimStart()
{
	// DeathChangeColor() �� ����ϴ� ���
	m_DeathColorChangeStart = true;

	CGameObject* Player = m_Object->GetScene()->GetPlayerObject();

	// �÷��̾��� ���ݿ� ���� ������Ʈ���� DataComponent�� ��Ƴ��� m_CollisionObjDataList���� ������ �����
	CPlayerNormalAttackCheckCollider* AttackCollider = Player->FindComponentFromType<CPlayerNormalAttackCheckCollider>();
	AttackCollider->DeleteObjectData(this);

}

void CMonsterDataComponent::OnDeadAnimEnd()
{
	// Death �ִϸ��̼��� ������ PaperBurn�� �Ҵ�.
	m_PaperBurn->StartPaperBurn();

	// Emissive
	size_t Size = m_AnimMesh->GetMaterialSlotCount();

	for (int i = 0; i < Size; ++i)
	{
		m_AnimMesh->GetMaterial(i)->SetEmissiveColor(1.f, 1.f, 1.f, 1.f);
	}
}

void CMonsterDataComponent::OnPlayerEnterZone(const CollisionResult& Result)
{
	if (m_PlayerEnterZoneTrigger)
	{
		m_PlayerEnterZoneTrigger->Enable(false);
	}

	m_PlayerEnterZone = true;

 //	if (m_CutSceneCam)
 //	{
 //		m_Scene->GetCameraManager()->KeepCamera();
 //		m_Scene->GetCameraManager()->SetCurrentCamera(m_CutSceneCam);
 //	}
 }

void CMonsterDataComponent::OnStartCutScene()
{
	CGameObject* PlayerObj = m_Scene->GetPlayerObject();

	// �÷��̾� �Է� ���� �ʰ� �ϰ�, �ִϸ��̼� ���� ��ȭ
	if (PlayerObj)
	{
		CGameStateComponent* PlayerState = PlayerObj->FindComponentFromType<CGameStateComponent>();
		CAnimationSequenceInstance* PlayerAnim = PlayerObj->FindComponentFromType<CAnimationMeshComponent>()->GetAnimationInstance();

		PlayerAnim->ChangeAnimation("Idle");
		PlayerState->SetTreeUpdate(false);
	}

	m_IsCutScenePlaying = true;

	// �ƽ� ī�޶� �̵� ������ �ִ���
	bool IsCutSceneMove = m_CutSceneCam->GetMoveDataCount() > 0 ? true : false;

	if (IsCutSceneMove)
	{
		m_CutSceneCam->StartCutSceneMove();
	}
	else
	{
		m_Scene->GetCameraManager()->KeepCamera();
		m_Scene->GetCameraManager()->SetCurrentCamera(m_CutSceneCam);
	}
}

void CMonsterDataComponent::OnEndCutScene()
{
	m_IsCutScenePlaying = false;

	// ���� ���� on
	OnCombatStart();

	if (m_CutSceneCam)
	{
		m_Scene->GetCameraManager()->ReturnCamera();
	}

	// �÷��̾� ���� On
	CGameObject* PlayerObj = m_Scene->GetPlayerObject();

	if (PlayerObj)
	{
		CGameStateComponent* PlayerState = PlayerObj->FindComponentFromType<CGameStateComponent>();

		PlayerState->SetTreeUpdate(true);
	}
}

void CMonsterDataComponent::OnActivateBloodParticle()
{
	if (!m_BloodParticle)
		return;

	// y ������ ��¦ ���� �����ǰ� �Ѵ�.
	const Vector3& ObjectWorldPos = m_Object->GetWorldPos();

	// m_BloodParticle->SetWorldPos(ObjectWorldPos + Vector3(0.f, 2.f, 0.f));
	// 
	// // Enable True �� ���Ѿ��༭ �ٽ� ���۵ǰ� �Ѵ�.
	// m_BloodParticle->Enable(true);
	// 
	// m_BloodParticle->RecreateOnlyOnceCreatedParticle();
	m_BloodParticle->StartParticle(ObjectWorldPos);
}

void CMonsterDataComponent::OnActivateSpawnParticle()
{
	if (!m_SpawnParticle)
		return;

	const Vector3& ObjectWorldPos = m_Object->GetWorldPos();

	m_SpawnParticle->StartParticle(ObjectWorldPos);
}

void CMonsterDataComponent::OnInActivateSpawnParticle()
{
	if (!m_SpawnParticle)
		return;

	m_SpawnParticle->CRef::Enable(false);
}

void CMonsterDataComponent::EnableAttackCoolDelay()
{
	m_AttackCoolTimeEnable = true;

	m_AttackCoolDelayTime = m_AttackCoolDelayTimeMax;
}

void CMonsterDataComponent::SetIsHit(bool Hit)
{
	CObjectDataComponent::SetIsHit(Hit);

	if (Hit)
	{
		if (!m_HitEffectStart)
		{
			m_HitEffectFlag = 0;
			m_HitEffectTimer = 0.f;
			m_HitEffectStart = true;
		}

		// Blood Particle �� ���۽�Ų��.
		OnActivateBloodParticle();

		// �߰������� Hit �� ȣ���� �Լ��� ȣ��
	}
}

void CMonsterDataComponent::SetCurrentNodeNull()
{
	m_State->GetBehaviorTree()->SetCurrentNode(nullptr);
}

void CMonsterDataComponent::OnActiveMeleeAttackCollider()
{
	m_MeleeAttackCollider->Enable(true);
}

void CMonsterDataComponent::OnInActiveMeleeAttackCollider()
{
	m_MeleeAttackCollider->Enable(false);
}

void CMonsterDataComponent::OnHitMeleeAttack(const CollisionResult& Result)
{
	// OBJ�� �߰� (Result �� Dest �� Object �� PlayerObject �϶��� �ش� CallBack�� ���ؼ� Player ���� Damage�� �־�� �Ѵ�.)
	CGameObject* Player = m_Scene->GetPlayerObject();

	if (Result.Dest->GetGameObject() != Player)
		return;

	if (m_PlayerData)
	{
 		m_PlayerData->DecreaseHP(1);
	}
}

void CMonsterDataComponent::OnEndAnimPostAttackDelayOn()
{
	SetCurrentNodeNull();

	m_PostAttackDelaying = true;
}

float CMonsterDataComponent::GetAnglePlayer()
{
	CGameObject* PlayerObj = m_Scene->GetPlayerObject();

	if (!PlayerObj)
	{
		return 0.f;
	}

	Vector3 PlayerPos = PlayerObj->GetWorldPos();
	Vector3 MyPos = m_Object->GetWorldPos();

	Vector3 ToPlayer = PlayerPos - MyPos;

	Vector3 MyAxisZ = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;

	float Angle = ToPlayer.Angle(MyAxisZ);

	return Angle;
}

bool CMonsterDataComponent::IsPlayerLeftBasedInLookDir()
{
	/*
	Vector3 vToPlayer = ToPlayer();
	// Vector3 vWorldLook = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
	Vector3 vLook = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
	Vector3 vWorldLook = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
	Vector3 vWorldRight = m_Object->GetWorldAxis(AXIS::AXIS_X) * -1.f;

	// Monster �� �����ʿ� Player ������ ���͸� ����
	float ProjToMonsterWorldRight = vToPlayer.Dot(vWorldRight);

	if (ProjToMonsterWorldRight < 0.f)
		return true;
	else
		return false;
		*/

	Vector3 vToPlayer = ToPlayer();
	Vector3 vLook = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;

	Vector3 vCross = vToPlayer.Cross(vLook);

	if (vCross.y < 0.f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CMonsterDataComponent::OnEnableLookPlayer()
{
	m_LookPlayer = true;
}

void CMonsterDataComponent::OnDisableLookPlayer()
{
	m_LookPlayer = false;
}

void CMonsterDataComponent::OnEnableRightLookPlayer()
{
	m_RightLookPlayer = true;
}

void CMonsterDataComponent::OnDisableRightLookPlayer()
{
	m_RightLookPlayer = false;
}

void CMonsterDataComponent::OnEnableLeftLookPlayer()
{
	m_LeftLookPlayer = true;
}

void CMonsterDataComponent::OnDisableLeftLookPlayer()
{
	m_LeftLookPlayer = false;
}

void CMonsterDataComponent::OnEnableMoveZ()
{
	m_MoveZ = true;
}

void CMonsterDataComponent::OnDisableMoveZ()
{
	m_MoveZ = false;
}

void CMonsterDataComponent::OnResetPrevMoveZ()
{
	m_MoveZ = m_PrevMoveZ;
}

void CMonsterDataComponent::OnKeepPrevMoveZ()
{
	m_PrevMoveZ = m_MoveZ;
}

void CMonsterDataComponent::OnEnableRandomRotate()
{
	m_RotateToRandom = true;
}

bool CMonsterDataComponent::IsPlayerInMeleeAttackRange()
{
	CGameObject* PlayerObj = m_Scene->GetPlayerObject();

	if (!PlayerObj)
	{
		return false;
	}

	Vector3 MyPos = m_Object->GetWorldPos();
	Vector3 PlayerPos = PlayerObj->GetWorldPos();
	float Dist = MyPos.Distance(PlayerPos);

	if (Dist <= m_Data.MeleeAttackRange)
	{
		return true;
	}

	return false;
}

Vector3 CMonsterDataComponent::ToPlayer()
{
	CGameObject* PlayerObj = m_Scene->GetPlayerObject();

	if (!PlayerObj)
	{
		return Vector3::Zero;
	}

	Vector3 PlayerPos = PlayerObj->GetWorldPos();
	Vector3 MyPos = m_Object->GetWorldPos();

	Vector3 ToP = PlayerPos - MyPos;
	ToP.Normalize();
	return ToP;
}

float CMonsterDataComponent::DistToPlayer()
{
	CGameObject* PlayerObj = m_Scene->GetPlayerObject();

	if (!PlayerObj)
	{
		return -1.f;
	}

	Vector3 MyPos = m_Object->GetWorldPos();
	Vector3 PlayerPos = PlayerObj->GetWorldPos();
	float Dist = MyPos.Distance(PlayerPos);

	return MyPos.Distance(PlayerPos);;
}

bool CMonsterDataComponent::IsPlayerInStopChaseRange()
{
	CGameObject* PlayerObj = m_Scene->GetPlayerObject();

	if (!PlayerObj)
	{
		return false;
	}

	Vector3 MyPos = m_Object->GetWorldPos();
	Vector3 PlayerPos = PlayerObj->GetWorldPos();
	float Dist = MyPos.Distance(PlayerPos);

	// �⺻������ Start���� StopChaseRange�� MeleeAttackRange�� ����
	// ChasePlyerNode���� ���
	if (Dist <= m_StopChaseRange)
	{
		return true;
	}

	return false;
}

void CMonsterDataComponent::SetRotateRandom()
{
	m_RotateToRandom = true;

	// ���� MoveZ ������ �����صд�.
	m_RandomRotatePrevMoveZ = m_MoveZ;

	// Rotate �ϴ� ���ȿ��� MoveZ �� False �� ������� ���̴�
	m_MoveZ = false;

	// Rotate �ϴ� ���ȿ��� �������� �ʵ��� ������ ���̴�.
	m_RotateRandomAngle = ((float)(rand()) / RAND_MAX) * 360.f;
}

void CMonsterDataComponent::RotateRandomly(float DeltaTime)
{
	Vector3 MyPos = m_Object->GetWorldPos();

	float RotSpeed = m_Data.RotateSpeedPerSec;

	// ���� m_CurRotSpeed �� ������ ������ ���¶��
	if (m_CurRotSpeed != 0.f)
	{
		RotSpeed = m_CurRotSpeed;
	}

	float RotAmount = RotSpeed * DeltaTime;

	m_RotateRandomAngle -= RotAmount;

	if (m_RotateRandomAngle < 0.f)
	{
		m_RotateToRandom = false;

		m_MoveZ = m_RandomRotatePrevMoveZ;

		return;
	}

	m_Object->AddWorldRotationY(RotAmount);
}

void CMonsterDataComponent::DecreaseHP(int Amount)
{
	CObjectDataComponent::DecreaseHP(Amount);

	if (m_Data.HP <= 0)
	{
		m_HitBox->Enable(false);

		if (m_MeleeAttackCollider)
		{
			m_MeleeAttackCollider->Enable(false);
		}
	}
}

bool CMonsterDataComponent::Save(FILE* File)
{
	CObjectDataComponent::Save(File);

	return true;
}

bool CMonsterDataComponent::Load(FILE* File)
{
	CObjectDataComponent::Load(File);

	return true;
}

bool CMonsterDataComponent::SaveOnly(FILE* File)
{
	CObjectDataComponent::SaveOnly(File);

	return true;
}

bool CMonsterDataComponent::LoadOnly(FILE* File)
{
	CObjectDataComponent::LoadOnly(File);

	return true;
}

void CMonsterDataComponent::ActivateHitSubEffect()
{
}

void CMonsterDataComponent::ActiveHitEffect(float DeltaTime)
{
	m_HitEffectTimer += DeltaTime;

	// ������ �Ǿ��� �� �÷� ü������ �� ���� ����
	if ((m_HitEffectFlag & HIT_EFFECT_FIRST) == 0 && ((m_HitEffectTimer <= m_HitEffectMax / 5.f)))
	{
		m_HitEffectFlag |= HIT_EFFECT_FIRST;
		ChangeHitColor(HIT_EFFECT_FIRST);
		return;
	}
	else if ((m_HitEffectFlag & HIT_EFFECT_SECOND) == 0 && (m_HitEffectTimer >= ((m_HitEffectMax * 2.f / 5.f))))
	{
		m_HitEffectFlag |= HIT_EFFECT_SECOND;
		ChangeHitColor(HIT_EFFECT_SECOND);
		return;
	}
	else if ((m_HitEffectFlag & HIT_EFFECT_THIRD) == 0  && (m_HitEffectTimer >= ((m_HitEffectMax * 3.f / 5.f))))
	{
		m_HitEffectFlag |= HIT_EFFECT_THIRD;
		ChangeHitColor(HIT_EFFECT_THIRD);
		return;
	}
	else if ((m_HitEffectFlag & HIT_EFFECT_FOURTH) == 0 && m_HitEffectTimer >= ((m_HitEffectMax * 4.f / 5.f)))
	{
		m_HitEffectFlag |= HIT_EFFECT_FOURTH;
		ChangeHitColor(HIT_EFFECT_FOURTH);
		return;
	}
	else if ((m_HitEffectFlag & HIT_EFFECT_FIFTH) == 0 && m_HitEffectTimer >= m_HitEffectMax)
	{
		m_HitEffectFlag |= HIT_EFFECT_FIFTH;
		ChangeHitColor(HIT_EFFECT_FIFTH);
		return;
	}
	else if (m_HitEffectTimer >= m_HitEffectMax + (m_HitEffectMax / 5.f))
	{
		m_HitEffectStart = false;
		m_HitEffectTimer = 0.f;
		m_HitEffectFlag = 0;

		// ���� �÷��� ���ƿ´�.
		for (int i = 0; i < m_MeshMatSize; ++i)
		{
			m_AnimMesh->GetMaterial(i)->SetBaseColor(m_vecOriginDiffuse[i]);
			m_AnimMesh->GetMaterial(i)->SetAmbientColor(m_vecOriginAmbient[i]);
			m_AnimMesh->GetMaterial(i)->SetSpecularColor(m_vecOriginSpecluar[i]);
			m_AnimMesh->GetMaterial(i)->SetEmissiveColor(m_vecOriginEmissive[i]);
		}
	}
}

void CMonsterDataComponent::ChangeHitColor(int EffectNum)
{
	// ��
	if (EffectNum == HIT_EFFECT_FIRST)
	{
		for (int i = 0; i < m_MeshMatSize; ++i)
		{
			m_AnimMesh->GetMaterial(i)->SetEmissiveColor(Vector4::White);
			m_AnimMesh->GetMaterial(i)->SetBaseColor(Vector4::Black);
			m_AnimMesh->GetMaterial(i)->SetAmbientColor(Vector4::Black);
			m_AnimMesh->GetMaterial(i)->SetSpecularColor(Vector4::Black);
		}
	}
	// ��
	else if (EffectNum == HIT_EFFECT_SECOND || EffectNum == HIT_EFFECT_FOURTH)
	{
		for (int i = 0; i < m_MeshMatSize; ++i)
		{
			m_AnimMesh->GetMaterial(i)->SetEmissiveColor(Vector4::Black); //
			m_AnimMesh->GetMaterial(i)->SetBaseColor(Vector4::Black);
			m_AnimMesh->GetMaterial(i)->SetAmbientColor(Vector4::Black);
			m_AnimMesh->GetMaterial(i)->SetSpecularColor(Vector4::Black);
		}
	}
	// ��
	else if (EffectNum == HIT_EFFECT_THIRD || EffectNum == HIT_EFFECT_FIFTH)
	{
		for (int i = 0; i < m_MeshMatSize; ++i)
		{
			m_AnimMesh->GetMaterial(i)->SetEmissiveColor(Vector4::Red);
			m_AnimMesh->GetMaterial(i)->SetBaseColor(Vector4::Black);
			m_AnimMesh->GetMaterial(i)->SetAmbientColor(Vector4::Black);
			m_AnimMesh->GetMaterial(i)->SetSpecularColor(Vector4::Black);
		}
	}
}

CMonsterNavAgent* CMonsterDataComponent::GetMonsterNavAgent() const
{
	return m_MonsterNavAgent;
}

CColliderBox3D* CMonsterDataComponent::GetHitBox() const
{
	return m_HitBox;
}
