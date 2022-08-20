#include "GruntCommonMoveStartCheckNode.h"
#include "../GruntCommonDataComponent.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

CGruntCommonMoveStartCheckNode::CGruntCommonMoveStartCheckNode()
{
	SetTypeID(typeid(CGruntCommonMoveStartCheckNode).hash_code());

}

CGruntCommonMoveStartCheckNode::CGruntCommonMoveStartCheckNode(const CGruntCommonMoveStartCheckNode& Node)
{
}

CGruntCommonMoveStartCheckNode::~CGruntCommonMoveStartCheckNode()
{
}

NodeResult CGruntCommonMoveStartCheckNode::OnStart(float DeltaTime)
{
	CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsStartEnable = Data->IsStartActEnable();

	// Player ���� �Ÿ��� Ȯ���Ѵ�
	CGameObject* PlayerObject = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

	if (PlayerObject && IsStartEnable == false)
	{
		Vector3 PlayerPos = PlayerObject->GetWorldPos();
		Vector3 MyPos = m_Object->GetWorldPos();

		if (PlayerPos.Distance(MyPos) <= Data->GetMeleeAttackRange())
		{
			// Ư�� �Ÿ� �̳���� StartActEnable ó���� ���ش�.
			Data->SetStartActEnable(true);
		}
	}

	// ���� �������� �ʾҴٸ� Node_True �� �����Ͽ�, �ٸ� Tree �� ���� �ʰ� ������ ���̴�.
	if (IsStartEnable == false)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CGruntCommonMoveStartCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CGruntCommonMoveStartCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
