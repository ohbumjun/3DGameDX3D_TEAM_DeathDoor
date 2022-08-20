#include "BossBettyCloseAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Collision/Collision.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../BossBettyDataComponent.h"
#include "BossBettyChangeAttackDirNode.h"
#include "ObjectPool.h"

CBossBettyCloseAttackNode::CBossBettyCloseAttackNode()
{
	SetTypeID(typeid(CBossBettyCloseAttackNode).hash_code());
}

CBossBettyCloseAttackNode::CBossBettyCloseAttackNode(const CBossBettyCloseAttackNode& Node)
{
}

CBossBettyCloseAttackNode::~CBossBettyCloseAttackNode()
{
}

void CBossBettyCloseAttackNode::Init()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	// 1) Slash Left
	std::string AnimName = "SlashLeft";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->AddNotify(AnimName, "DisableLook", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);

	AnimInst->AddNotify(AnimName, "OnEnableRightLookPlayer", 13,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableRightLookPlayer);

	AnimInst->AddNotify(AnimName, "SlashSound", 13,
		Data, &CBossBettyDataComponent::OnBossBettySlashSound);

	AnimInst->AddNotify(AnimName, "OnSlashLeft", 13,
		this, &CBossBettyCloseAttackNode::OnBossBettySlashLeftEffect);

	AnimInst->AddNotify(AnimName, "OnDisableRightLookPlayer", 22,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableRightLookPlayer);

	AnimInst->AddNotify(AnimName, "CameraShake", 19,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);
	// AnimInst->AddNotify(AnimName, "EnableAttackCollider", 18,
	AnimInst->AddNotify(AnimName, "EnableAttackCollider", 14,
		Data, &CBossBettyDataComponent::OnBossBettyEnableAttackCollider);
	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 20,
		Data, &CBossBettyDataComponent::OnBossBettyDisableAttackCollider);

	// >> End
	AnimInst->SetEndFunction(AnimName, 
		this, &CBossBettyCloseAttackNode::OnBossBettyCommonEndFunctionOfCloseAttack);

	// 2) Slash Right
	AnimName = "SlashRight";

	AnimInst->AddNotify(AnimName, "DisableLook", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnEnableLeftLookPlayer", 13,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLeftLookPlayer);
	AnimInst->AddNotify(AnimName, "SlashSound", 13,
		Data, &CBossBettyDataComponent::OnBossBettySlashSound);
	AnimInst->AddNotify(AnimName, "OnSlashRight", 13,
		this, &CBossBettyCloseAttackNode::OnBossBettySlashRightEffect);
	AnimInst->AddNotify(AnimName, "OnDisableLeftLookPlayer", 22,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLeftLookPlayer);

	AnimInst->AddNotify(AnimName, "EnableAttackCollider", 14,
		Data, &CBossBettyDataComponent::OnBossBettyEnableAttackCollider);
	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 20,
		Data, &CBossBettyDataComponent::OnBossBettyDisableAttackCollider);
	AnimInst->AddNotify(AnimName, "CameraShake", 18,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// >> End
	AnimInst->SetEndFunction(AnimName,
		this, &CBossBettyCloseAttackNode::OnBossBettyCommonEndFunctionOfCloseAttack);

	// 3) PunchLeft
	AnimName = "PunchLeft";

	AnimInst->AddNotify(AnimName, "DisableLook", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnPunchLeft", 15,
		Data, &CBossBettyDataComponent::OnBossBettyGenerateLeftCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackDownSound", 15,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "CameraShake", 18,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);
	AnimInst->AddNotify(AnimName, "EnableAttackCollider", 17,
		Data, &CBossBettyDataComponent::OnBossBettyEnableAttackCollider);
	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 22,
		Data, &CBossBettyDataComponent::OnBossBettyDisableAttackCollider);

	AnimInst->SetEndFunction(AnimName,
		this, &CBossBettyCloseAttackNode::OnBossBettyCommonEndFunctionOfCloseAttack);

	// 4) PunchRight
	AnimName = "PunchRight";

	AnimInst->AddNotify(AnimName, "DisableLook", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnPunchRight", 15,
		Data, &CBossBettyDataComponent::OnBossBettyGenerateRightCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackDownSound", 15,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "CameraShake", 18,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);
	AnimInst->AddNotify(AnimName, "EnableAttackCollider", 17,
		Data, &CBossBettyDataComponent::OnBossBettyEnableAttackCollider);
	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 22,
		Data, &CBossBettyDataComponent::OnBossBettyDisableAttackCollider);

	AnimInst->SetEndFunction(AnimName,
		this, &CBossBettyCloseAttackNode::OnBossBettyCommonEndFunctionOfCloseAttack);

	// 5) Two Side Attack
	AnimName = "FirstSlam";

	AnimInst->AddNotify(AnimName, "DisableLook", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnFirstSlam", 17,
		Data, &CBossBettyDataComponent::OnBossBettyGenerateTwoSideCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "CameraShake", 20,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);
	AnimInst->AddNotify(AnimName, "AttackDownSound", 17,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "EnableAttackCollider", 18,
		Data, &CBossBettyDataComponent::OnBossBettyEnableAttackCollider);
	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 24,
		Data, &CBossBettyDataComponent::OnBossBettyDisableAttackCollider);

	AnimInst->SetEndFunction(AnimName,
		this, &CBossBettyCloseAttackNode::OnBossBettyCommonEndFunctionOfCloseAttack);

}

