

#include "BehaviorTree.h"
#include "StateComponent.h"
#include "Node/Node.h"
#include "Node/ActionNode.h"
#include "Node/ConditionNode.h"
#include "Node/SequenceNode.h"
#include "Node/SelectorNode.h"
#include "Node/DecoratorNode.h"


CBehaviorTree::CBehaviorTree() :
	m_Root(nullptr),
	m_CurrentNode(nullptr)
{
	
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& Tree)
{
}

CBehaviorTree::~CBehaviorTree()
{
	auto iter = m_NodeList.begin();
	auto iterEnd = m_NodeList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(*iter);
	}
}

CNode* CBehaviorTree::GetCurrentNode() const
{
	return m_CurrentNode;
}

void CBehaviorTree::SetCurrentNode(CNode* CurrentNode)
{
	m_CurrentNode = CurrentNode;
}

void CBehaviorTree::SetOwner(CStateComponent* StateComp)
{
	m_Owner = StateComp;
}

CStateComponent* CBehaviorTree::GetOwner() const
{
	return m_Owner;
}

void CBehaviorTree::SetAnimationMeshComponent(CAnimationMeshComponent* Mesh)
{
	m_AnimationMeshComp = Mesh;
}

CAnimationMeshComponent* CBehaviorTree::GetAnimationMeshComp() const
{
	return m_AnimationMeshComp;
}

CNode* CBehaviorTree::GetRootNode() const
{
	return m_Root;
}

void CBehaviorTree::SetRoot(class CNode* Node)
{
	m_Root = Node;
}

CNode* CBehaviorTree::FindNode(const std::string& Name)
{
	auto iter = m_NodeList.begin();
	auto iterEnd = m_NodeList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetName() == Name)
			return (*iter);
	}

	return nullptr;
}

//CNode* CBehaviorTree::FindNode(CNode* Node)
//{
//	auto iter = m_NodeList.begin();
//	auto iterEnd = m_NodeList.end();
//
//	for (; iter != iterEnd; ++iter)
//	{
//		if ((*iter) == Node)
//			return (*iter);
//	}
//
//	return nullptr;
//}

void CBehaviorTree::AddNode(CNode* Node)
{
	m_NodeList.push_back(Node);
}

void CBehaviorTree::DeleteNode(CNode* Node)
{
	auto iter = m_NodeList.begin();
	auto iterEnd = m_NodeList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == Node)
		{
			CCompositeNode* Parent = (CCompositeNode*)((*iter)->GetParent());

			if (Parent)
			{
				if (Parent->m_NodeType == Node_Type::Decorator)
				{
					((CDecoratorNode*)Parent)->SetChild(nullptr);
				}

				else
				{
					if (Parent)
						Parent->DeleteChild((*iter));
				}
			}
			

			SAFE_DELETE(*iter);
			m_NodeList.erase(iter);

			break;
		}
	}
}

void CBehaviorTree::Start()
{
	// 노드를 순회하며 ActionNode와 Condition Node에 한하여 단 한번만 초기화 한다.
	auto iter = m_NodeList.begin();
	auto iterEnd = m_NodeList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Init();
	}
}

bool CBehaviorTree::Init()
{
	return true;
}

void CBehaviorTree::Update(float DeltaTime)
{

}

void CBehaviorTree::PostUpdate(float DeltaTime)
{
	CGameObject* OwnerObject = m_Owner->GetGameObject();


	if (m_CurrentNode)
		m_CurrentNode->Invoke(DeltaTime);

	else if (m_Root)
	{
		m_Root->Invoke(DeltaTime);
	}

}

void CBehaviorTree::PrevRender()
{

}

void CBehaviorTree::Render()
{
}

void CBehaviorTree::PostRender()
{
}

bool CBehaviorTree::Save(FILE* File)
{
	bool Root = false;

	if (m_Root)
		Root = true;

	fwrite(&Root, sizeof(bool), 1, File);

	if (m_Root)
	{
		size_t	TypeID = m_Root->GetTypeID();
		fwrite(&TypeID, sizeof(size_t), 1, File);

		m_Root->Save(File);
	}

	return true;
}

bool CBehaviorTree::Load(FILE* File)
{
	bool Root = false;
	fread(&Root, sizeof(bool), 1, File);

	if (Root)
	{
		size_t	TypeID = 0;
		fread(&TypeID, sizeof(size_t), 1, File);

		if (!m_Root)
		{
			if (TypeID == typeid(CSequenceNode).hash_code())
			{
				m_Root = new CSequenceNode;
			}

			else if (TypeID == typeid(CSelectorNode).hash_code())
			{
				m_Root = new CSelectorNode;
			}

			m_Root->m_Owner = this;
			m_Root->m_Parent = nullptr;
			m_Root->m_Object = m_Owner->GetGameObject();
		}
		
		m_NodeList.push_back(m_Root);
		m_Root->Load(File);
		
	}

	return true;
}

bool CBehaviorTree::SaveOnly(FILE* File)
{
	/*
    std::list<class CNode*> m_NodeList;
	*/

	bool Root = false;
	if (m_Root)
	{
		Root = true;
		fwrite(&Root, sizeof(bool), 1, File);

		int Length = (int)m_Root->m_Name.length();

		fwrite(&Length, sizeof(int), 1, File);

		char Name[256] = {};
		strcpy_s(Name, m_Root->m_Name.c_str());

		fwrite(Name, sizeof(char), Length, File);
		fwrite(&m_Root->m_TypeID, sizeof(size_t), 1, File);

		fwrite(&m_Root->m_NodeType, sizeof(Node_Type), 1, File);

		m_Root->SaveOnly(File);
	}


	return true;
}

