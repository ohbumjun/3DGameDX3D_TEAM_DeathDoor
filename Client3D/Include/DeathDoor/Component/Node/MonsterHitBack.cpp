#include "MonsterHitBack.h"
#include "../MonsterDataComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Component/BehaviorTree.h"	
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"

CMonsterHitBack::CMonsterHitBack()
{
	SetTypeID(typeid(CMonsterHitBack).hash_code());
}

CMonsterHitBack::CMonsterHitBack(const CMonsterHitBack& Node)
{
}

CMonsterHitBack::~CMonsterHitBack()
{
}

void CMonsterHitBack::Init()
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	// 1) Slash Left
	std::string AnimName = "HitImplace";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->AddNotify(AnimName, "KeepPrevZMove", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnKeepPrevMoveZ);
	AnimInst->AddNotify(AnimName, "DisableLookPlayer", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableLookPlayer);
	AnimInst->AddNotify(AnimName, "DisableMoveZ", 0,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableMoveZ);

	AnimInst->AddNotify(AnimName, "SetCurrentNodeNull", 14,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::SetCurrentNodeNull);

	AnimInst->AddNotify(AnimName, "EnableMoveZ", 16,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnDisableMoveZ);
	AnimInst->AddNotify(AnimName, "OnResetPrevMoveZ", 16,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::OnResetPrevMoveZ);

}

NodeResult CMonsterHitBack::OnStart(float DeltaTime)
{
	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("HitImpact");
	
	m_Owner->SetCurrentNode(this);

	m_IsEnd = false;

	return NodeResult::Node_True;
}

NodeResult CMonsterHitBack::OnUpdate(float DeltaTime)
{
	// 마지막 Frame 에 도달했다면 Null로 바꿔준다. (혹시 모를 사항에 대비해서)
	int EndFrameIndex = m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationSequence()->GetEndFrame();

	if (m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationSequence()->GetCurrentFrameIdx() >= EndFrameIndex)
	{
		m_IsEnd = true;
	}

	return NodeResult::Node_True;
}

NodeResult CMonsterHitBack::OnEnd(float DeltaTime)
{
	// 마지막 Frame 에 도달했다면 Null로 바꿔준다. (혹시 모를 사항에 대비해서)
	m_Owner->SetCurrentNode(nullptr);

	return NodeResult::Node_True;
}
