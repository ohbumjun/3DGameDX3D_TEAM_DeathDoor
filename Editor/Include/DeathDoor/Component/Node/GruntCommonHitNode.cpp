#include "GruntCommonHitNode.h"

CGruntCommonHitNode::CGruntCommonHitNode()
{
	SetTypeID(typeid(CGruntCommonHitNode).hash_code());
}

CGruntCommonHitNode::CGruntCommonHitNode(const CGruntCommonHitNode& Node)
{
}

CGruntCommonHitNode::~CGruntCommonHitNode()
{
}

void CGruntCommonHitNode::Init()
{
}

NodeResult CGruntCommonHitNode::OnStart(float DeltaTime)
{
	return NodeResult();
}

NodeResult CGruntCommonHitNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CGruntCommonHitNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
