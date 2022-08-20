
#include "DecoratorNode.h"
#include "../BehaviorTree.h"

CDecoratorNode::CDecoratorNode()	:
	m_Child(nullptr)
{
	m_NodeType = Node_Type::Decorator;
	SetTypeID(typeid(CDecoratorNode).hash_code());
}

CDecoratorNode::CDecoratorNode(const CDecoratorNode& Node)	:
	CNode(Node)
{
}

CDecoratorNode::~CDecoratorNode()
{
}

bool CDecoratorNode::Save(FILE* File)
{
	CNode::Save(File);

	size_t Count = 0;

	if (m_Child)
		Count = 1;

	fwrite(&Count, sizeof(size_t), 1, File);

	if (Count != 0 && m_Child)
	{
		size_t ChildTypeID = m_Child->GetTypeID();
		fwrite(&ChildTypeID, sizeof(size_t), 1, File);
		m_Child->Save(File);
	}

	return true;
}

bool CDecoratorNode::Load(FILE* File)
{
	CNode::Load(File);

	size_t Count = -1;
	fread(&Count, sizeof(size_t), 1, File);

	if(Count > 0)
	{
		size_t ChildTypeID = -1;
		fread(&ChildTypeID, sizeof(size_t), 1, File);

		CNode* Child = m_Owner->LoadNode(this, ChildTypeID);
		m_Owner->AddNode(Child);
		Child->Load(File);
		SetChild(Child);
	}

	return true;
}

bool CDecoratorNode::SaveOnly(FILE* File)
{
	CNode::SaveOnly(File);

	size_t Count = 0;

	if (m_Child)
		Count = 1;

	fwrite(&Count, sizeof(size_t), 1, File);

	if (Count != 0 && m_Child)
	{
		size_t ChildTypeID = m_Child->GetTypeID();

		fwrite(&ChildTypeID, sizeof(size_t), 1, File);
		m_Child->SaveOnly(File);
	}

	return true;
}

bool CDecoratorNode::LoadOnly(FILE* File)
{
	CNode::LoadOnly(File);

	size_t Count = -1;
	fread(&Count, sizeof(size_t), 1, File);

	if(Count != -1)
	{
		size_t TypeID = -1;
		fread(&TypeID, sizeof(size_t), 1, File);

		CNode* Child = m_Owner->LoadNode(this, TypeID);
		Child->LoadOnly(File);

		SetChild(Child);

		m_Owner->AddNode(Child);
	}

	return true;
}
