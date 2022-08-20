
#include "SequenceNode.h"
#include "../BehaviorTree.h"

CSequenceNode::CSequenceNode()
{
	SetTypeID(typeid(CSequenceNode).hash_code());
	m_NodeType = Node_Type::Sequence;
}

CSequenceNode::CSequenceNode(const CSequenceNode& Node)	:
	CCompositeNode(Node)
{
}

CSequenceNode::~CSequenceNode()
{
}

NodeResult CSequenceNode::Invoke(float DeltaTime)
{
	auto iter = GetChildrenList().begin();
	auto iterEnd = GetChildrenList().end();

	for (; iter != iterEnd; ++iter)
	{
		NodeResult Result = (*iter)->Invoke(DeltaTime);
		// Sequence�� �ϳ��� �ڽ��̶� false�� �����ϸ� �ڽŵ� false�� ����
		if (Result == NodeResult::Node_False)
		{
			// �� �ڽ��� �ϳ��� false��� ���� �̹� �����ӿ� �� �ڽĵ��� Action�� �������� �ʰڴٴ� �ǹ��̹Ƿ�
			// ���� �����ӿ� �ٽ� ���� �ڽ��� ��ȸ�ؼ� ��� true�� ��ȯ�ؼ� �ڽĵ��� Action�� �����Ҷ���
			// OnStart���� ȣ���ϰ� �ؾ��ϹǷ� ��� m_CallStart�� false�� �����
			SetChildrenCallStart(false);
			return  NodeResult::Node_False;
		}

		else if (Result == NodeResult::Node_Running)
			return  NodeResult::Node_Running;
	}

	return NodeResult::Node_True;
}

NodeResult CSequenceNode::OnStart(float DeltaTime)
{
	return NodeResult();
}

NodeResult CSequenceNode::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CSequenceNode::OnEnd(float DeltaTime)
{
	return NodeResult();
}
