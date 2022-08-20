
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
	// PathFindEnable을 False로해서 CheckDetectRangeNode에서 CFindPathNode로 못 넘어오게 하고, 
	// PathFindCoolTime이 다 지나면 다시 PathFindEnable = true로 해서 길찾기 할 수 있도록 한다
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

