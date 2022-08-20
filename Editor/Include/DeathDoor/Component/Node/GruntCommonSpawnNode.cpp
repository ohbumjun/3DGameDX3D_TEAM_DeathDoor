#include "GruntCommonSpawnNode.h"
#include "../GruntCommonDataComponent.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Component/AnimationMeshComponent.h"

CGruntCommonSpawnNode::CGruntCommonSpawnNode() :
	m_SpawnComplete(false)
{
	SetTypeID(typeid(CGruntCommonSpawnNode).hash_code());
}

CGruntCommonSpawnNode::CGruntCommonSpawnNode(const CGruntCommonSpawnNode& Node)
{
}

CGruntCommonSpawnNode::~CGruntCommonSpawnNode()
{
}

void CGruntCommonSpawnNode::Init()
{
	CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Animation CallBack
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	// 1) Slash Left
	std::string AnimName = "ZombieSpawn";
	CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();

	AnimInst->AddNotify(AnimName, "SetWorldPosUp", 1,
		this, &CGruntCommonSpawnNode::SetWorldPosYUp);

	AnimInst->AddNotify(AnimName, "SpawnSound", 2,
		Data, &CGruntCommonDataComponent::OnGruntSpawnSound);
	
	// 42 : 마지막 Node
	AnimInst->AddNotify(AnimName, "SetCurrentNodeNull", 40,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::SetCurrentNodeNull);

	// 어차피 한번 Enable 되고 안될 것이다.
	AnimInst->AddNotify(AnimName, "SpawnEnableFalse", 40,
		Data, &CGruntCommonDataComponent::DisableSpawnEnable);

}

void CGruntCommonSpawnNode::SetWorldPosYUp()
{
	CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Player 와의 거리를 확인한다
	CGameObject* PlayerObject = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

	// 처음에는 World Pos Y를 자신의 MeshSize 만큼 아래로 내려가 있다가,
	// 이후에 올라올 것이다.
	if (m_Object->GetWorldPos().y < -2.f)
	{
		const Vector3& WorldPos = m_Object->GetWorldPos();

		CGameObject* PlayerObject = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

		m_Object->SetWorldPos(WorldPos.x, PlayerObject->GetWorldPos().y, WorldPos.z);
	}
}

NodeResult CGruntCommonSpawnNode::OnStart(float DeltaTime)
{
	CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("ZombieSpawn");

	if (!m_SpawnComplete)
	{
		Data->OnActivateSpawnParticle();
		m_SpawnComplete = true;
	}

	m_IsEnd = false;

	return NodeResult::Node_True;
}

NodeResult CGruntCommonSpawnNode::OnUpdate(float DeltaTime)
{
	// 마지막 Frame 에 도달했다면 Null로 바꿔준다. (혹시 모를 사항에 대비해서)
	int EndFrameIndex = m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationSequence()->GetEndFrame();

	m_IsEnd = true;

	if (m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetAnimationSequence()->GetCurrentFrameIdx() >= EndFrameIndex)
	{
		m_IsEnd = true;
	}

	return NodeResult::Node_True;
}

NodeResult CGruntCommonSpawnNode::OnEnd(float DeltaTime)
{
	// 마지막 Frame 에 도달했다면 Null로 바꿔준다. (혹시 모를 사항에 대비해서)
	m_Owner->SetCurrentNode(nullptr);

	return NodeResult::Node_True;
}
