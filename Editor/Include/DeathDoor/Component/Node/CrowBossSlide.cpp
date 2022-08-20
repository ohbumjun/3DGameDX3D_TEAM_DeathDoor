
#include "CrowBossSlide.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "../CrowBossDataComponent.h"
#include "../MonsterNavAgent.h"
#include "Scene/Scene.h"

CCrowBossSlide::CCrowBossSlide()
{
	SetTypeID(typeid(CCrowBossSlide).hash_code());
}

CCrowBossSlide::CCrowBossSlide(const CCrowBossSlide& Node) :
	CActionNode(Node)
{
}

CCrowBossSlide::~CCrowBossSlide()
{
}

NodeResult CCrowBossSlide::OnStart(float DeltaTime)
{

	return NodeResult::Node_True;
}

NodeResult CCrowBossSlide::OnUpdate(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->GetHP() <= 0)
	{
		Data->ClearPhaseQueue();

		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Death");

		m_Owner->GetOwner()->SetTreeUpdate(false);

		m_Object->GetScene()->GetResource()->SoundPlay("OldCrow_Death");

		return NodeResult::Node_False;
	}

	CNavAgent* Agent = Data->GetMonsterNavAgent();

	Vector3 FaceDir = Agent->GetCurrentFaceDir();

	m_Object->AddWorldPos(FaceDir * 4.f * DeltaTime);

	return NodeResult::Node_False;
}

NodeResult CCrowBossSlide::OnEnd(float DeltaTime)
{
	return NodeResult();
}
