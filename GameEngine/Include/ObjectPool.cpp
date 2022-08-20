
#include "ObjectPool.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Resource/Excel/ExcelManager.h"
#include "PathManager.h"

#include <sstream>

DEFINITION_SINGLE(CObjectPool)

CObjectPool::CObjectPool()
{

}

CObjectPool::~CObjectPool()
{
	// auto iter = m_mapProjectile.begin();
	// auto iterEnd = m_mapProjectile.end();
	// 
	// for (; iter != iterEnd; ++iter)
	// {
	// 	SAFE_RELEASE(iter->second);
	// }

	//iter = m_mapMonster.begin();
	//iterEnd = m_mapMonster.end();

	//for (; iter != iterEnd; ++iter)
	//{
	//	SAFE_RELEASE(iter->second);
	//}

	//iter = m_mapParticle.begin();
	//iterEnd = m_mapParticle.end();

	//for (; iter != iterEnd; ++iter)
	//{
	//	SAFE_RELEASE(iter->second);
	//}
}

void CObjectPool::Init()
{

}

CGameObject* CObjectPool::GetProjectile(const std::string& Name, class CScene* Scene, bool CallStart)
{
	auto iter = m_mapProjectile.find(Name);

	if (iter == m_mapProjectile.end())
		return nullptr;

	CGameObject* Object = iter->second;

	Object->m_Active = true;

	Scene->AddObject(Object);

	Object->Enable(true);

	if(CallStart)
		Object->Start();

	m_mapProjectile.erase(iter);

	return Object;
}

CGameObject* CObjectPool::GetMonster(const std::string& Name, class CScene* Scene, bool CallStart)
{
	auto iter = m_mapMonster.find(Name);

	if (iter == m_mapMonster.end())
		return nullptr;

	CGameObject* Object = iter->second;

	Object->m_Active = true;
	Scene->AddObject(Object);

	Object->Enable(true);

	if(CallStart)
		Object->Start();

	m_mapMonster.erase(iter);

	return Object;
}

CGameObject* CObjectPool::GetParticle(const std::string& Name, class CScene* Scene, bool CallStart)
{
	auto iter = m_mapParticle.find(Name);

	if (iter == m_mapParticle.end())
		return nullptr;

	CGameObject* Object = iter->second;

	Object->m_Active = true;
	Scene->AddObject(Object);

	Object->Enable(true);

	if (CallStart)
		Object->Start();

	m_mapParticle.erase(iter);

	return Object;
}

CGameObject* CObjectPool::GetMapObject(const std::string& Name, CScene* Scene, bool CallStart)
{
	auto iter = m_mapMapObject.find(Name);

	if (iter == m_mapMapObject.end())
		return nullptr;

	CGameObject* Object = iter->second;

	Object->m_Active = true;
	Scene->AddObject(Object);

	Object->Enable(true);

	if (CallStart)
		Object->Start();

	m_mapMapObject.erase(iter);

	return Object;
}

void CObjectPool::ReturnToPool(CGameObject* Object)
{
	if (Object->GetObjectType() == Object_Type::Particle)
	{
		m_mapParticle.insert(std::make_pair(Object->GetName(), Object));
	}

	else if (Object->GetObjectType() == Object_Type::Monster)
	{
		m_mapMonster.insert(std::make_pair(Object->GetName(), Object));
	}

	else if (Object->GetObjectType() == Object_Type::Projectile)
	{
		m_mapProjectile.insert(std::make_pair(Object->GetName(), Object));
	}

	else if (Object->GetObjectType() == Object_Type::MapObject)
	{
		m_mapMapObject.insert(std::make_pair(Object->GetName(), Object));
	}
}

void CObjectPool::LoadPoolData(const char* FileName)
{
	CResourceManager::GetInst()->LoadCSV(m_DataName, FileName);
}

void CObjectPool::CreatePoolObject(const std::string& PathName)
{
	CExcelData* Data = CResourceManager::GetInst()->FindCSV(m_DataName);

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	std::vector<std::string> vecNames;
	Data->GetRowNames(vecNames);

	size_t Count = vecNames.size();

	for (size_t i = 0; i < Count; ++i)
	{
		char FullPath[MAX_PATH] = {};
		strcpy_s(FullPath, Info->PathMultibyte);

		std::string ObjectName = vecNames[i];

		Row* row = Data->GetRow(ObjectName);

		std::stringstream ss;
		std::string FileName; 

		ss << (*row)[0];

		int ObjectCount = 0;

		ss >> ObjectCount;

		ss.clear();

		ss << (*row)[1];

		ss >> FileName;

		strcat_s(FullPath, FileName.c_str());

		for (int j = 0; j < ObjectCount; ++j)
		{
			CGameObject* Object = new CGameObject;
			Object->SetName(ObjectName);
			Object->SetInPool(true);
			Object->SetScene(CSceneManager::GetInst()->GetScene());

			// 해당 Object 들은, Scene Save 가 안되게 해야 한다. (OBJ) 추가
			Object->ExcludeFromSceneSave();

			bool Result = Object->LoadHierarchy(FullPath);

			Object->SetScene(CSceneManager::GetInst()->GetScene());
			Object->SetWorldPos(FLT_MAX, FLT_MAX, FLT_MAX);

			Object->Enable(false);

			if (Result)
			{
				Object->GetRootComponent()->GetTransform()->ForceUpdateMat();

				switch (Object->GetObjectType())
				{
				case Object_Type::Projectile:
					m_mapProjectile.insert(std::make_pair(ObjectName, Object));
					break;
				case Object_Type::Particle:
					m_mapParticle.insert(std::make_pair(ObjectName, Object));
					break;
				case Object_Type::Monster:
					m_mapMonster.insert(std::make_pair(ObjectName, Object));
					break;
				case Object_Type::MapObject:
					m_mapMapObject.insert(std::make_pair(ObjectName, Object));
					break;
				}
			}
			else
			{
				SAFE_DELETE(Object);
				// Object->Destroy();
			}
		}

		memset(FullPath, 0, MAX_PATH);
	}
}

void CObjectPool::RefreshNewScene(CScene* Scene)
{
	auto iter = m_mapMonster.begin();
	auto iterEnd = m_mapMonster.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->SetScene(Scene);
	}

	iter = m_mapParticle.begin();
	iterEnd = m_mapParticle.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->SetScene(Scene);
	}

	iter = m_mapProjectile.begin();
	iterEnd = m_mapProjectile.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->SetScene(Scene);
	}

	iter = m_mapMapObject.begin();
	iterEnd = m_mapMapObject.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->SetScene(Scene);
	}
}

void CObjectPool::GetAllMonsterNames(std::vector<std::string>& OutNames)
{
	auto iter = m_mapMonster.begin();
	auto iterEnd = m_mapMonster.end();

	std::string Name;
	for (; iter != iterEnd; ++iter)
	{
		if (Name != iter->first)
		{
			OutNames.push_back(iter->first);
			Name = iter->first;
		}
	}
}