bool CBehaviorTree::LoadOnly(FILE* File)
{
	bool Root = false;

	fread(&Root, sizeof(bool), 1, File);

	if (Root)
	{
		int Length = 0;
		fread(&Length, sizeof(int), 1, File);

		char Name[256] = {};
		fread(Name, sizeof(char), Length, File);

		size_t RootTypeID = 0;
		fread(&RootTypeID, sizeof(size_t), 1, File);
		Node_Type Type;

		fread(&Type, sizeof(Node_Type), 1, File);

		CNode* Root = LoadNode(nullptr, RootTypeID);
		m_NodeList.push_back(Root);
		m_Root = Root;

		m_Root->LoadOnly(File);
	}

	return true;
}


CNode* CBehaviorTree::LoadNode(CNode* Parent, size_t TypeID)
{
	//if (m_NodeCreateCallback)
	//	return m_NodeCreateCallback(Parent, TypeID);

	//if (TypeID == typeid(CSelectorNode).hash_code())
	//{
	//	CSelectorNode* NewNode = new CSelectorNode;
	//	NewNode->SetParent(Parent);
	//	NewNode->SetOwner(this);
	//	NewNode->SetObject(m_Owner->GetGameObject());
	//	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	//	return NewNode;
	//}

	//else if (TypeID == typeid(CSequenceNode).hash_code())
	//{
	//	CSequenceNode* NewNode = new CSequenceNode;
	//	NewNode->SetParent(Parent);
	//	NewNode->SetOwner(this);
	//	NewNode->SetObject(m_Owner->GetGameObject());
	//	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	//	return NewNode;
	//}

	//else if (TypeID == typeid(CNoInterruptNode).hash_code())
	//{
	//	CNoInterruptNode* NewNode = new CNoInterruptNode;
	//	NewNode->SetParent(Parent);
	//	NewNode->SetOwner(this);
	//	NewNode->SetObject(m_Owner->GetGameObject());
	//	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	//	return NewNode;
	//}

	//else if (TypeID == typeid(CIdleNode).hash_code())
	//{
	//	CIdleNode* NewNode = new CIdleNode;
	//	NewNode->SetParent(Parent);
	//	NewNode->SetOwner(this);
	//	NewNode->SetObject(m_Owner->GetGameObject());
	//	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	//	return NewNode;
	//}

	////else if (TypeID == typeid(CMovePickingNode).hash_code())
	////{
	////	CMovePickingNode* NewNode = new CMovePickingNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	//else if (TypeID == typeid(CMoveInputCheckNode).hash_code())
	//{
	//	CMoveInputCheckNode* NewNode = new CMoveInputCheckNode;
	//	NewNode->SetParent(Parent);
	//	NewNode->SetOwner(this);
	//	NewNode->SetObject(m_Owner->GetGameObject());
	//	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	//	return NewNode;
	//}

	////else if (TypeID == typeid(CSkillQNode).hash_code())
	////{
	////	CSkillQNode* NewNode = new CSkillQNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CSkillWNode).hash_code())
	////{
	////	CSkillWNode* NewNode = new CSkillWNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CSkillENode).hash_code())
	////{
	////	CSkillENode* NewNode = new CSkillENode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CSkillRNode).hash_code())
	////{
	////	CSkillRNode* NewNode = new CSkillRNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CInputQCheckNode).hash_code())
	////{
	////	CInputQCheckNode* NewNode = new CInputQCheckNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CInputWCheckNode).hash_code())
	////{
	////	CInputWCheckNode* NewNode = new CInputWCheckNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CInputECheckNode).hash_code())
	////{
	////	CInputECheckNode* NewNode = new CInputECheckNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CInputRCheckNode).hash_code())
	////{
	////	CInputRCheckNode* NewNode = new CInputRCheckNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CCheckAttackTarget).hash_code())
	////{
	////	CCheckAttackTarget* NewNode = new CCheckAttackTarget;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	//else if (TypeID == typeid(CNormalAttack).hash_code())
	//{
	//	CNormalAttack* NewNode = new CNormalAttack;
	//	NewNode->SetParent(Parent);
	//	NewNode->SetOwner(this);
	//	NewNode->SetObject(m_Owner->GetGameObject());
	//	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	//	return NewNode;
	//}

	////else if (TypeID == typeid(CSkillEndCheckNode).hash_code())
	////{
	////	CSkillEndCheckNode* NewNode = new CSkillEndCheckNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CInSkillCheck).hash_code())
	////{
	////	CInSkillCheck* NewNode = new CInSkillCheck;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CCheckTurretAttackTarget).hash_code())
	////{
	////	CCheckTurretAttackTarget* NewNode = new CCheckTurretAttackTarget;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CCheckTurretAttackFrequency).hash_code())
	////{
	////	CCheckTurretAttackFrequency* NewNode = new CCheckTurretAttackFrequency;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	////else if (TypeID == typeid(CNegateNode).hash_code())
	////{
	////	CNegateNode* NewNode = new CNegateNode;
	////	NewNode->SetParent(Parent);
	////	NewNode->SetOwner(this);
	////	NewNode->SetObject(m_Owner->GetGameObject());
	////	NewNode->SetAnimationMeshComponent(m_AnimationMeshComp);

	////	return NewNode;
	////}

	//else if (TypeID == typeid(CNegateNode).hash_code())
	//{
	//	int a = 3;
	//}

	return nullptr;
}
