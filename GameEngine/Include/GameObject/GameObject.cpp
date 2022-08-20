
#include "GameObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Navigation3DManager.h"
#include "../PathManager.h"
#include "../Component/NavAgent.h"
#include "../Component/ParticleComponent.h"
#include "../Component/PaperBurnComponent.h"
#include "../ObjectPool.h"

CGameObject::CGameObject() :
	m_Scene(nullptr),
	m_Parent(nullptr),
	m_LifeSpan(0.f),
	m_NavAgent(nullptr),
	m_IsEnemy(false),
	m_ExcludeSceneSave(false),
	m_NoDestroyFromSceneChange(false),
	m_InPool(false)
{
	SetTypeID<CGameObject>();
	m_ObjectType = Object_Type::None;
}

CGameObject::CGameObject(const CGameObject& obj)
{
	*this = obj;

	m_RefCount = 0;

	if (obj.m_RootComponent)
	{
		m_RootComponent = obj.m_RootComponent->Clone();
		
		m_RootComponent->SetScene(m_Scene);

		m_RootComponent->SetGameObject(this);

		m_RootComponent->SetSceneComponent(this);
	}

	m_vecObjectComponent.clear();

	size_t	Size = obj.m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent.push_back(obj.m_vecObjectComponent[i]->Clone());

		m_vecObjectComponent[i]->SetScene(m_Scene);

		m_vecObjectComponent[i]->SetGameObject(this);
	}
	
	m_Scene->AddObject(this);
}

CGameObject::~CGameObject()
{

}

void CGameObject::Enable(bool Enable)
{
	CRef::Enable(Enable);

	if (m_RootComponent)
	{
		m_RootComponent->Enable(Enable);
	}

	size_t Size = m_vecObjectComponent.size();
	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->Enable(Enable);
	}

	Size = m_vecChildObject.size();
	for (size_t i = 0; i > Size; ++i)
	{
		m_vecChildObject[i]->Enable(Enable);
	}
}

void CGameObject::SetScene(CScene* Scene)
{
	m_Scene = Scene;

	if (m_RootComponent)
	{
		m_RootComponent->SetScene(Scene);
	}

	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->SetScene(Scene);
	}
}

void CGameObject::Destroy()
{
	CRef::Destroy();

	if (m_InPool)
	{
		Reset();
		//CObjectPool::GetInst()->ReturnToPool(this);
	}

	else
	{
		CRef::Destroy();

		if (m_RootComponent)
			m_RootComponent->Destroy();

		size_t	Size = m_vecObjectComponent.size();

		for (size_t i = 0; i < Size; ++i)
		{
			m_vecObjectComponent[i]->Destroy();
		}
	}
}

void CGameObject::AddChildObject(CGameObject* Obj, const std::string& SocketName)
{
	Obj->m_Parent = this;
	m_vecChildObject.push_back(Obj);

	if(m_RootComponent)
		m_RootComponent->AddChild(Obj, SocketName);

	/*Obj->GetRootComponent()->m_Parent = GetRootComponent();

	m_RootComponent->AddChild(Obj->GetRootComponent(), SocketName);*/
}

void CGameObject::DeleteObj()
{
	if (!m_Parent)
	{
		Destroy();
		m_SceneComponentList.clear();

		return;
	}

	else
	{
		m_SceneComponentList.clear();
		m_Parent->DeleteChildObj(this);
		//Destroy();
	}
}

bool CGameObject::DeleteChildObj(const std::string& Name)
{
	size_t	Size = m_vecChildObject.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecChildObject[i]->GetName() == Name)
		{
			CGameObject* DeleteTarget = m_vecChildObject[i];

			auto	iter = m_vecChildObject.begin() + i;

			if ((*iter)->m_vecChildObject.size() > 0)
			{
				auto FirstChildIter = (*iter)->m_vecChildObject.begin();

				CGameObject* FirstChildObj = (*iter)->m_vecChildObject[0];

				(*iter)->m_vecChildObject.erase(FirstChildIter);

				m_vecChildObject[i] = nullptr;

				FirstChildObj->m_Parent = this;
				m_vecChildObject[i] = FirstChildObj;
			}

			else
				m_vecChildObject.erase(iter);

			m_Scene->EraseObjFromList(DeleteTarget);

			return true;
		}

		if (m_vecChildObject[i]->DeleteChildObj(Name))
			return true;
	}

	return false;
}

