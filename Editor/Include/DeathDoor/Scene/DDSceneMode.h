#pragma once

#include "Scene\SceneMode.h"

class CDDSceneMode :
    public CSceneMode
{
	friend class CScene;

protected:
	CDDSceneMode();
	virtual ~CDDSceneMode();

public:
	virtual bool Init() override;
	virtual void Start() override;

public:
	virtual bool Save(FILE* File) override;
	virtual bool Load(FILE* File) override;

public:
	virtual void OnDieMonster();
	virtual void OnClearDungeon();

public:
	void SetEntryPoint(const Vector3& EntryPoint)
	{
		m_EntryPoint = EntryPoint;
	}

	virtual bool SetExitPointObj(class CGameObject* Obj);

	void SetNextSceneFileName(const std::string& FileName)
	{
		m_NextSceneFileName = FileName;
	}

	void SetInitialScene(bool IsInitial)
	{
		m_InitialScene = IsInitial;
	}

	void OnNextSceneLoadComplete()
	{
		m_NextSceneLoadComplete = true;
	}

	virtual void OnFadeOutEnd();
	virtual void OnFadeInStart();

public:
	bool IsInitialScene() const
	{
		return m_InitialScene;
	}

	const Vector3& GetEntryPoint() const
	{
		return m_EntryPoint;
	}

	class CColliderBox3D* GetExitPointCollider() const
	{
		return m_ExitPointCollider;
	}

	const std::string& GetNextSceneFileName() const
	{
		return m_NextSceneFileName;
	}

	class CGameObject* GetExitPointObject() const
	{
		return m_ExitPointObj;
	}

	const std::string& GetExitPointObjectName() const
	{
		return m_ExitPointObjName;
	}

protected:
	virtual void OnCollideExitCollider(const CollisionResult& Result);

protected:
	bool m_NextSceneLoadComplete;
	bool m_InitialScene;						// 플레이어가 존재하는 최초 씬인지
	int m_MonsterCount;
	Vector3 m_EntryPoint;
	std::string m_ExitPointObjName;
	class CGameObject* m_ExitPointObj;
	class CColliderBox3D* m_ExitPointCollider;
	std::string	m_NextSceneFileName;
	class CDDSceneLoadingThread* m_NextSceneLoadingThread;
};