NodeResult CBossBettyCloseAttackNode::OnStart(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	m_Owner->SetCurrentNode(this);

	// 근거리 공격 타입을 체크한다.
	m_CloseAttackType = DetermineBettyCloseAttackType();

	// 원래는 별도의 Condition Node 로 들어가야 하는 것
	if (!Data->IsCloseAttackAnimChangeEnable())
	{
		return NodeResult::Node_True;
	}

	float AngleToPlayer = Data->GetAnglePlayer();

	if (AngleToPlayer > 35.f)
		Data->SetCurRotSpeed(Data->GetOriginRotSpeed() * 1.5f);
	else
		Data->SetCurRotSpeed(Data->GetOriginRotSpeed());

	Data->SetCloseAttackAnimChangeEnable(false);

	switch (m_CloseAttackType)
	{
	case BossBettyCloseAttackType::PunchLeft:
	{
		// m_Owner->GetAnimationMeshComp()->GetAnimationInstance()->ChangeAnimation("IdleBeast");
		AnimInst->ChangeAnimation("PunchLeft");
	}
	break;
	case BossBettyCloseAttackType::PunchRight:
	{	
		// m_Owner->GetAnimationMeshComp()->GetAnimationInstance()->ChangeAnimation("IdleBeast");
		AnimInst->ChangeAnimation("PunchRight");
	}
	break;
	case BossBettyCloseAttackType::SlashLeft:
	{	
		AnimInst->ChangeAnimation("SlashLeft");
	}
	break;
	case BossBettyCloseAttackType::SlashRight:
	{
		AnimInst->ChangeAnimation("SlashRight");
	}
	break;
	case BossBettyCloseAttackType::TwoSideFront:
	{
		AnimInst->ChangeAnimation("FirstSlam");
	}
	break;
	case BossBettyCloseAttackType::NotInCloseRange:
	{
		Data->SetCloseAttackAnimChangeEnable(true);

		m_Owner->SetCurrentNode(nullptr);

		AnimInst->ChangeAnimation("BackUpStep");

		m_Owner->SetCurrentNode(m_Owner->FindNodeByType<CBossBettyChangeAttackDirNode>());

		Data->SetCurRotSpeed(Data->GetOriginRotSpeed());
	}
	break;
	}

	return NodeResult::Node_True;
}

NodeResult CBossBettyCloseAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossBettyCloseAttackNode::OnEnd(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	Data->SetCurRotSpeed(Data->GetOriginRotSpeed());

	return NodeResult::Node_True;
}

BossBettyCloseAttackType CBossBettyCloseAttackNode::DetermineBettyCloseAttackType()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	const Vector3* const& PunchLeftSquarePoses = Data->GetPunchLeftSquarePoses();

	const Vector3& RelativeXAxis = m_Object->GetRootComponent()->GetRelativeAxis(AXIS_X) * -1;
	const Vector3& RelativeZAxis = m_Object->GetRootComponent()->GetRelativeAxis(AXIS_Z) * -1;
	const Vector3& WorldScale = m_Object->GetRootComponent()->GetWorldScale();
	const Vector3& WorldPos = m_Object->GetRootComponent()->GetWorldPos();

	float AnglePlayer = Data->GetAnglePlayer();
	float DistToPlayer = Data->DistToPlayer();
	float MeleeAttackRange = Data->GetMeleeAttackRange();

	// Player 정보
	CGameObject* PlayerObj = CSceneManager::GetInst()->GetScene()->GetPlayerObject();
	Vector2 Player2DWorldPos = Vector2(PlayerObj->GetWorldPos().x, PlayerObj->GetWorldPos().z);
	Vector2 Betty2DWorldPos = Vector2(WorldPos.x, WorldPos.z);

	// 1) Box2DInfo m_PunchLeftSquareBox;
	if (DistToPlayer < MeleeAttackRange * 0.8f && AnglePlayer < 45.f && Data->IsPlayerLeftBasedInLookDir() == false)
	{
		return BossBettyCloseAttackType::PunchLeft;
	}
	// 2) Box2DInfo m_PunchRightSquareBox;
	else if (DistToPlayer < MeleeAttackRange * 0.8f && AnglePlayer < 45.f && Data->IsPlayerLeftBasedInLookDir())
	{
		return BossBettyCloseAttackType::PunchRight;
	}
	// 3) Box2DInfo m_SlashLeftSquareBox;
	else if (DistToPlayer < MeleeAttackRange * 0.75f&& 
		AnglePlayer >= 45.f && 
		AnglePlayer < 135 &&
		Data->IsPlayerLeftBasedInLookDir())
	{
		return BossBettyCloseAttackType::SlashRight;
	}
	// 4) Box2DInfo m_SlashRightSquareBox;
	else if (DistToPlayer < MeleeAttackRange * 0.75f && 
		AnglePlayer >=  45.f &&
		AnglePlayer < 135 &&
		Data->IsPlayerLeftBasedInLookDir() == false)
	{
		return BossBettyCloseAttackType::SlashLeft;
	}
	// 5) Two Down
	else if (DistToPlayer < MeleeAttackRange * 1.2f &&
		DistToPlayer >= MeleeAttackRange * 0.8f &&
		AnglePlayer < 45.f)
	{
		return BossBettyCloseAttackType::TwoSideFront;
	}

	return BossBettyCloseAttackType::NotInCloseRange;
}