bool CGameObject::DeleteChildObj(CGameObject* Child)
{
	size_t	Size = m_vecChildObject.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecChildObject[i] == Child)
		{
			CGameObject* DeleteTarget = m_vecChildObject[i];

			auto	iter = m_vecChildObject.begin() + i;

			if ((*iter)->m_vecChildObject.size() > 0)
			{
				auto FirstChildIter = (*iter)->m_vecChildObject.begin();

				CGameObject* FirstChildObj = (*iter)->m_vecChildObject[0];

				(*iter)->m_vecChildObject.erase(FirstChildIter);

				m_vecChildObject[i] = nullptr;

				FirstChildObj->m_Parent = this;
				m_vecChildObject[i] = FirstChildObj;
			}

			else
				m_vecChildObject.erase(iter);

			m_Scene->EraseObjFromList(DeleteTarget);

			return true;
		}

		if (m_vecChildObject[i]->DeleteChildObj(Child))
			return true;
	}

	return false;
}

void CGameObject::ClearParent()
{
	if (!m_Parent)
		return;

	size_t Count = m_Parent->m_vecChildObject.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_Parent->m_vecChildObject[i] == this)
		{
			auto iter = m_Parent->m_vecChildObject.begin();
			std::advance(iter, i);

			m_Parent->m_vecChildObject.erase(iter);
			break;
		}
	}

	m_Parent = nullptr;
}

bool CGameObject::DeleteObjectComponent(const std::string& Name)
{
	size_t Count = m_vecObjectComponent.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecObjectComponent[i]->GetName() == Name)
		{
			auto iter = m_vecObjectComponent.begin();
			std::advance(iter, i);

			m_vecObjectComponent.erase(iter);
			return true;
		}
	}

	return false;
}

CComponent* CGameObject::FindComponent(const std::string& Name)
{
	{
		auto	iter = m_SceneComponentList.begin();
		auto	iterEnd = m_SceneComponentList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->GetName() == Name)
				return *iter;
		}
	}

	{
		auto	iter = m_vecObjectComponent.begin();
		auto	iterEnd = m_vecObjectComponent.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->GetName() == Name)
				return *iter;
		}
	}

	return nullptr;
}

void CGameObject::GetAllSceneComponentsName(std::vector<FindComponentName>& vecNames)
{
	if (!m_RootComponent)
		return;

	m_RootComponent->GetAllSceneComponentsName(vecNames);
}

void CGameObject::GetAllSceneComponentsPointer(std::vector<CSceneComponent*>& OutVecSceneComp)
{
	if (!m_RootComponent)
	{
		return;
	}

	m_RootComponent->GetAllSceneComponentsPointer(OutVecSceneComp);
}

void CGameObject::GetAllObjectComponentsPointer(std::vector<CObjectComponent*>& OutVecObjComp)
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		OutVecObjComp.push_back(m_vecObjectComponent[i]);
	}
}

void CGameObject::Start()
{
	m_StartCalled = true;

	if (m_RootComponent)
		m_RootComponent->Start();

	size_t	Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->Start();
	}
}

bool CGameObject::Init()
{
	return true;
}

void CGameObject::Update(float DeltaTime)
{
	if (m_LifeSpan > 0.f)
	{
		m_LifeSpan -= DeltaTime;

		if (m_LifeSpan <= 0.f)
		{
			m_LifeSpan = 0.f;
			Destroy();
			return;
		}
	}

	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->Update(DeltaTime);
	}

	if (m_RootComponent)
		m_RootComponent->Update(DeltaTime);
}

void CGameObject::PostUpdate(float DeltaTime)
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->PostUpdate(DeltaTime);
	}

	if (m_RootComponent)
		m_RootComponent->PostUpdate(DeltaTime);
}

