#pragma once

#include "Scene.h"
#include "../Component/State/StateManager.h"

class CSceneManager
{
private:
	CRITICAL_SECTION	m_Crt;
	CScene* m_Scene;
	CScene* m_NextScene;
	std::function<void(CScene*, size_t)>	m_CreateSceneModeCallback;
	std::function<CGameObject* (CScene*, size_t)>	m_CreateObjectCallback;
	std::function<void (CGameObject* , const std::string& )>	m_ObjectDataSetCallback;
	std::function<class CComponent* (CGameObject* Obj, size_t Type)>	m_CreateComponentCallback;
	std::function<void(class CSpriteComponent* Sprite, size_t Type)>	m_CreateAnimInstanceCallback;
	std::function<bool(class CScene* CurScene, class CScene* NextScene)> m_IsKeepUICallBack;
	CStateManager* m_StateManager;

	char m_LoadedSceneFullPath[MAX_PATH];

public:
	void SetStateManager(CStateManager* StateManager)
	{
		m_StateManager = StateManager;
	}

	CStateManager* GetStateManager()	const
	{
		return m_StateManager;
	}

	CScene* GetScene()	const
	{
		return m_Scene;
	}

	CScene* GetNextScene() const
	{
		return m_NextScene;
	}

	void CallCreateSceneMode(CScene* Scene, size_t Type)
	{
		if (m_CreateSceneModeCallback)
			m_CreateSceneModeCallback(Scene, Type);
	}

	void CallObjectDataSet(CGameObject* Obj, const std::string& Name)
	{
		if (m_ObjectDataSetCallback)
			m_ObjectDataSetCallback(Obj, Name);
	}

	CGameObject* CallCreateObject(CScene* Scene, size_t Type)
	{
		if (m_CreateObjectCallback)
			return m_CreateObjectCallback(Scene, Type);

		return nullptr;
	}

	class CComponent* CallCreateComponent(CGameObject* Obj, size_t Type)
	{
		if (m_CreateComponentCallback)
			return m_CreateComponentCallback(Obj, Type);

		return nullptr;
	}

	void CallCreateAnimInstance(class CSpriteComponent* Sprite, size_t Type)
	{
		if (m_CreateAnimInstanceCallback)
			m_CreateAnimInstanceCallback(Sprite, Type);
	}

	CGameObject* CreateObjectByTypeID(size_t TypeID);

public:
	void Start();
	bool Init();
	bool Update(float DeltaTime);
	bool PostUpdate(float DeltaTime);

public:
	bool ReloadScene();

public:
	bool LoadNewScene(const char* FileName, const std::string& PathName = SCENE_PATH, bool ChangeNow = true);
	bool LoadNewSceneFullPath(const char* FullPath, bool ChangeNow = false);

private:
	bool ChangeScene();

public:
	void CreateNextScene(bool AutoChange = true);
	void ChangeNextScene();

	template <typename T>
	bool CreateSceneMode(bool Current = true)
	{
		if (Current)
			return m_Scene->CreateSceneMode<T>();

		return m_NextScene->CreateSceneMode<T>();
	}

	template <typename T>
	T* CreateSceneModeEmpty(bool Current = true)
	{
		if (Current)
			return m_Scene->CreateSceneModeEmpty<T>();

		return m_NextScene->CreateSceneModeEmpty<T>();
	}

	template <typename T>
	void SetCreateSceneModeFunction(T* Obj, void(T::* Func)(CScene*, size_t))
	{
		m_CreateSceneModeCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template <typename T>
	void SetCreateObjectFunction(T* Obj, CGameObject* (T::* Func)(CScene*, size_t))
	{
		m_CreateObjectCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template <typename T>
	void SetObjectDataSetFunction(T* Obj, void (T::* Func)(CGameObject*, const std::string&))
	{
		m_ObjectDataSetCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template <typename T>
	void SetCreateComponentFunction(T* Obj, class CComponent* (T::* Func)(CGameObject*, size_t))
	{
		m_CreateComponentCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template <typename T>
	void SetCreateAnimInstanceFunction(T* Obj, void (T::* Func)(CSpriteComponent*, size_t))
	{
		m_CreateAnimInstanceCallback = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template <typename T>
	void SetIsKeepUIFunction(T* Obj, bool(T::* Func)(CScene*, CScene*))
	{
		m_IsKeepUICallBack = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
	}

	DECLARE_SINGLE(CSceneManager)
};

