#include "BossBettySpinAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderBox3D.h"
#include "Collision/Collision.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../BossBettyDataComponent.h"

CBossBettySpinAttackNode::CBossBettySpinAttackNode() :
	m_JumpSpinChangeEnable(true)
{
	SetTypeID(typeid(CBossBettySpinAttackNode).hash_code());
}

CBossBettySpinAttackNode::CBossBettySpinAttackNode(const CBossBettySpinAttackNode& Node)
{
}

CBossBettySpinAttackNode::~CBossBettySpinAttackNode()
{
}

void CBossBettySpinAttackNode::Init()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// 1) JumpSpin
	std::string AnimName = "JumpSpin";

	// 매우 느리게 앞으로 갈 수 있도록 한다.
	// Move Speed 의 경우 BossDataComponent 에서 처리해주고 있다.
	// 이제 Close Attack 은 가능하게 해야 한다. Far Attack 이 진행중이므로
	AnimInst->AddNotify(AnimName, "JumpStartSound", 0,
		Data, &CBossBettyDataComponent::OnBossBettyJumpStartSound);
	AnimInst->AddNotify(AnimName, "EnableCloseAttackChangeAnim", 0,
		Data, &CBossBettyDataComponent::OnBossBettyEnableCloseAttackChangeAnim);
	AnimInst->AddNotify(AnimName, "EnableZMove", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableMoveZ);
	AnimInst->AddNotify(AnimName, "SlowMoveSpeed", 0,
		Data, &CBossBettyDataComponent::OnBossBettyApplyOutOfMapSurroundingColliderMoveSpeed);

	// End
	AnimInst->AddNotify(AnimName, "CameraShake", 17,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// JumpSpin Animation 이 끝나면 바로 Spin Animation 으로 바꾼다.
	AnimInst->SetEndFunction(AnimName,
		this, &CBossBettySpinAttackNode::OnBossBettyChangeToSpinAnimation);

	// 2) Spin
	AnimName = "Spin";

	// Spin 을 하는 순간, 처음에 ColliderBox 에 특정 Callback 을 건다.
	// - 벽 등과 충돌 시에 SpinCollide 로 바꿔야 한다.
	// - 해당 ColliderBox 및, 콜백함수는 이미 BossBettyDataComponent 에서 세팅된 상태이다.
	// - 이를 다시 Enable 만 시켜주면 된다.
	AnimInst->AddNotify(AnimName, "AttackColliderToOriginalPs", 0,
		Data, &CBossBettyDataComponent::OnSetBossBettyAttackColliderPosToBettyBody);
	AnimInst->AddNotify(AnimName, "DisalbeLookPlayer", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "ResetMoveSpeed", 0,
		Data, &CBossBettyDataComponent::OnBossBettyResetOriginalMoveSpeed);
	AnimInst->AddNotify(AnimName, "EnableSpinCollider", 1,
		Data, &CBossBettyDataComponent::OnBossBettyEnableSpinCollider);	
	AnimInst->AddNotify(AnimName, "EnableAttackCollider", 4,
		Data, &CBossBettyDataComponent::OnBossBettyEnableAttackCollider);


	// 2) Spin Collide
	AnimName = "SpinCollide";

	// 처음 시작하는 순간, 다시 Spin Collide 를 비활성화 시킨다.
	AnimInst->AddNotify(AnimName, "CollideWithWallSound", 0,
		Data, &CBossBettyDataComponent::OnBossBettyCollideWithWallSound);

	AnimInst->AddNotify(AnimName, "DisableSpinCollider", 0, 
		Data, &CBossBettyDataComponent::OnBossBettyDisableSpinCollider);
	// 앞으로 가는 것을 무효화
	AnimInst->AddNotify(AnimName, "DisableZMove", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableMoveZ);
	// 원거리 공격 횟수 1 증가
	AnimInst->AddNotify(AnimName, "AddFarAttakCount", 0,
		Data, &CBossBettyDataComponent::IncFarAttackCount);
	// 공중에서 Player 를 향해 서서히 돈다.
	// 던지기 전까지 Player 방향으로 회전할 수 있도록 한다.
	AnimInst->AddNotify(AnimName, "OnTracePlayer", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);

	// 중간
	AnimInst->AddNotify(AnimName, "CameraShake", 20,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// >> EndFunctions
	AnimInst->AddNotify(AnimName, "DisablePlayerLook", 24,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	// Sound
	AnimInst->AddNotify(AnimName, "JumpLandSound", 25,
		Data, &CBossBettyDataComponent::OnBossBettyLandAfterJumpSound);
	// 마지막 순간에 착지한 바닥을 공격한다.
	AnimInst->AddNotify(AnimName, "AttackDown", 25,
		Data, &CBossBettyDataComponent::OnBossBettyGenerateTwoSideCloseAttackEffect);
	// 모든 Spin이 끝나고 나서야 비로소, CurrentNode 를 nullptr 로 세팅한다.
	AnimInst->AddNotify(AnimName, "SetCurrentNodeNull", 25,
		Data, &CBossBettyDataComponent::OnBossBettySetCurrentNodeNullPtr);

	// Jump Spin 으로의 전환을 허용한다.
	AnimInst->AddNotify(AnimName, "EnableJumpSpinChange", 25,
		this, &CBossBettySpinAttackNode::OnBossBettyEnableSpinChange);

	AnimInst->AddNotify(AnimName, "DisableAttackCollider", 25,
		Data, &CBossBettyDataComponent::OnBossBettyDisableAttackCollider);

	// AnimInst->AddNotify(AnimName, "ForceHPStateCheck", 25,
	// 	Data, &CBossBettyDataComponent::OnBossBettyForceCheckHPState);
}

NodeResult CBossBettySpinAttackNode::OnStart(float DeltaTime)
{
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// 1. Spin 중이라면, Animation 을 바꿔서는 안된다.
	// 2. 혹은, m_JumpSpinChangeEnable 가 false 이지만, 현재 현재 Spin, SpinCollide Animation 이 아니라면
	if (m_JumpSpinChangeEnable || 
		(!m_JumpSpinChangeEnable && 
			AnimInst->GetCurrentAnimation()->GetName() != "Spin" &&
			AnimInst->GetCurrentAnimation()->GetName() != "SpinCollide"))
	{
		AnimInst->ChangeAnimation("JumpSpin");
		m_JumpSpinChangeEnable = false;
	}

	m_Owner->SetCurrentNode(this);
	// CAnimationSequenceData* CurAnim = AnimInst->GetCurrentAnimation();
	// 
	// if (CurAnim->GetName() != "Spin" && CurAnim->GetName() != "SpinCollide")
	// {
	// 	m_JumpSpinChangeEnable = true;
	// }
	// 
	// if (m_JumpSpinChangeEnable)
	// {
	// 	AnimInst->ChangeAnimation("JumpSpin");
	// 	m_JumpSpinChangeEnable = false;
	// }

	return NodeResult::Node_True;
}

NodeResult CBossBettySpinAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossBettySpinAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

void CBossBettySpinAttackNode::OnBossBettyChangeToSpinAnimation()
{
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->ChangeAnimation("Spin");

	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	Data->OnBossBettyRollStartSound();
}

void CBossBettySpinAttackNode::OnBossBettyEnableSpinChange()
{
	m_JumpSpinChangeEnable = true;
}

void CBossBettySpinAttackNode::OnBossBettyDisableSpinChange()
{
	m_JumpSpinChangeEnable = false;
}