void CGameObject::AddCollision()
{
	if (m_RootComponent)
		m_RootComponent->CheckCollision();
}

void CGameObject::PrevRender()
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->PrevRender();
	}

	if (m_RootComponent)
		m_RootComponent->PrevRender();
}

void CGameObject::Render()
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->Render();
	}

	if (m_RootComponent)
		m_RootComponent->Render();
}

void CGameObject::PostRender()
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->PostRender();
	}

	if (m_RootComponent)
		m_RootComponent->PostRender();
}

CGameObject* CGameObject::Clone()
{
	return new CGameObject(*this);
}

bool CGameObject::Save(FILE* File)
{
	CRef::Save(File);

	if (m_RootComponent)
	{
		bool	Root = true;
		fwrite(&Root, sizeof(bool), 1, File);

		size_t	TypeID = m_RootComponent->GetTypeID();
		fwrite(&TypeID, sizeof(size_t), 1, File);

		m_RootComponent->Save(File);
	}

	else
	{
		bool	Root = false;
		fwrite(&Root, sizeof(bool), 1, File);
	}

	fwrite(&m_ObjectType, sizeof(Object_Type), 1, File);
	fwrite(&m_IsEnemy, sizeof(bool), 1, File);

	int	ObjComponentCount = (int)m_vecObjectComponent.size();

	fwrite(&ObjComponentCount, sizeof(int), 1, File);

	for (int i = 0; i < ObjComponentCount; ++i)
	{
		size_t	TypeID = m_vecObjectComponent[i]->GetTypeID();
		fwrite(&TypeID, sizeof(size_t), 1, File);

		m_vecObjectComponent[i]->Save(File);
	}

	return true;
}

bool CGameObject::Load(FILE* File)
{
	CRef::Load(File);

	bool	Root = false;
	fread(&Root, sizeof(bool), 1, File);

	if (Root)
	{
		size_t	TypeID = 0;
		fread(&TypeID, sizeof(size_t), 1, File);

		CSceneManager::GetInst()->CallCreateComponent(this, TypeID);

		if (!m_RootComponent->Load(File))
			return false;
	}

	fread(&m_ObjectType, sizeof(Object_Type), 1, File);
	fread(&m_IsEnemy, sizeof(bool), 1, File);

	int	ObjComponentCount = 0;

	fread(&ObjComponentCount, sizeof(int), 1, File);

	for (int i = 0; i < ObjComponentCount; ++i)
	{
		size_t	TypeID = 0;
		fread(&TypeID, sizeof(size_t), 1, File);

		CComponent* Component = CSceneManager::GetInst()->CallCreateComponent(this, TypeID);

		Component->SetGameObject(this);

		if (!Component->Load(File))
			return false;
	}

	// NavAgent가 있을 경우, 처리해준다.
	for (int i = 0; i < ObjComponentCount; ++i)
	{
		if (m_vecObjectComponent[i]->CheckType<CNavAgent>())
		{
			SetNavAgent((CNavAgent*)m_vecObjectComponent[i].Get());

			if (m_RootComponent)
			{
				((CNavAgent*)m_vecObjectComponent[i].Get())->SetUpdateComponent(m_RootComponent);
			}
			break;
		}
	}

	return true;
}

bool CGameObject::Save(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	Save(File);

	fclose(File);

	return true;
}

bool CGameObject::Load(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;

	if (!Load(File))
	{
		fclose(File);
		return false;
	}

	fclose(File);

	Start();

	return true;
}

bool CGameObject::Save(const char* FileName, const std::string& PathName)
{
	char	FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	Save(FullPath);

	return true;
}

bool CGameObject::Load(const char* FileName, const std::string& PathName)
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

