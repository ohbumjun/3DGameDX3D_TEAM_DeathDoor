#include "DDSceneMode.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Component/Component.h"
#include "Component/ColliderBox3D.h"
#include "../Thread/DDSceneLoadingThread.h"
#include "Render/RenderManager.h"
#include "../Component/GameStateComponent.h"
#include "Engine.h"
#include "Resource/Sound/Sound.h"
#include "../UI/UIManager.h"

CDDSceneMode::CDDSceneMode()	:
	m_ExitPointCollider(nullptr),
	m_NextSceneLoadComplete(false),
	m_NextSceneLoadingThread(nullptr),
	m_InitialScene(false)
{
	SetTypeID<CDDSceneMode>();
}

CDDSceneMode::~CDDSceneMode()
{
	SAFE_DELETE(m_NextSceneLoadingThread);
}

bool CDDSceneMode::Init()
{
	CSceneMode::Init();

	return true;
}

void CDDSceneMode::Start()
{
	CSceneMode::Start();

	m_ExitPointObj = m_Scene->FindObject(m_ExitPointObjName);

	if (m_ExitPointObj)
	{
		m_ExitPointCollider = m_ExitPointObj->FindComponentFromType<CColliderBox3D>();
	}

	// Initial Scene이 아닐 경우, 이전 씬에서 NoDestroy 상태로 플레이어가 넘어오기 때문에 찾아서 넣어준다.
	if (!m_InitialScene)
	{
		m_PlayerObject = m_Scene->FindObjectFromType(Object_Type::Player);
	}

	if (m_PlayerObject)
	{
		if (m_InitialScene)
		{
			m_PlayerObject->SetNoDestroyOnSceneChange(true);
		}

		m_PlayerObject->SetWorldPos(m_EntryPoint);
	}

	// 씬 전환 콜라이더 콜백
	if (m_ExitPointCollider)
	{
		m_ExitPointCollider->AddCollisionCallback(Collision_State::Begin, this, &CDDSceneMode::OnCollideExitCollider);
	}

	// 바로 다음 씬을 미리 로딩해두기 위한 스레드
	if (!m_NextSceneLoadingThread)
	{
		m_NextSceneLoadingThread = new CDDSceneLoadingThread;

		if (!m_NextSceneFileName.empty())
		{
			m_NextSceneLoadingThread->Init();
			m_NextSceneLoadingThread->Load(m_NextSceneFileName, this);
		}
	}

	// 몬스터 카운트 체크
	std::vector<CGameObject*> vecMonsterObj;
	m_Scene->GetObjectsByType(Object_Type::Monster, vecMonsterObj);

	size_t Size = vecMonsterObj.size();
	for (size_t i = 0; i < Size; ++i)
	{
		if (vecMonsterObj[i]->IsInPool())
		{
			continue;
		}

		++m_MonsterCount;
	}

	// Fade In 
	CRenderManager::GetInst()->SetFadeStartCallBack(this, &CDDSceneMode::OnFadeInStart);
	CRenderManager::GetInst()->StartFadeEffect(FadeEffecType::FADE_IN, false);

	// Play
	if (!CEngine::GetInst()->GetEditMode())
	{
		m_Scene->Play();
	}

	std::vector<CGameObject*> vecObject;
	CSceneManager::GetInst()->GetScene()->GetAllObjectsPointer(vecObject);

	size_t Count = vecObject.size();

	for (size_t i = 0; i < Count; ++i)
	{
		CGameStateComponent* Comp = vecObject[i]->FindObjectComponentFromType<CGameStateComponent>();

		if (Comp)
		{
			Comp->SetTreeUpdate(true);
		}
	}


	if (m_InitialScene)
	{
		CUIManager::GetInst()->SetViewport(m_Scene->GetViewport());
		CUIManager::GetInst()->CreateDeathDoorUI();
	}
}

bool CDDSceneMode::Save(FILE* File)
{
	CRef::Save(File);

	fwrite(&m_InitialScene, sizeof(bool), 1, File);
	fwrite(&m_EntryPoint, sizeof(Vector3), 1, File);
	int Length = (int)m_NextSceneFileName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_NextSceneFileName.c_str(), sizeof(char), Length, File);

	Length = (int)m_ExitPointObjName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(m_ExitPointObjName.c_str(), sizeof(char), Length, File);

	return true;
}

bool CDDSceneMode::Load(FILE* File)
{
	CRef::Load(File);

	fread(&m_InitialScene, sizeof(bool), 1, File);
	fread(&m_EntryPoint, sizeof(Vector3), 1, File);

	int Length = 0;
	char NextSceneFileName[MAX_PATH] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(NextSceneFileName, sizeof(char), Length, File);

	char ExitObjName[MAX_PATH] = {};
	fread(&Length, sizeof(int), 1, File);
	fread(ExitObjName, sizeof(char), Length, File);

	m_NextSceneFileName = NextSceneFileName;
	m_ExitPointObjName = ExitObjName;

	return true;
}

void CDDSceneMode::OnClearDungeon()
{
	m_ExitPointObj->Enable(true);
	m_ExitPointCollider->Enable(true);
}

bool CDDSceneMode::SetExitPointObj(CGameObject* Obj)
{
	CColliderBox3D* Collider = Obj->FindComponentFromType<CColliderBox3D>();

	if (!Collider)
	{
		return false;
	}

	m_ExitPointObjName = Obj->GetName();
	m_ExitPointObj = Obj;
	m_ExitPointCollider = Collider;

	return true;
}

void CDDSceneMode::OnDieMonster()
{
	--m_MonsterCount;

	if (m_MonsterCount == 0)
	{
		OnClearDungeon();
	}
}

void CDDSceneMode::OnFadeOutEnd()
{
	// Scene Change
	CSceneManager::GetInst()->ChangeNextScene();

	// 플레이어 조작 불가능 상태로
	if (m_PlayerObject)
	{
		CGameStateComponent* PlayerState = m_PlayerObject->FindObjectComponentFromType<CGameStateComponent>();
		PlayerState->SetTreeUpdate(false);
	}
}

void CDDSceneMode::OnFadeInStart()
{
	// 플레이어 조작 가능 상태로
	if (m_PlayerObject)
	{
		CGameStateComponent* PlayerState = m_PlayerObject->FindObjectComponentFromType<CGameStateComponent>();
		PlayerState->SetTreeUpdate(true);
	}
}

void CDDSceneMode::OnCollideExitCollider(const CollisionResult& Result)
{
	bool NextSceneExist = CSceneManager::GetInst()->GetNextScene() ? true : false;

	if (!NextSceneExist)
	{
		return;
	}

	// 로드 되기 전까지는 대기
	while (!m_NextSceneLoadComplete)
	{
		continue;
	}

	// Fade Out
	CRenderManager::GetInst()->SetFadeEndCallBack(this, &CDDSceneMode::OnFadeOutEnd);
	CRenderManager::GetInst()->StartFadeEffect(FadeEffecType::FADE_OUT, true);
}
