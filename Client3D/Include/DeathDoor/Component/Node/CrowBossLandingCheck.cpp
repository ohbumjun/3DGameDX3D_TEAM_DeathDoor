
#include "CrowBossLandingCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"
#include "../ProjectileComponent.h"
#include "Scene/Scene.h"
#include "Component/ColliderBox3D.h"

CCrowBossLandingCheck::CCrowBossLandingCheck()
{
	SetTypeID(typeid(CCrowBossLandingCheck).hash_code());
}

CCrowBossLandingCheck::CCrowBossLandingCheck(const CCrowBossLandingCheck& Node) :
	CConditionNode(Node)
{
}

CCrowBossLandingCheck::~CCrowBossLandingCheck()
{
}

NodeResult CCrowBossLandingCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool StartStomp = Data->GetStartStomp();

	Vector3 MyPos = m_Object->GetWorldPos();

	if (MyPos.y <= 0.f)
	{
		CProjectileComponent* Projectile = m_Object->FindObjectComponentFromType<CProjectileComponent>();
		Projectile->SetNoUpdate(true);

		Data->SetStartStomp(false);
		Data->SetStartJump(false);
		Data->GetMeleeAttackCollider()->Enable(true);

		m_Object->SetWorldPos(MyPos.x, 0.f, MyPos.z);

		m_Object->GetScene()->GetCameraManager()->ShakeCamera(0.4f, 0.5f);

		return NodeResult::Node_True;
	}

	return NodeResult::Node_False;
}

NodeResult CCrowBossLandingCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CCrowBossLandingCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