bool CGameObject::SaveHierarchy(FILE* File)
{
	CRef::Save(File);

	size_t Count = m_vecChildObject.size();
	fwrite(&Count, sizeof(size_t), 1, File);

	for (size_t i = 0; i < Count; ++i)
	{
		size_t ChildTypeID = m_vecChildObject[i]->GetTypeID();
		fwrite(&ChildTypeID, sizeof(size_t), 1, File);
		m_vecChildObject[i]->SaveHierarchy(File);
	}

	if (m_RootComponent)
	{
		bool	Root = true;
		fwrite(&Root, sizeof(bool), 1, File);

		size_t	TypeID = m_RootComponent->GetTypeID();
		fwrite(&TypeID, sizeof(size_t), 1, File);

		m_RootComponent->Save(File);
	}

	else
	{
		bool	Root = false;
		fwrite(&Root, sizeof(bool), 1, File);
	}

	fwrite(&m_ObjectType, sizeof(Object_Type), 1, File);
	fwrite(&m_IsEnemy, sizeof(bool), 1, File);

	int	ObjComponentCount = (int)m_vecObjectComponent.size();

	fwrite(&ObjComponentCount, sizeof(int), 1, File);

	for (int i = 0; i < ObjComponentCount; ++i)
	{
		size_t	TypeID = m_vecObjectComponent[i]->GetTypeID();
		fwrite(&TypeID, sizeof(size_t), 1, File);

		m_vecObjectComponent[i]->Save(File);
	}

	return true;
}

bool CGameObject::LoadHierarchy(FILE* File, CScene* NextScene)
{
	CRef::Load(File);

	size_t ChildCount = 0;
	fread(&ChildCount, sizeof(size_t), 1, File);
	CGameObject* Child = nullptr;
	for (size_t i = 0; i < ChildCount; ++i)
	{
		size_t ChildTypeID = 0;
		fread(&ChildTypeID, sizeof(size_t), 1, File);
		Child = CSceneManager::GetInst()->CreateObjectByTypeID(ChildTypeID);

		if (!Child)
		{
			Child = CSceneManager::GetInst()->CallCreateObject(NextScene, ChildTypeID);
		}

		if (Child)
		{
			// CurrentScene이 아니라 NextScene에다가 AddObject해줘야 한다
			NextScene->AddObject(Child);
			Child->LoadHierarchy(File, NextScene);
		}

		Child->m_Parent = this;
		m_vecChildObject.push_back(Child);
	}


	bool	Root = false;
	fread(&Root, sizeof(bool), 1, File);

	if (Root)
	{
		size_t	TypeID = 0;
		fread(&TypeID, sizeof(size_t), 1, File);

		CSceneManager::GetInst()->CallCreateComponent(this, TypeID);

		if(NextScene)
			m_RootComponent->SetScene(NextScene);
		else
			m_RootComponent->SetScene(m_Scene);

		if (!m_RootComponent->Load(File))
			return false;
	}

	fread(&m_ObjectType, sizeof(Object_Type), 1, File);
	fread(&m_IsEnemy, sizeof(bool), 1, File);

	int	ObjComponentCount = 0;

	fread(&ObjComponentCount, sizeof(int), 1, File);


	for (int i = 0; i < ObjComponentCount; ++i)
	{
		size_t	TypeID = 0;
		fread(&TypeID, sizeof(size_t), 1, File);

		CComponent* Component = CSceneManager::GetInst()->CallCreateComponent(this, TypeID);

		if (!Component->Load(File))
			return false;

		Component->SetGameObject(this);
	}

	// NavAgent가 있을 경우, 처리해준다.
	for (int i = 0; i < ObjComponentCount; ++i)
	{
		if (m_vecObjectComponent[i]->CheckType<CNavAgent>())
		{
			SetNavAgent((CNavAgent*)m_vecObjectComponent[i].Get());

			if (m_RootComponent)
			{
				((CNavAgent*)m_vecObjectComponent[i].Get())->SetUpdateComponent(m_RootComponent);
			}
			break;
		}
	}
	

	//if(Child)
	//	AddChildObject(Child);

	CSceneManager::GetInst()->CallObjectDataSet(this, m_Name);

	return true;
}

bool CGameObject::SaveHierarchy(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	SaveHierarchy(File);

	fclose(File);

	return true;
}

bool CGameObject::LoadHierarchy(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;

	if (!LoadHierarchy(File))
	{
		fclose(File);
		return false;
	}

	fclose(File);

	// Pool에서 꺼낸 오브젝트들은 CObjectPool::GetXXX 함수 호출해서 Start를 호출할 것이다
	if (!m_InPool)
		Start();

	return true;
}

