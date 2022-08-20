
#include "CompositeNode.h"
#include "../BehaviorTree.h"

CCompositeNode::CCompositeNode()
{
	SetTypeID(typeid(CCompositeNode).hash_code());
}

CCompositeNode::CCompositeNode(const CCompositeNode& Node)	:
	CNode(Node)
{
}

CCompositeNode::~CCompositeNode()
{

}

bool CCompositeNode::Save(FILE* File)
{
	CNode::Save(File);

	size_t Count = m_ChildrenList.size();
	fwrite(&Count, sizeof(size_t), 1, File);

	auto iter = m_ChildrenList.begin();
	auto iterEnd = m_ChildrenList.end();

	for (; iter != iterEnd; ++iter)
	{
		size_t ChildTypeID = (*iter)->GetTypeID();
		fwrite(&ChildTypeID, sizeof(size_t), 1, File);
		(*iter)->Save(File);
	}

	return true;
}

bool CCompositeNode::Load(FILE* File)
{
	CNode::Load(File);

	size_t Count = -1;
	fread(&Count, sizeof(size_t), 1, File);

	for (size_t i = 0; i < Count; ++i)
	{
		size_t ChildTypeID = -1;
		fread(&ChildTypeID, sizeof(size_t), 1, File);
		CNode* Child = m_Owner->LoadNode(this, ChildTypeID);
		m_Owner->AddNode(Child);
		Child->Load(File);
		AddChild(Child);
	}

	return true;
}

bool CCompositeNode::SaveOnly(FILE* File)
{
	CNode::SaveOnly(File);

	size_t Count = m_ChildrenList.size();

	auto iter = m_ChildrenList.begin();
	auto iterEnd = m_ChildrenList.end();

	fwrite(&Count, sizeof(size_t), 1, File);

	for (; iter != iterEnd; ++iter)
	{
		size_t ChildTypeID = (*iter)->GetTypeID();

		fwrite(&ChildTypeID, sizeof(size_t), 1, File);
		(*iter)->SaveOnly(File);
	}

	return true;
}

bool CCompositeNode::LoadOnly(FILE* File)
{
	CNode::LoadOnly(File);

	size_t Count = 0;
	fread(&Count, sizeof(size_t), 1, File);

	for (size_t i = 0 ; i < Count; ++i)
	{
		size_t TypeID = -1;
		fread(&TypeID, sizeof(size_t), 1, File);

		CNode* Child = m_Owner->LoadNode(this, TypeID);
		Child->LoadOnly(File);
		m_ChildrenList.push_back(Child);
		m_Owner->AddNode(Child);
	}

	return true;
}

