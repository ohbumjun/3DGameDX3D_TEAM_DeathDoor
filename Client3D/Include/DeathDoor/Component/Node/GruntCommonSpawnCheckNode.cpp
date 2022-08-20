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

	// ���� �������� �ʾҴٸ� Node_True �� �����Ͽ�, �ٸ� Tree �� ���� �ʰ� ������ ���̴�.
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
