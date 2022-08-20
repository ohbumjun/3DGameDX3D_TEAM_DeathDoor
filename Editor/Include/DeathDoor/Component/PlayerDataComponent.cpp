
#include "PlayerDataComponent.h"
#include "Input.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Component/AnimationMeshComponent.h"
#include "../DataManager.h"
#include "GameStateComponent.h"
#include "Component/ColliderComponent.h"
#include "Component/ColliderBox3D.h"
#include "../Component/PlayerNormalAttackCheckCollider.h"
#include "../Component/PlayerBombComponent.h"
#include "Scene/Navigation3DManager.h"
#include "Component/StaticMeshComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/PaperBurnComponent.h"

CPlayerDataComponent::CPlayerDataComponent() :
	m_OnSlash(false),
	m_AnimComp(nullptr),
	m_Body(nullptr),
	m_FrameCount(0),
	m_MouseLButtonDown(false),
	m_Unbeatable(false),
	m_UnbeatableAccTime(0.f),
	m_UnbeatableTime(2.7f),
	m_LadderUpEnable(false),
	m_LadderDownEnable(false),
	m_IsClimbingLadder(false),
	m_AdjLadder(nullptr),
	m_Slash(nullptr),
	m_Sword(nullptr),
	m_CurrentDustIndex(0),
	m_SlashPaperBurn(nullptr),
	m_ConsecutiveAttackCount(0),
	m_HitBackUnbeatable(false),
	m_HitBackUnbeatableTime(1.2f),
	m_HitBackUnbeatableAccTime(0.f),
	m_AccHPChargeTime(0.f),
	m_HPChargeTime(3.f)
{
	SetTypeID<CPlayerDataComponent>();
	m_ComponentType = Component_Type::ObjectComponent;

	m_PlayerData.Abilty_Type = Player_Ability::Arrow;
}

CPlayerDataComponent::CPlayerDataComponent(const CPlayerDataComponent& com)	:
	CObjectDataComponent(com)
{
}

CPlayerDataComponent::~CPlayerDataComponent()
{
}

