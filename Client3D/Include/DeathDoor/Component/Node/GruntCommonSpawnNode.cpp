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
	
	// 42 : ������ Node
	AnimInst->AddNotify(AnimName, "SetCurrentNodeNull", 40,
		(CMonsterDataComponent*)Data, &CMonsterDataComponent::SetCurrentNodeNull);

	// ������ �ѹ� Enable �ǰ� �ȵ� ���̴�.
	AnimInst->AddNotify(AnimName, "SpawnEnableFalse", 40,
		Data, &CGruntCommonDataComponent::DisableSpawnEnable);

}

void CGruntCommonSpawnNode::SetWorldPosYUp()
{
	CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	// Player ���� �Ÿ��� Ȯ���Ѵ�
	CGameObject* PlayerObject = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

	// ó������ World Pos Y�� �ڽ��� MeshSize ��ŭ �Ʒ��� ������ �ִٰ�,
	// ���Ŀ� �ö�� ���̴�.
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
	// ������ Frame �� �����ߴٸ� Null�� �ٲ��ش�. (Ȥ�� �� ���׿� ����ؼ�)
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
	// ������ Frame �� �����ߴٸ� Null�� �ٲ��ش�. (Ȥ�� �� ���׿� ����ؼ�)
	m_Owner->SetCurrentNode(nullptr);

	return NodeResult::Node_True;
}
