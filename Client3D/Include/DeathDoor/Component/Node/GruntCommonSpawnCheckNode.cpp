#include "GruntCommonSpawnCheckNode.h"
#include "../GruntCommonDataComponent.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

CGruntCommonSpawnCheckNode::CGruntCommonSpawnCheckNode()
{
	SetTypeID(typeid(CGruntCommonSpawnCheckNode).hash_code());
}

CGruntCommonSpawnCheckNode::CGruntCommonSpawnCheckNode(const CGruntCommonSpawnCheckNode& Node)
{
}

CGruntCommonSpawnCheckNode::~CGruntCommonSpawnCheckNode()
{
}

NodeResult CGruntCommonSpawnCheckNode::OnStart(float DeltaTime)
{
	CGruntCommonDataComponent* Data = dynamic_cast<CGruntCommonDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	bool IsSpawnEnable = Data->IsSpawnEnable();

	// 현재 시작하지 않았다면 Node_True 를 리턴하여, 다른 Tree 로 들어가지 않게 세팅할 것이다.
	if (IsSpawnEnable)
	{
		return NodeResult::Node_True;
	}
	else
	{
		return NodeResult::Node_False;
	}
}

NodeResult CGruntCommonSpawnCheckNode::OnUpdate(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CGruntCommonSpawnCheckNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}
