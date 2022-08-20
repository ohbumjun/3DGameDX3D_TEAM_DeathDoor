#include "BossBettyJumpAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Collision/Collision.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../BossBettyDataComponent.h"
#include "EngineUtil.h"

CBossBettyJumpAttackNode::CBossBettyJumpAttackNode() :
	m_CalRealTimeChangingSpeed(false)
{
	SetTypeID(typeid(CBossBettyJumpAttackNode).hash_code());
}

CBossBettyJumpAttackNode::CBossBettyJumpAttackNode(const CBossBettyJumpAttackNode& Node)
{
}

CBossBettyJumpAttackNode::~CBossBettyJumpAttackNode()
{
}

void CBossBettyJumpAttackNode::Init()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// PunchDouble
	std::string AnimName = "PunchDouble";

	// >> Start
	AnimInst->AddNotify(AnimName, "EnableCloseAttackChangeAnim", 0,
		Data, &CBossBettyDataComponent::OnBossBettyEnableCloseAttackChangeAnim);
	AnimInst->AddNotify(AnimName, "OnTracePlayer", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnZMove", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableMoveZ);
	AnimInst->AddNotify(AnimName, "SlowMoveSpeed", 0,
	Data, &CBossBettyDataComponent::OnBossBettyApplyOutOfMapSurroundingColliderMoveSpeed);


	// >> 중간
	AnimInst->AddNotify(AnimName, "StartJump", 6,
		this, &CBossBettyJumpAttackNode::OnBossBettyStartJump);
	AnimInst->AddNotify(AnimName, "JumpStartSound", 6,
		Data, &CBossBettyDataComponent::OnBossBettyJumpStartSound);

	// JumpAttackNode 의 경우, Spin Collider 를 별도로 세팅할 필요가 없다.
	//AnimInst->AddNotify(AnimName, "OnEnableSpinCollider", 10,
	//	Data, &CBossBettyDataComponent::OnBossBettyEnableSpinCollider);

	AnimInst->AddNotify(AnimName, "DisableLookPlayer", 10,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);

	AnimInst->AddNotify(AnimName, "DisableZMove", 25,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableMoveZ);
	AnimInst->AddNotify(AnimName, "CameraShake", 25,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// >> End
	AnimInst->AddNotify(AnimName, "JumpEndSound", 24,
		Data, &CBossBettyDataComponent::OnBossBettyLandAfterJumpSound);
	// PunchDown Animation 이 끝나면 해당 위치에 2단 공격
	AnimInst->AddNotify(AnimName, "AttackDown", 24,
		Data, &CBossBettyDataComponent::OnBossBettyGenerateTwoSideCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "EnableAttackCollider", 25,
		Data, &CBossBettyDataComponent::OnBossBettyEnableAttackCollider);
	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 29,
		Data, &CBossBettyDataComponent::OnBossBettyDisableAttackCollider);
	//AnimInst->AddNotify(AnimName, "OnDisableSpinCollider", 29,
	//	Data, &CBossBettyDataComponent::OnBossBettyDisableSpinCollider);

	AnimInst->AddNotify(AnimName, "AttackFarAttackCnt", 35,
		Data, &CBossBettyDataComponent::IncFarAttackCount);
	AnimInst->AddNotify(AnimName, "BackToOriginalSpeed", 35,
		Data, &CBossBettyDataComponent::OnBossBettyResetOriginalMoveSpeed);
	AnimInst->AddNotify(AnimName, "EndJump", 35,
		this, &CBossBettyJumpAttackNode::OnBossBettyEndJump);
	AnimInst->AddNotify(AnimName, "SetCurrentNodeNullptr", 35,
		Data, &CBossBettyDataComponent::OnBossBettySetCurrentNodeNullPtr);
	// AnimInst->AddNotify(AnimName, "ForceHPStateCheck", 35,
	// 	Data, &CBossBettyDataComponent::OnBossBettyForceCheckHPState);
}

NodeResult CBossBettyJumpAttackNode::OnStart(float DeltaTime)
{
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->ChangeAnimation("PunchDouble");

	m_Owner->SetCurrentNode(this);

	return NodeResult::Node_True;
}

NodeResult CBossBettyJumpAttackNode::OnUpdate(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// 지수함수를 이용해서, Speed 를 서서히 줄인다.
	if (m_CalRealTimeChangingSpeed)
	{
		m_JumpAccTime += DeltaTime;
		float RealTimeSpeed = CEngineUtil::CalculateRealTimeSpeedUsingExponentialWithSpeed(m_JumpTotalTime, m_InitJumpStartSpeed, 1.f, m_JumpAccTime);
		Data->SetCurMoveSpeed(RealTimeSpeed);
	}

	return NodeResult::Node_True;
}

NodeResult CBossBettyJumpAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

void CBossBettyJumpAttackNode::OnBossBettyStartJump()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	m_JumpAccTime = 0.f;

	m_CalRealTimeChangingSpeed = true;

	float FrameTime = m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationSequence()->GetFrameTime();
	
	// m_JumpTotalTime = m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationPlayTime() * 0.7f;
	m_JumpTotalTime = FrameTime * 25.f;

	// m_JumpTotalTime 동안 이동한다고 가정
	float DistToPlayer = Data->DistToPlayer();

	m_InitJumpStartSpeed = (DistToPlayer - 1.f) / m_JumpTotalTime;
}

void CBossBettyJumpAttackNode::OnBossBettyEndJump()
{
	m_JumpAccTime = 0.f;
	m_CalRealTimeChangingSpeed = false;
}

void CBossBettyJumpAttackNode::OnBossBettySlowMoveSpeed()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	
	Data->SetCurMoveSpeed(Data->GetOriginMoveSpeed() * 0.7f);
}

void CBossBettyJumpAttackNode::OnBossBettyCalJumpAttackTimeAndDist()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	float DistToPlayer = Data->DistToPlayer();
	float CurMoveSpeed = Data->GetCurMoveSpeed();
	float MoveTime = DistToPlayer / CurMoveSpeed;

	Data->SetMoveZEnableMaxTime(MoveTime);
}
