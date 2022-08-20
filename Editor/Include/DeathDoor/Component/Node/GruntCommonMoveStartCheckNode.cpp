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

	// Player 와의 거리를 확인한다
	CGameObject* PlayerObject = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

	if (PlayerObject && IsStartEnable == false)
	{
		Vector3 PlayerPos = PlayerObject->GetWorldPos();
		Vector3 MyPos = m_Object->GetWorldPos();

		if (PlayerPos.Distance(MyPos) <= Data->GetMeleeAttackRange())
		{
			// 특정 거리 이내라면 StartActEnable 처리를 해준다.
			Data->SetStartActEnable(true);
		}
	}

	// 현재 시작하지 않았다면 Node_True 를 리턴하여, 다른 Tree 로 들어가지 않게 세팅할 것이다.
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