void CPlayerDataComponent::Start()
{
	m_Scene->GetSceneMode()->SetPlayerObject(m_Object);

	CInput::GetInst()->CreateKey("WeaponArrow", '1');
	//CInput::GetInst()->CreateKey("WeaponFire", '2');
	CInput::GetInst()->CreateKey("WeaponChain", '3');
	CInput::GetInst()->CreateKey("WeaponBomb", '4');
	CInput::GetInst()->CreateKey("None", '5');

	CInput::GetInst()->CreateKey("MoveForward", 'W');
	CInput::GetInst()->CreateKey("MoveBack", 'S');
	CInput::GetInst()->CreateKey("MoveLeft", 'A');
	CInput::GetInst()->CreateKey("MoveRight", 'D');
	CInput::GetInst()->CreateKey("Roll", VK_SPACE);

	CInput::GetInst()->SetKeyCallback("WeaponArrow", KeyState_Down, this, &CPlayerDataComponent::SetPlayerAbilityArrow);
	CInput::GetInst()->SetKeyCallback("WeaponFire", KeyState_Down, this, &CPlayerDataComponent::SetPlayerAbilityFire);
	CInput::GetInst()->SetKeyCallback("WeaponChain", KeyState_Down, this, &CPlayerDataComponent::SetPlayerAbilityChain);
	CInput::GetInst()->SetKeyCallback("WeaponBomb", KeyState_Down, this, &CPlayerDataComponent::SetPlayerAbilityBomb);
	CInput::GetInst()->SetKeyCallback("None", KeyState_Down, this, &CPlayerDataComponent::SetSkillNone);

	m_AnimComp = m_Object->FindComponentFromType<CAnimationMeshComponent>();

	// Player Melee Attack Check Collider
	m_AttackCheckCollider = m_Object->FindComponentFromType<CPlayerNormalAttackCheckCollider>();
	m_AttackCheckCollider->Enable(false);

	// Player Animation Notify는 여기 추가
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerSlashL", "PlayerSlashL", 1, this, &CPlayerDataComponent::SetTrueOnSlash);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerSlashL", "PlayerSlashL", 7, this, &CPlayerDataComponent::SetFalseOnSlash);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerSlashR", "PlayerSlashR", 1, this, &CPlayerDataComponent::SetTrueOnSlash);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerSlashR", "PlayerSlashR", 7, this, &CPlayerDataComponent::SetFalseOnSlash);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerHitBack", "PlayerHitBack", 0, this, &CPlayerDataComponent::OnHitBack);

	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerBomb", "PlayerBomb", 3, this, &CPlayerDataComponent::OnBombLift);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerBomb", "PlayerBombCountReset", 4, this, &CPlayerDataComponent::OnBombCountReset);

	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerRun", "MoveDustOn", 10, this, &CPlayerDataComponent::OnResetDustParticle);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerRun", "MoveDustOn", 30, this, &CPlayerDataComponent::OnResetDustParticle);

	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerRoll", "PlayerRoll", 0, this, &CPlayerDataComponent::OnRoll);

	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerLadderUp", "PlayerLadderUpStepSoundPlay1", 6, this, &CPlayerDataComponent::OnLadderStepSoundPlay);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerLadderUp", "PlayerLadderUpStepSoundPlay2", 25, this, &CPlayerDataComponent::OnLadderStepSoundPlay);
	//m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerLadderUp", "PlayerLadderUpStepSoundPlay3", 28, this, &CPlayerDataComponent::OnLadderStepSoundPlay);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerLadderDown", "PlayerLadderDownStepSoundPlay1", 6, this, &CPlayerDataComponent::OnLadderStepSoundPlay);
	m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerLadderDown", "PlayerLadderDownStepSoundPlay2", 25, this, &CPlayerDataComponent::OnLadderStepSoundPlay);
	//m_AnimComp->GetAnimationInstance()->AddNotify<CPlayerDataComponent>("PlayerLadderDown", "PlayerLadderDownStepSoundPlay3", 25, this, &CPlayerDataComponent::OnLadderStepSoundPlay);
	
	m_AnimComp->GetAnimationInstance()->SetEndFunction<CPlayerDataComponent>("PlayerHitBack", this, &CPlayerDataComponent::OnHitBackEnd);
	m_AnimComp->GetAnimationInstance()->SetEndFunction<CPlayerDataComponent>("PlayerHitRecover", this, &CPlayerDataComponent::OnHitRecoverEnd);
	m_AnimComp->GetAnimationInstance()->SetEndFunction<CPlayerDataComponent>("PlayerRoll", this, &CPlayerDataComponent::OnRollEnd);


	m_Data = CDataManager::GetInst()->GetObjectData("Player");
	m_Body = (CColliderComponent*)m_Object->FindComponent("Body");

	m_Slash = (CStaticMeshComponent*)m_Object->FindComponent("Slash");

	if(m_Slash)
		m_Slash->Enable(false);


	m_SlashDir = m_Object->GetWorldAxis(AXIS_Z);
	m_SlashDir *= 1.f;

	if (m_Slash)
	{
		// x는 xz평면에 평행하게 하려고 100.f으로 맞춘거고 나머지 y,z의 초기 설정값을 0으로 relative rotation을 맞춰줘야한다
		m_Slash->SetRelativeRotation(100.f, 0.f, 0.f);
	}

	m_Sword = (CAnimationMeshComponent*)m_Object->FindComponent("SwordAnim");

	m_Object->FindAllSceneComponentFromTypeName<CParticleComponent>("Dust", m_vecMoveDust);

	CCameraComponent* Cam = m_Object->FindComponentFromType<CCameraComponent>();

	if (Cam)
	{
		Vector3 RootPos = m_AnimComp->GetWorldPos();
		Vector3 CamPos = Cam->GetWorldPos();
		m_CamRelativePos = CamPos - RootPos;
	}

	m_SlashPaperBurn = m_Object->FindObjectComponentFromType<CPaperBurnComponent>();

	SetPlayerAbilityArrow(0.f);
}

bool CPlayerDataComponent::Init()
{
	return true;
}