void CBossBettyCloseAttackNode::OnBossBettySlashLeftEffect()
{
	// 왼쪽으로 45 도 회전하기 (이것은 다른 Callback 에서 수행중이다)
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	const Vector3& SlashLeftSpawnPoint = Data->GetRelativeSlashLeftPos();
	
	// 1.바로 왼쪽 해당 위치에 Collider 생성하기 
	// 2. 연속적으로 Particle 생성하기 (3개)

	const Vector3& XWorldAxis = Data->GetMeleeAttackCollider()->GetRelativeAxis(AXIS::AXIS_X) * -1.f;
	const Vector3& ZWorldAxis = Data->GetMeleeAttackCollider()->GetRelativeAxis(AXIS::AXIS_Z) * -1.f;

	const Vector3& ColliderRelativePos = XWorldAxis * 6.0f * -1.f + ZWorldAxis;

	Data->GetMeleeAttackCollider()->SetExtent(2.f, 3.f, 5.f);
	Data->GetMeleeAttackCollider()->SetRelativePos(ColliderRelativePos);

	const Vector3& MeleeAttackColliderWorldPos = Data->GetMeleeAttackCollider()->GetWorldPos();

	// Slash Effect SlashParticle
	CGameObject* AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos);

	AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos + ZWorldAxis * 3.f);

	AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos - ZWorldAxis * 3.f);
}

void CBossBettyCloseAttackNode::OnBossBettySlashRightEffect()
{
	// 오른쪽으로 45도 회전하기 (이것은 다른 Callback 에서 수행중이다)
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	const Vector3& SlashRightSpawnPoint = Data->GetRelativeSlashRightPos();
	
	// 1.바로 왼쪽 해당 위치에 Collider 생성하기 
	// 2. 연속적으로 Particle 생성하기 (3개)
	const Vector3& XWorldAxis = Data->GetMeleeAttackCollider()->GetRelativeAxis(AXIS::AXIS_X) * -1.f;
	const Vector3& ZWorldAxis = Data->GetMeleeAttackCollider()->GetRelativeAxis(AXIS::AXIS_Z) * -1.f;

	const Vector3& ColliderRelativePos = XWorldAxis * 6.0f + ZWorldAxis;

	Data->GetMeleeAttackCollider()->SetExtent(2.f, 3.f, 5.f);
	Data->GetMeleeAttackCollider()->SetRelativePos(ColliderRelativePos);

	const Vector3& MeleeAttackColliderWorldPos = Data->GetMeleeAttackCollider()->GetWorldPos();

	// Slash Effect SlashParticle
	CGameObject* AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos);

	AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos + ZWorldAxis * 3.f);

	AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos - ZWorldAxis * 3.f);

	//AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	//AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos + ZWorldAxis * 2.f);
	//
	//AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettySlash", CSceneManager::GetInst()->GetScene());
	//AfterEffectParticle->StartParticle(MeleeAttackColliderWorldPos - ZWorldAxis * 2.f);
}

void CBossBettyCloseAttackNode::OnBossBettyCommonEndFunctionOfCloseAttack()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	
	// Close Attack Anim 전환
	Data->OnBossBettyEnableCloseAttackChangeAnim();

	// Player 를 향해 회전하는 것 방지
	Data->OnDisableLookPlayer();

	// Current Node 를 nullptr 로 하여, 다른 Node 도 검사할 수 있게 한다.
	m_Owner->SetCurrentNode(nullptr);

	// PosAttackDelay 상태에 둔다.
	Data->SetAttackCoolDelayTimeEnable(true);

	// HP State 를 한번 더 체크할 수 있도록 한다.
	Data->OnBossBettyForceCheckHPState();
}

void CBossBettyCloseAttackNode::OnBossBettyCommonStartFunctionOfCloseAttack()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	Data->OnBossBettyNormalShakeCamera();
}
