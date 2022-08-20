
#include "FindPathNode.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Navigation3DManager.h"
#include "Component/BehaviorTree.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "../MonsterNavAgent.h"
#include "Component/AnimationMeshComponent.h"

CFindPathNode::CFindPathNode()	:
	m_NavAgent(nullptr)
{
	SetTypeID(typeid(CFindPathNode).hash_code());
}

CFindPathNode::CFindPathNode(const CFindPathNode& Node)	:
	CActionNode(Node)
{
}

CFindPathNode::~CFindPathNode()
{
}

NodeResult CFindPathNode::OnStart(float DeltaTime)
{
	CScene* Scene = CSceneManager::GetInst()->GetScene();

	CGameObject* Player = Scene->GetPlayerObject();

	CMonsterDataComponent* DataComp = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	m_NavAgent = (dynamic_cast<CMonsterDataComponent*>(DataComp))->GetMonsterNavAgent();

	if (m_NavAgent)
	{
		CSceneComponent* Root = m_Object->GetRootComponent();

		if (!Player)
		{
			return NodeResult::Node_True;
		}

		Vector3 PlayerPos = Player->GetWorldPos();
		std::vector<Vector3> vecPath;
		bool Straight = m_NavAgent->CheckStraightPath(Root->GetWorldPos(), PlayerPos, vecPath);

		if (Straight)
		{
			m_NavAgent->AddPath(PlayerPos);
		}

		else
			m_NavAgent->FindPath(Root, PlayerPos);

		DataComp->SetPahtFindExecute(true);
	}

	
	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("Run");

	return NodeResult::Node_True;
}

NodeResult CFindPathNode::OnUpdate(float DeltaTime)
{
	// PathFindEnable�� False���ؼ� CheckDetectRangeNode���� CFindPathNode�� �� �Ѿ���� �ϰ�, 
	// PathFindCoolTime�� �� ������ �ٽ� PathFindEnable = true�� �ؼ� ��ã�� �� �� �ֵ��� �Ѵ�
	m_NavAgent->SetPathFindEnable(false);


	return NodeResult::Node_True;
}

NodeResult CFindPathNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CFindPathNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}