void CPlayerDataComponent::Update(float DeltaTime)
{
	m_FrameCount = 0;

	CGameStateComponent* Comp = m_Object->FindObjectComponentFromType<CGameStateComponent>();

	if (Comp->IsTreeUpdate())
	{
		//++m_FrameCount;
		bool LButtonDown = CInput::GetInst()->GetMouseLButtonClick();
		bool LButtonUp = CInput::GetInst()->GetMouseLButtonUp();

		// 
		if (LButtonDown && !m_MouseLButtonDown && !LButtonUp)
		{
			m_MouseLButtonDown = true;

			m_KeyStateQueue.push(VK_LBUTTON);

		}

		if (LButtonUp)
			m_MouseLButtonDown = false;

		if (m_Unbeatable)
		{
			if (m_UnbeatableAccTime > m_UnbeatableTime)
			{
				CColliderBox3D* BodyCollider = (CColliderBox3D*)m_Object->FindComponent("Body");

				if (BodyCollider)
					BodyCollider->SetRigidCollisionIgnore(false);

				m_Unbeatable = false;
				m_UnbeatableAccTime = 0.f;
				return;
			}

			m_IsHit = false;

			m_UnbeatableAccTime += DeltaTime;
		}

		else if (m_HitBackUnbeatable)
		{
			if (m_HitBackUnbeatableAccTime > m_HitBackUnbeatableTime)
			{
				m_Body->Enable(true);

				m_IsHit = false;

				m_HitBackUnbeatable = false;
				m_HitBackUnbeatableAccTime = 0.f;
				return;
			}

			m_IsHit = false;

			m_HitBackUnbeatableAccTime += DeltaTime;
		}
	}

	if (m_HPChargeTimeStart)
	{
		if (m_AccHPChargeTime >= m_HPChargeTime)
		{
			CUIManager::GetInst()->IncreaseHP();

			int HP = m_Data.HP;
			++HP;
			m_Data.HP = HP;

			m_AccHPChargeTime = 0.f;
			m_HPChargeTimeStart = false;

			if (m_Data.HP < 5)
				m_HPChargeTimeStart = true;
		}

		m_AccHPChargeTime += DeltaTime;
	}

	m_SlashPaperBurn = m_Object->FindObjectComponentFromType<CPaperBurnComponent>();
}

void CPlayerDataComponent::PostUpdate(float DeltaTime)
{
}

void CPlayerDataComponent::PrevRender()
{
}

void CPlayerDataComponent::Render()
{
}

void CPlayerDataComponent::PostRender()
{
}

CPlayerDataComponent* CPlayerDataComponent::Clone()
{
	return new CPlayerDataComponent(*this);
}

bool CPlayerDataComponent::Save(FILE* File)
{
	CObjectDataComponent::Save(File);

	fwrite(&m_PlayerData, sizeof(PlayerData), 1, File);

	return true;
}

bool CPlayerDataComponent::Load(FILE* File)
{
	CObjectDataComponent::Load(File);

	fread(&m_PlayerData, sizeof(PlayerData), 1, File);

	return true;
}

bool CPlayerDataComponent::SaveOnly(FILE* File)
{
	CObjectDataComponent::Save(File);

	fwrite(&m_PlayerData, sizeof(PlayerData), 1, File);

	return true;
}

bool CPlayerDataComponent::LoadOnly(FILE* File)
{
	CObjectDataComponent::Load(File);

	fread(&m_PlayerData, sizeof(PlayerData), 1, File);

	return true;
}

void CPlayerDataComponent::SetAdjLadder(CGameObject* Ladder)
{
	m_AdjLadder = Ladder;
}

inline void CPlayerDataComponent::SetTrueOnSlash()
{
	m_OnSlash = true;

	m_AttackCheckCollider->Enable(true);
	m_Slash->Enable(true);

	if (m_SlashPaperBurn)
	{
		m_SlashPaperBurn->SetEndEvent(PaperBurnEndEvent::Disable);
		m_SlashPaperBurn->StartPaperBurn();
	}
}

inline void CPlayerDataComponent::SetFalseOnSlash()
{
	m_OnSlash = false;

	m_AttackCheckCollider->Enable(false);
	//m_Slash->Enable(false);
}

CAnimationMeshComponent* CPlayerDataComponent::GetSword() const
{
	return m_Sword;
}

CGameObject* CPlayerDataComponent::GetAdjLadder() const
{
	return m_AdjLadder;
}

CAnimationMeshComponent* CPlayerDataComponent::GetAnimationMeshComponent() const
{
	return m_AnimComp;
}

void CPlayerDataComponent::OnHitBack()
{
	m_Body->Enable(false);
	m_NoInterrupt = true;

	m_Object->GetScene()->GetResource()->SoundPlay("EnemyHit1");

	m_HPChargeTimeStart = true;
}

void CPlayerDataComponent::OnHitBackEnd()
{
	m_AnimComp->GetAnimationInstance()->ChangeAnimation("PlayerHitRecover");
	m_HitBackUnbeatable = true;
}

void CPlayerDataComponent::OnHitRecoverEnd()
{
	//m_Body->Enable(true);

	//m_NoInterrupt = false;
	m_IsHit = false;
	m_NoInterrupt = false;
}

void CPlayerDataComponent::OnRoll()
{
	//m_Body->Enable(false);
	m_Unbeatable = true;

	m_Body->SetRigidCollisionIgnore(true);
}

