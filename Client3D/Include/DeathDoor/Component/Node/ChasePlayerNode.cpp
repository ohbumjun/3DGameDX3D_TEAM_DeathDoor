#include "ChasePlayerNode.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Navigation3DManager.h"
#include "Component/BehaviorTree.h"	
#include "Component/AnimationMeshComponent.h"	
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "../MonsterNavAgent.h"

CChasePlayerNode::CChasePlayerNode()
{
	SetTypeID(typeid(CChasePlayerNode).hash_code());
}

CChasePlayerNode::CChasePlayerNode(const CChasePlayerNode& Node)	:
	CActionNode(Node)
{
}

CChasePlayerNode::~CChasePlayerNode()
{
}

void CChasePlayerNode::Init()
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	m_DataComp = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
	m_Nav = m_DataComp->GetMonsterNavAgent();

	m_MoveSpeed = m_DataComp->GetMoveSpeed();
	m_RotationSpeed = m_DataComp->GetRotateSpeed();
	m_RootComp = m_Owner->GetOwner()->GetGameObject()->GetRootComponent();

	CScene* Scene = m_Owner->GetOwner()->GetScene();
	m_PlayerObj = Scene->GetPlayerObject();
}

NodeResult CChasePlayerNode::OnStart(float DeltaTime)
{
	if (m_Owner->GetCurrentNode() != this)
	{
		m_Owner->SetCurrentNode(this);

		m_Nav->SetMoveSpeed(m_MoveSpeed);
		m_Nav->SetRotationSpeed(m_RotationSpeed);

		m_DataComp->OnCombatStart();

		CAnimationSequenceInstance* AnimInst = m_AnimationMeshComp->GetAnimationInstance();
		AnimInst->ChangeAnimation("Run");

		FindPath();
	}

	return NodeResult::Node_True;
}

NodeResult CChasePlayerNode::OnUpdate(float DeltaTime)
{
	// �⺻������ MonsterDataComponent���� ���� ���� �Ÿ��� MeleeAttack���� �����Ǿ� ����
	// �ٸ� �Ÿ������� �����ؾ� �ϴ� ���, �� ���� �����Ϳ��� MonsterData�� m_StopChaseRange ������ �����Ͽ� ����ϸ� ��
	bool InStopChaseRange = m_DataComp->IsPlayerInStopChaseRange();

	if (InStopChaseRange)
	{
		m_DataComp->SetCurrentNodeNull();
		m_Nav->ClearPathList();
		m_DataComp->SetCurMoveSpeed(0.f);
		m_DataComp->SetCurRotSpeed(0.f);

		return NodeResult::Node_True;
	}

	bool NavCool = m_Nav->GetPathFindEnable();

	if (NavCool)
	{
		bool NavEmpty = m_Nav->IsEmptyPathList();

		if (!NavEmpty)
		{
			bool Rotaiting = m_Nav->IsRotaiting();

			// �ٶ󺸴� ����� ���� ������ ���� �ʾ� ���� �ִ� ���߿��� ������ �������� �ʴ´�
			if (Rotaiting)
			{
				m_Nav->SetMoveSpeed(0.f);
			}
			else
			{
				m_Nav->SetMoveSpeed(m_MoveSpeed);
			}
		}
		return NodeResult::Node_True;
	}
	else
	{
		// ��Ÿ�Ӹ��� �ٽ� ã�´�.
		FindPath();
	}

	return NodeResult::Node_True;
}

NodeResult CChasePlayerNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}

void CChasePlayerNode::FindPath()
{
	Vector3 PlayerPos = m_PlayerObj->GetWorldPos();

	m_Nav->ClearPathList();

	CSceneComponent* Root = m_Object->GetRootComponent();
	std::vector<Vector3> vecPath;
	bool Straight = m_Nav->CheckStraightPath(Root->GetWorldPos(), PlayerPos, vecPath);

	if (Straight)
	{
		m_Nav->AddPath(PlayerPos);
	}
	else
	{
		m_Nav->FindPath(Root, PlayerPos);
	}
}