bool CGameObject::SaveOnly(CComponent* Component, const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
	{
		assert(false);
		return false;
	}

	size_t TypeID = Component->GetTypeID();
	fwrite(&TypeID, sizeof(size_t), 1, File);

	bool Ret = Component->SaveOnly(File);

	fclose(File);

	return Ret;
}

bool CGameObject::SaveOnly(const std::string& ComponentName, const char* FullPath)
{
	CComponent* Component = FindComponent(ComponentName);


	if (!Component)
	{
		assert(false);
		return false;
	}

	return SaveOnly(Component, FullPath);
}

bool CGameObject::LoadOnly(const char* FullPath, CComponent*& OutCom)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
	{
		assert(false);
		return false;
	}

	size_t TypeID = -1;
	fread(&TypeID, sizeof(size_t), 1, File);

	CComponent* Component = nullptr;
	Component = CSceneManager::GetInst()->CallCreateComponent(this, TypeID);

	if (!Component)
	{
		assert(false);
		return false;
	}

	if (Component->GetComponentType() == Component_Type::SceneComponent && 
		Component != m_RootComponent)
	{
		SetRootComponent((CSceneComponent*)Component);
	}

	bool Ret = Component->LoadOnly(File);

	if (Component->GetTypeID() == typeid(CNavAgent).hash_code())
	{
		m_NavAgent = (CNavAgent*)Component;
		m_NavAgent->SetUpdateComponent(GetRootComponent());
	}

	Component->Start();

	OutCom = Component;

	fclose(File);

	return Ret;
}

void CGameObject::Reset()
{
	m_Scene = nullptr;
	SetWorldPos(FLT_MAX, FLT_MAX, FLT_MAX);
	m_RootComponent->GetTransform()->ForceUpdateMat();

	m_LifeSpan = 0.f;

	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->Reset();
	}

	if (m_RootComponent)
		m_RootComponent->Reset();
}

void CGameObject::StartParticle(const Vector3& WorldPos)
{
	Enable(true);

	SetWorldPos(WorldPos);

	std::vector<CParticleComponent*> vecParticleComponents;
	FindAllSceneComponentFromType(vecParticleComponents);

	size_t vecSize = vecParticleComponents.size();

	for (size_t i = 0; i < vecSize; ++i)
	{
		vecParticleComponents[i]->StartParticle(WorldPos);
	}
}

void CGameObject::Move(const Vector3& EndPos)
{
	size_t	Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecObjectComponent[i]->CheckType<CNavAgent>())
		{
			((CNavAgent*)m_vecObjectComponent[i].Get())->Move(EndPos);
			break;
		}
	}
}

void CGameObject::AddPath(const Vector3& TargetPos)
{
	if (m_NavAgent)
	{
		m_NavAgent->AddTargetPos(TargetPos);
	}
}

void CGameObject::ClearPath()
{
	m_NavAgent->m_PathList.clear();
}

void CGameObject::SetNavManagerLandScape(CLandScape* LandScape)
{
	m_Scene->GetNavigation3DManager()->SetLandScape(LandScape);
}

bool CGameObject::IsNavAgentPathListEmpty() const
{
	return m_NavAgent->m_PathList.empty();
}

void CGameObject::SetUpdateByMat(bool UpdateByMat)
{
	m_RootComponent->SetUpdateByMat(UpdateByMat);
}

void CGameObject::DecomposeWorld()
{
	m_RootComponent->DecomposeWorld();
}

void CGameObject::SetTransformByWorldMatrix(const Matrix& matTRS)
{
	m_RootComponent->SetTransformByWorldMatrix(matTRS);
}

void CGameObject::AddWorldPosByLocalAxis(AXIS Axis, float Amount)
{
	m_RootComponent->AddWorldPosByLocalAxis(Axis, Amount);
}

void CGameObject::AddWorldPosByLocalAxis(const Vector3& Pos)
{
	m_RootComponent->AddWorldPosByLocalAxis(Pos);
}

