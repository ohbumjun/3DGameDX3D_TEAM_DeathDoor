#include "BossBettyAngryAttackNode.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../ProjectileComponent.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Scene/Scene.h"
#include "../BossBettyDataComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "EngineUtil.h"
#include "ObjectPool.h"

CBossBettyAngryAttackNode::CBossBettyAngryAttackNode()
{
	SetTypeID(typeid(CBossBettyAngryAttackNode).hash_code());
}

CBossBettyAngryAttackNode::CBossBettyAngryAttackNode(const CBossBettyAngryAttackNode& Node)
{
}

CBossBettyAngryAttackNode::~CBossBettyAngryAttackNode()
{
}

void CBossBettyAngryAttackNode::Init()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	// 1) HPDamageAngry
	std::string AnimName = "FirstSlamAnger";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	// 던지기 전까지 Player 방향으로 회전할 수 있도록 한다.
	AnimInst->AddNotify(AnimName, "OnTracePlayer", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnEnableLookPlayer);
	AnimInst->AddNotify(AnimName, "OnDisableTracePlayer", 5,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "RoarSound", 0,
		Data, &CBossBettyDataComponent::OnBossBettyRoarSound);

	// Snow Ball Falling
	AnimInst->AddNotify(AnimName, "CameraShake", 9,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	AnimInst->AddNotify(AnimName, "OnStartFallingSnowBallEffect", 9, 
		this, &CBossBettyAngryAttackNode::OnBossBettyStartFallingSnowBallEffect);

	// 양쪽
	AnimInst->AddNotify(AnimName, "OnTwoSideCloseAttackEffect", 25, 
		Data, &CBossBettyDataComponent::OnBossBettyGenerateTwoSideCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackSound", 25,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);

	// 왼쪽
	AnimInst->AddNotify(AnimName, "OnLeftAttackDownEffect", 34, 
		Data, &CBossBettyDataComponent::OnBossBettyGenerateLeftCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackSound", 34,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "CameraShake", 34,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);
	// 오른쪽
	AnimInst->AddNotify(AnimName, "OnRightAttackDownEffect", 43, 
		Data, &CBossBettyDataComponent::OnBossBettyGenerateRightCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackSound", 43,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "OnRightAttackDownEffect", 43, 
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// 왼쪽
	AnimInst->AddNotify(AnimName, "OnLeftAttackDownEffect", 52, 
		Data, &CBossBettyDataComponent::OnBossBettyGenerateLeftCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackSound", 52,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "OnLeftAttackDownEffect", 52, 
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// 오른쪽
	AnimInst->AddNotify(AnimName, "OnRightAttackDownEffect", 62,
		Data, &CBossBettyDataComponent::OnBossBettyGenerateRightCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackSound", 63,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "OnRightAttackDownEffect", 62,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// 양쪽
	AnimInst->AddNotify(AnimName, "OnTwoSideCloseAttackEffect", 78,
		Data, &CBossBettyDataComponent::OnBossBettyGenerateTwoSideCloseAttackEffect);
	AnimInst->AddNotify(AnimName, "AttackSound", 73,
		Data, &CBossBettyDataComponent::OnBossBettyAttackDownSound);
	AnimInst->AddNotify(AnimName, "OnTwoSideCloseAttackEffect", 78,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// 울부짖기
	AnimInst->AddNotifyDeltaTimeFrameRange(AnimName, "OnBettyRoar", 100, 120,
		Data, &CBossBettyDataComponent::OnBossBettyRoarEffect);
	AnimInst->AddNotify(AnimName, "OnBettyRoar", 100,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	AnimInst->AddNotify(AnimName, "RoarSound", 100,
		Data, &CBossBettyDataComponent::OnBossBettyRoarSound);

	AnimInst->AddNotify(AnimName, "ActivateRoarParticle", 100,
		Data, &CBossBettyDataComponent::OnBossBettyActivateRoarParticle);
	
	AnimInst->AddNotify(AnimName, "CameraShake", 100,
		Data, &CBossBettyDataComponent::OnBossBettyNormalShakeCamera);

	// End
	AnimInst->AddNotify(AnimName, "ResetHPState", 121,
	 	Data, &CBossBettyDataComponent::ResetBettyHPState);

	AnimInst->AddNotify(AnimName, "ResetCurrentNode", 121,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::SetCurrentNodeNull);
}

NodeResult CBossBettyAngryAttackNode::OnStart(float DeltaTime)
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	
	m_Owner->SetCurrentNode(this);

	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->ChangeAnimation("FirstSlamAnger");

	return NodeResult::Node_True;
}

NodeResult CBossBettyAngryAttackNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CBossBettyAngryAttackNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

void CBossBettyAngryAttackNode::OnBossBettyStartFallingSnowBallEffect()
{
	CBossBettyDataComponent* Data = dynamic_cast<CBossBettyDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CScene* CurrentScene = CSceneManager::GetInst()->GetScene();

	// 사방에서 투사체 눈덩이가 떨어지게 한다.
	// 1. 특정 위치에 투사체 눈덩이 Object 를 생성
	CGameObject* MapSurroundingObject = CurrentScene->FindObject("MapSurrounding");

	const Vector3& MapSurroundingObjectWorldPos = MapSurroundingObject->GetWorldPos();

	CColliderSphere* ColliderSphere = MapSurroundingObject->FindComponentFromType<CColliderSphere>();

	int SphereRadius = (int)ColliderSphere->GetInfo().Radius;

	const Vector3& PlayerPos = CSceneManager::GetInst()->GetScene()->GetPlayerObject()->GetWorldPos();

	for (int i = 0; i < 6; ++i)
	{
		int XRand = rand() % SphereRadius;
		int YRand = SphereRadius + rand() % (SphereRadius * 3);
		int ZRand = rand() % SphereRadius;

		float RandV = ((float)rand() / (RAND_MAX)) + 1;
		float RandomAngle = 360.f * RandV;

		CGameObject* SnowFallingObject = CObjectPool::GetInst()->GetProjectile("BossBettySnowAttackParticle", CurrentScene);

		SnowFallingObject->SetWorldPos(MapSurroundingObjectWorldPos + Vector3(
			XRand * cos(RandomAngle), (float)YRand, ZRand * sin(RandomAngle)));
			
		CProjectileComponent* ProjTileComp = SnowFallingObject->FindComponentFromType<CProjectileComponent>();

		CGameObject* AfterEffectParticle = CObjectPool::GetInst()->GetParticle("BettyAttackAfterEffect", CSceneManager::GetInst()->GetScene());

		CColliderBox3D* Collider3D = AfterEffectParticle->FindComponentFromType<CColliderBox3D>();

		Collider3D->AddCollisionCallback(Collision_State::Begin, (CMonsterDataComponent*)Data, &CMonsterDataComponent::OnHitMeleeAttack);

		Collider3D->SetExtent(1.5f, 1.f, 1.5f);

		const Vector3& SnowObjectWorldPos = SnowFallingObject->GetWorldPos();

		ProjTileComp->ShootByTargetPos(SnowObjectWorldPos, 20.f + rand() % 10, 
			Vector3(SnowObjectWorldPos.x, PlayerPos.y + 2.f, SnowObjectWorldPos.z), AfterEffectParticle);

		// CallBack Sound
		ProjTileComp->SetEndCallBack(Data, &CBossBettyDataComponent::OnBossBettyThrowBallLandSound);

		//Sound
		Data->OnBossBettyThrowBallFallStartSound();
	}

	// 2. 각각에 대해서, 충돌시 동작시킬 콜백들을 세팅한다.
	// - 투사체의 경우, ThrowNode, AngryAttackNode 둘 다에서 동작하기 때문에 
	// - BossBettyDataComponent::OnExplodeBettyThrowBallCallback 로 세팅
}

