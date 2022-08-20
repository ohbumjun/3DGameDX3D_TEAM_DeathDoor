
#include "SceneManager.h"
#include "../Render/RenderManager.h"
#include "../Sync.h"
#include "../PathManager.h"
#include "../GameObject/SkyObject.h"
#include "../GameObject/LightObj.h"
#include "../ObjectPool.h"

DEFINITION_SINGLE(CSceneManager)

CSceneManager::CSceneManager()	:
	m_Scene(nullptr),
	m_NextScene(nullptr),
	m_StateManager(nullptr),
	m_ObjectDataSetCallback(nullptr)
{
	InitializeCriticalSection(&m_Crt);
}

CSceneManager::~CSceneManager()
{
	DeleteCriticalSection(&m_Crt);
	SAFE_DELETE(m_Scene);
	SAFE_DELETE(m_NextScene);
	//SAFE_DELETE(m_StateManager);
}

CGameObject* CSceneManager::CreateObjectByTypeID(size_t TypeID)
{
	CGameObject* Child = nullptr;

	if (TypeID == typeid(CGameObject).hash_code())
	{
		Child = new CGameObject;
	}

	else if (TypeID == typeid(CLightObj).hash_code())
	{
		Child = new CLightObj;
	}

	else if (TypeID == typeid(CSkyObject).hash_code())
	{
		Child = new CSkyObject;
	}


	return Child;
}

void CSceneManager::Start()
{
	m_Scene->Start();
}

bool CSceneManager::Init()
{
	m_Scene = new CScene;

	CRenderManager::GetInst()->SetObjectList(&m_Scene->m_ObjList);

	//m_StateManager = new CStateManager;

	//m_StateManager->Init();

	return true;
}

bool CSceneManager::Update(float DeltaTime)
{
	m_Scene->Update(DeltaTime);

	return ChangeScene();
}

bool CSceneManager::PostUpdate(float DeltaTime)
{
	m_Scene->PostUpdate(DeltaTime);

	return ChangeScene();
}

bool CSceneManager::ReloadScene()
{
	if (strlen(m_LoadedSceneFullPath) == 0)
	{
		return false;
	}

	return LoadNewSceneFullPath(m_LoadedSceneFullPath, true);
}

bool CSceneManager::LoadNewScene(const char* FileName, const std::string& PathName, bool ChangeNow)
{
	const PathInfo* Path = CPathManager::GetInst()->FindPath(PathName);

	char FullPath[MAX_PATH] = {};

	if (Path)
	{
		strcpy_s(FullPath, Path->PathMultibyte);
	}

	strcat_s(FullPath, FileName);

	return LoadNewSceneFullPath(FullPath, ChangeNow);
}

bool CSceneManager::LoadNewSceneFullPath(const char* FullPath, bool ChangeNow)
{
	CreateNextScene(ChangeNow);
	
	bool LoadResult = m_NextScene->LoadFullPath(FullPath);

	if (LoadResult)
	{
		strcpy_s(m_LoadedSceneFullPath, FullPath);
	}

	m_NextScene->m_Play = true;

	return LoadResult;
}

bool CSceneManager::ChangeScene()
{
	CSync	sync(&m_Crt);

	if (m_NextScene)
	{
		if (m_NextScene->m_Change)
		{
			std::list<CSharedPtr<CGameObject>> NoDestroyObjectCloneList;
			m_Scene->CloneAllNoDestroyObjects(NoDestroyObjectCloneList);

			std::string PrevMusicKeyName = m_Scene->GetSceneSaveGlobalData().BackGroundData.MusicKeyName;

			if (PrevMusicKeyName.empty())
			{
				PrevMusicKeyName = m_Scene->GetSceneSaveGlobalData().BackGroundData.PrevMusicKeyName;
			}

			// 이전 씬의 UI가 파괴되지 않아야 할 경우
			bool KeepUI = false;
			std::list<CSharedPtr<CWidgetWindow>> WindowList;

			if (m_IsKeepUICallBack)
			{
				KeepUI = m_IsKeepUICallBack(m_Scene, m_NextScene);

				if (KeepUI)
				{
					WindowList = m_Scene->GetViewport()->GetWindowList();
				}
			}

			SAFE_DELETE(m_Scene);
			m_Scene = m_NextScene;
			m_NextScene = nullptr;

			// 다음 Scene의 viewport에게 이전 Scene의 viewport 갖고 있던 WindowList를 전달
			m_Scene->GetViewport()->SetWindowList(WindowList);

			// 씬이 바뀌어도 파괴되지 않는 오브젝트들을 새로운 씬으로 옮김
			auto iter = NoDestroyObjectCloneList.begin();
			auto iterEnd = NoDestroyObjectCloneList.end();

			for (; iter != iterEnd; ++iter)
			{
				m_Scene->AddObject((*iter).Get());
			}

			if (KeepUI)
			{
				m_Scene->GetViewport()->SetWindowList(WindowList);
			}

			CRenderManager::GetInst()->SetObjectList(&m_Scene->m_ObjList);

			CObjectPool::GetInst()->RefreshNewScene(m_Scene);

			m_Scene->SetPrevSceneMusicKeyName(PrevMusicKeyName);
			m_Scene->Start();

			// HDR 렌더 설정, 전역 라이트 설정 등 로드
			m_Scene->UpdateSceneGlobalData();
			

			return true;
		}
	}

	return false;
}

void CSceneManager::CreateNextScene(bool AutoChange)
{
	CSync	sync(&m_Crt);

	SAFE_DELETE(m_NextScene);

	m_NextScene = new CScene;

	m_NextScene->SetAutoChange(AutoChange);
}

void CSceneManager::ChangeNextScene()
{
	CSync	sync(&m_Crt);

	m_NextScene->SetAutoChange(true);
}
