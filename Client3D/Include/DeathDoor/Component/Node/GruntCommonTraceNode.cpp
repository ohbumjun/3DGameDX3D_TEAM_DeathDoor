#include "GruntCommonTraceNode.h"

CGruntCommonTraceNode::CGruntCommonTraceNode()
{
	SetTypeID(typeid(CGruntCommonTraceNode).hash_code());

}

CGruntCommonTraceNode::CGruntCommonTraceNode(const CGruntCommonTraceNode& Node)
{
}

CGruntCommonTraceNode::~CGruntCommonTraceNode()
{
}

void CGruntCommonTraceNode::Init()
{
}

NodeResult CGruntCommonTraceNode::OnStart(float DeltaTime)
{
	return NodeResult();
}

NodeResult CGruntCommonTraceNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CGruntCommonTraceNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
