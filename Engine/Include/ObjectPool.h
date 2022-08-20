#pragma once

#include "GameInfo.h"
#include "GameObject/GameObject.h"

class CObjectPool
{
private:
	std::unordered_multimap <std::string, CSharedPtr<CGameObject>> m_mapProjectile;
	std::unordered_multimap <std::string, CSharedPtr<CGameObject>> m_mapMonster;
	std::unordered_multimap <std::string, CSharedPtr<CGameObject>> m_mapParticle;
	std::unordered_multimap <std::string, CSharedPtr<CGameObject>> m_mapMapObject;

	std::string m_DataName;

public:
	virtual void Init();

	class CGameObject* GetProjectile(const std::string& Name, class CScene* Scene, bool CallStart = true);
	class CGameObject* GetMonster(const std::string& Name, class CScene* Scene, bool CallStart = true);
	class CGameObject* GetParticle(const std::string& Name, class CScene* Scene, bool CallStart = true);
	class CGameObject* GetMapObject(const std::string& Name, class CScene* Scene, bool CallStart = true);


	void ReturnToPool(class CGameObject* Object);
	void LoadPoolData(const char* FileName);
	void CreatePoolObject(const std::string& PathName);
	void RefreshNewScene(class CScene* Scene);

	void GetAllMonsterNames(std::vector<std::string>& OutNames);

	DECLARE_SINGLE(CObjectPool)
};