void CPlayerDataComponent::OnRollEnd()
{
	//m_Body->Enable(true);
	//m_Unbeatable = false;

	m_Body->SetRigidCollisionIgnore(false);
}

void CPlayerDataComponent::OnBombLift()
{
	CPlayerBombComponent* BombComp = m_Object->FindObjectComponentFromType<CPlayerBombComponent>();

	if (BombComp)
	{
		BombComp->SetLiftSpeed(0.1f);
		BombComp->LiftBomb();
	}
}

void CPlayerDataComponent::OnBombCountReset()
{
	CPlayerBombComponent* BombComp = m_Object->FindObjectComponentFromType<CPlayerBombComponent>();

	if (BombComp)
	{
		BombComp->ResetCount();
	}
}


void CPlayerDataComponent::ForceUpdateAttackDirection()
{
	CNavigation3DManager* Manager = m_Object->GetScene()->GetNavigation3DManager();

	Vector3 TargetPos;
	Vector3 ZAxis = m_Object->GetWorldAxis(AXIS::AXIS_Z);

	Vector3 CurrentForwardVector = Vector3(-ZAxis.x, 0.f, -ZAxis.z);
	Vector3 DestForwardVector;
	Vector3 PickingPoint;
	Vector3 CrossVec;
	bool Over180;

	if (Manager->CheckNavMeshPickingPoint(PickingPoint))
	{
		Vector3 CurrentPos = m_Object->GetWorldPos();
		DestForwardVector = Vector3(PickingPoint.x, 0.f, PickingPoint.z) - Vector3(CurrentPos.x, 0.f, CurrentPos.z);
		DestForwardVector.Normalize();

		CrossVec = Vector3(CurrentForwardVector.x, CurrentForwardVector.y, CurrentForwardVector.z).Cross(Vector3(DestForwardVector.x, DestForwardVector.y, -DestForwardVector.z));

		// 반시계 방향으로 180도가 넘는다
		if (CrossVec.y < 0)
			Over180 = true;
		else
			Over180 = false;
	}

	else
	{
		PickingPoint = m_Object->GetWorldPos();
		DestForwardVector = CurrentForwardVector;
		return;
	}

	//////

	Vector3 CurrentFowardYZero = Vector3(CurrentForwardVector.x, 0.f, CurrentForwardVector.z);
	Vector3 DestForwardYZero = Vector3(DestForwardVector.x, 0.f, DestForwardVector.z);

	CrossVec = Vector3(CurrentFowardYZero.x, 0.f, CurrentFowardYZero.z).Cross(Vector3(DestForwardYZero.x, 0.f, DestForwardYZero.z));

	// 반시계 방향으로 180도가 넘는다
	if (CrossVec.y < 0)
		Over180 = true;
	else
		Over180 = false;

	float DotProduct = CurrentFowardYZero.Dot(DestForwardYZero);

	if (DotProduct > 0.99f || DotProduct < -0.99f)
	{
		return;
	}

	float Rad = acosf(DotProduct);
	float Degree = RadianToDegree(Rad);

	DestForwardVector.Normalize();

	// CurretForwardVector기준 DestForwardVector로 시계 방향으로 180도가 넘는다면
	if (Over180)
	{
		m_Object->AddWorldRotationY(-Degree);
	}

	else
	{
		m_Object->AddWorldRotationY(Degree);
	}

	m_AttackDir = DestForwardVector;
}

void CPlayerDataComponent::OnResetDustParticle()
{
	if (m_vecMoveDust.size() > 0)
	{
		Vector3 ObjectPos = m_Object->GetWorldPos();
		Vector3 ZDir = m_Object->GetWorldAxis(AXIS_Z);

		if (m_vecMoveDust[m_CurrentDustIndex])
		{
			m_vecMoveDust[m_CurrentDustIndex]->StartParticle(Vector3(ObjectPos.x + ZDir.x, ObjectPos.y + 1.f, ObjectPos.z + ZDir.z));
			//m_vecMoveDust[m_CurrentDustIndex]->RecreateOnlyOnceCreatedParticleWithOutLifeTimeSetting();
		}

		++m_CurrentDustIndex;

		if (m_CurrentDustIndex >= m_vecMoveDust.size())
			m_CurrentDustIndex = 0;
	}
}

void CPlayerDataComponent::OnLadderStepSoundPlay()
{
	m_Object->GetScene()->GetResource()->SoundPlay("LadderStep");
}
