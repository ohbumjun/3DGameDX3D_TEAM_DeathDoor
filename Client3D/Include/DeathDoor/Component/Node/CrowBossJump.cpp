
#include "CrowBossJump.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../CrowBossDataComponent.h"
#include "../MonsterNavAgent.h"
#include "../ProjectileComponent.h"
#include "Scene/Scene.h"
#include "Component/ColliderBox3D.h"

CCrowBossJump::CCrowBossJump()
{
	SetTypeID(typeid(CCrowBossJump).hash_code());
}

CCrowBossJump::CCrowBossJump(const CCrowBossJump& Node) :
	CActionNode(Node)
{
}

CCrowBossJump::~CCrowBossJump()
{
}

NodeResult CCrowBossJump::OnStart(float DeltaTime)
{
	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Jump");

	m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Jump");

	return NodeResult::Node_True;
}

NodeResult CCrowBossJump::OnUpdate(float DeltaTime)
{
	CProjectileComponent* Projectile = m_Object->FindObjectComponentFromType<CProjectileComponent>();

	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	Data->SetStartJump(true);
	Data->GetHitBox()->SetRigidCollisionIgnore(false);

	Vector3 CurrentPos = m_Object->GetWorldPos();
	Vector3 PlayerPos = m_Object->GetScene()->GetPlayerObject()->GetWorldPos();
	Vector3 MyPos = m_Object->GetWorldPos();

	Vector3 Dir = PlayerPos - CurrentPos;
	Dir.Normalize();
	Dir.y = 0.7f;
	Dir.Normalize();


	Projectile->SetDestroy(false);
	Projectile->SetNoUpdate(false);

	float ShootAngle = 50.f;
	float Dist = CurrentPos.Distance(PlayerPos);

	if (Dist > 30.f)
	{
		ShootAngle = 40.f;
	}
	else if (Dist > 20.f && Dist <= 30.f)
	{
		ShootAngle = 50.f;
	}

	else if (Dist > 10.f && Dist <= 20.f)
	{
		ShootAngle = 60.f;
	}

	else if (Dist <= 10.f)
	{
		ShootAngle = 70.f;
	}

	Projectile->ShootByGravityTargetPos(CurrentPos, Dir, ShootAngle, PlayerPos, 16.f);

	return NodeResult::Node_True;
}

NodeResult CCrowBossJump::OnEnd(float DeltaTime)
{
	return NodeResult();
}
