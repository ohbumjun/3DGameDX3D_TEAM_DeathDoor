#include "DDSceneLoadingThread.h"
#include "../Scene/DDSceneMode.h"
#include "Scene/SceneManager.h"

CDDSceneLoadingThread::CDDSceneLoadingThread()
{
}

CDDSceneLoadingThread::~CDDSceneLoadingThread()
{
}

void CDDSceneLoadingThread::Run()
{
	bool Success = CSceneManager::GetInst()->LoadNewScene(m_NextSceneFileName.c_str(), SCENE_PATH, false);

	if (!Success)
	{
		assert(false);
	}

	if (m_Owner)
	{
		m_Owner->OnNextSceneLoadComplete();
	}

	if (m_EndFunction)
	{
		m_EndFunction();
	}
}

void CDDSceneLoadingThread::Load(const std::string& SceneFileName, class CDDSceneMode* Owner)
{
	m_Owner = Owner;
	m_NextSceneFileName = SceneFileName;

	Run();
}
