
#include "StateComponent.h"
#include "AnimationMeshComponent.h"
#include "BehaviorTree.h"
#include "State/State.h"
#include "../PathManager.h"
#include "../GameObject/GameObject.h"
#include "../Scene/SceneManager.h"

CStateComponent::CStateComponent()	:
	m_BehaviorTree(nullptr),
	m_TreeUpdate(false),
	m_TreeInit(false)
{
	SetTypeID<CStateComponent>();
	m_ComponentType = Component_Type::ObjectComponent;

	//m_BehaviorTree = new CBehaviorTree;
	//m_BehaviorTree->m_Owner = this;
}

CStateComponent::CStateComponent(const CStateComponent& com)	:
	CObjectComponent(com)
{

}

CStateComponent::~CStateComponent()
{
	SAFE_DELETE(m_BehaviorTree);

	auto iter = m_StateList.begin();
	auto iterEnd = m_StateList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(*iter);
	}
}

bool CStateComponent::Init()
{
	m_BehaviorTree->Init();

	return true;
}

void CStateComponent::Start()
{
	CObjectComponent::Start();
}

void CStateComponent::Update(float DeltaTime)
{
	if (!m_TreeInit)
	{
		m_BehaviorTree->Start();
		m_TreeInit = true;
	}

	auto iter = m_StateList.begin();
	auto iterEnd = m_StateList.end();

	for (; iter != iterEnd; )
	{
		(*iter)->Update(DeltaTime);

		if ((*iter)->IsEnd())
		{
			(*iter)->OnEnd();
			SAFE_DELETE(*iter);
			iter = m_StateList.erase(iter);
			iterEnd = m_StateList.end();

			/*if(m_StateList.empty())
				m_Object->SetOntHit(false);*/

			continue;
		}

		else
			++iter;
	}

	//if(m_TreeUpdate)
	//	m_BehaviorTree->Update(DeltaTime);
}

void CStateComponent::PostUpdate(float DeltaTime)
{
	auto iter = m_StateList.begin();
	auto iterEnd = m_StateList.end();

	for (; iter != iterEnd; )
	{
		(*iter)->PostUpdate(DeltaTime);

		if ((*iter)->IsEnd())
		{
			(*iter)->OnEnd();
			SAFE_DELETE(*iter);
			iter = m_StateList.erase(iter);
			iterEnd = m_StateList.end();

			/*if (m_StateList.empty())
				m_Object->SetOntHit(false);*/

			continue;
		}

		else
			++iter;
	}

	if (m_TreeUpdate)
		m_BehaviorTree->PostUpdate(DeltaTime);

}

void CStateComponent::PrevRender()
{
}

void CStateComponent::Render()
{
}

void CStateComponent::PostRender()
{
}

CStateComponent* CStateComponent::Clone()
{
	return new CStateComponent(*this);
}

bool CStateComponent::Save(FILE* File)
{
	CComponent::Save(File);

	m_BehaviorTree->Save(File);

	return true;
}

bool CStateComponent::Save(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	Save(File);

	fclose(File);

	return true;
}

bool CStateComponent::Save(const char* FileName, const std::string& PathName)
{
	char	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	if (!Save(FullPath))
		return false;

	return true;
}

bool CStateComponent::Load(const char* FileName, const std::string& PathName)
{
	char	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	if (!Load(FullPath))
		return false;

	return true;
}

bool CStateComponent::Load(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	Load(File);

	fclose(File);

	return true;
}

bool CStateComponent::SaveOnly(FILE* File)
{
	CComponent::SaveOnly(File);

	m_BehaviorTree->SaveOnly(File);

	return true;
}

bool CStateComponent::LoadOnly(FILE* File)
{
	CComponent::LoadOnly(File);

	m_BehaviorTree->LoadOnly(File);

	m_AnimationMeshComp = m_Object->FindComponentFromType<CAnimationMeshComponent>();
	m_BehaviorTree->m_Owner = this;
	m_BehaviorTree->SetAnimationMeshComponent(m_AnimationMeshComp);

	return true;
}


bool CStateComponent::Load(FILE* File)
{
	CComponent::Load(File);

	CAnimationMeshComponent* AnimMeshComp = m_Object->FindComponentFromType<CAnimationMeshComponent>();
	m_BehaviorTree->SetAnimationMeshComponent(AnimMeshComp);

	m_BehaviorTree->Load(File);

	return true;
}

CState* CStateComponent::GetState(int Idx) const
{
	auto iter = m_StateList.begin();

	std::advance(iter, Idx);

	return *iter;
}

void CStateComponent::SetAnimationMeshComponent(CAnimationMeshComponent* Mesh)
{
	m_AnimationMeshComp = Mesh;
	
	if (m_BehaviorTree)
		m_BehaviorTree->SetAnimationMeshComponent(m_AnimationMeshComp);
}

CAnimationMeshComponent* CStateComponent::GetAnimationMeshComp() const
{
	return m_AnimationMeshComp;
}

CState* CStateComponent::AddState(const std::string& Name, CGameObject* SourceObject)
{
	CState* FindState = CSceneManager::GetInst()->GetStateManager()->FindState(Name);

	if (FindState)
	{
		CState* CloneState = FindState->Clone();

		CloneState->SetName(Name);
		CloneState->SetOwner(this);
		CloneState->SetOwnerObject(m_Object);
		CloneState->SetSourceObject(SourceObject);

		m_StateList.push_back(CloneState);

		CloneState->OnStart();

		return CloneState;
	}

	else
		return nullptr;
}

bool CStateComponent::DeleteState(CState* State)
{
	auto iter = m_StateList.begin();
	auto iterEnd = m_StateList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == State)
		{
			SAFE_DELETE(*iter);
			m_StateList.erase(iter);
			State->OnEnd();
			return true;
		}
	}

	return false;
}

bool CStateComponent::DeleteState(const std::string& Name)
{
	CState* State = FindState(Name);

	if (!State)
		return false;

	auto iter = m_StateList.begin();
	auto iterEnd = m_StateList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == State)
		{
			SAFE_DELETE(*iter);
			m_StateList.erase(iter);
			State->OnEnd();
			return true;
		}
	}

	return false;
}

CState* CStateComponent::FindState(const std::string& Name)
{
	auto iter = m_StateList.begin();
	auto iterEnd = m_StateList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetName() == Name)
			return (*iter);
	}

	return nullptr;
}
