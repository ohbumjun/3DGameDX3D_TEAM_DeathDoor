#include "BossKnightSlamEnd.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Scene/Scene.h"
#include "../KnightDataComponent.h"

CBossKnightSlamEnd::CBossKnightSlamEnd()
{
	SetTypeID(typeid(CBossKnightSlamEnd).hash_code());
}

CBossKnightSlamEnd::CBossKnightSlamEnd(const CBossKnightSlamEnd& Node)	:
	CActionNode(Node)
{
}

CBossKnightSlamEnd::~CBossKnightSlamEnd()
{
}

void CBossKnightSlamEnd::Init()
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	std::string AnimName = "SlamEnd";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
	AnimInst->SetEndFunction(AnimName, Data, &CMonsterDataComponent::OnEndAnimPostAttackDelayOff);
}

NodeResult CBossKnightSlamEnd::OnStart(float DeltaTime)
{
	if (this != m_Owner->GetCurrentNode())
	{
		m_Owner->SetCurrentNode(this);

		// 후딜레이 애니메이션 재생
		std::string AnimName = "SlamEnd";
		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation(AnimName);
	}

	return NodeResult::Node_True;
}

NodeResult CBossKnightSlamEnd::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CBossKnightSlamEnd::OnEnd(float DeltaTime)
{
	return NodeResult();
}
