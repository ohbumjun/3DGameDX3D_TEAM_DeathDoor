
#include "Scene.h"
#include "../PathManager.h"
#include "SceneManager.h"
#include "../Component/CameraComponent.h"
#include "../GameObject/SkyObject.h"
#include "../Input.h"
#include "../Collision/Collision.h"
#include "Navigation3DManager.h"
#include "../Render/RenderManager.h"
#include "../EngineUtil.h"
#include "../ObjectPool.h"
#include "../GameObject/SkyObject.h"
#include "../GameObject/GameObject.h"

CScene::CScene()
{
	m_Mode = new CSceneMode;
	m_Resource = new CSceneResource;
	m_Collision = new CSceneCollision;
	m_CameraManager = new CCameraManager;
	m_Viewport = new CViewport;
	m_NavManager = new CNavigationManager;
	m_Nav3DManager = new CNavigation3DManager;
	m_LightManager = new CLightManager;

	m_Mode->m_Scene = this;
	m_Resource->m_Scene = this;
	m_Collision->m_Scene = this;
	m_CameraManager->m_Scene = this;
	m_Viewport->m_Scene = this;
	m_NavManager->m_Scene = this;
	m_Nav3DManager->m_Scene = this;
	m_LightManager->m_Scene = this;

	m_Start = false;
	m_Play = true;

	m_Collision->Init();
	m_CameraManager->Init();
	m_Viewport->Init();
	m_NavManager->Init();
	m_Nav3DManager->Init();
	m_LightManager->Init();

	m_Change = true;

	m_SkyObject = new CSkyObject;

	m_SkyObject->SetName("Sky");
	m_SkyObject->SetScene(this);

	m_SkyObject->Init();

	m_RestoreCamDirFix = false;

	m_AccTime = 0.f;
}

CScene::~CScene()
{
	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->SetScene(nullptr);
	}

	m_ObjList.clear();
	SAFE_DELETE(m_NavManager)	
	SAFE_DELETE(m_Nav3DManager);
	SAFE_DELETE(m_Viewport);
	SAFE_DELETE(m_CameraManager);
	SAFE_DELETE(m_Collision);
	SAFE_DELETE(m_Resource);
	m_LightManager->Destroy();
	SAFE_DELETE(m_LightManager);
}

void CScene::Play()
{
	m_Play = true;

	if (!m_Mode->IsStart())
	{
		m_Mode->Start();
	}
}

CNavigation3DManager* CScene::GetNavigation3DManager() const
{
	return m_Nav3DManager;
}

int CScene::GetObjectCountByType(Object_Type Type)
{
	int Count = 0;

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetObjectType() == Type)
		{
			++Count;
		}
	}

	return Count;
}

void CScene::GetObjectsByType(Object_Type Type, std::vector<class CGameObject*>& outVecObj)
{
	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetObjectType() == Type)
		{
			outVecObj.push_back(*iter);
		}
	}
}

void CScene::Start()
{
	m_Mode->Start();

	std::string CurBGM = m_SceneGlobalData.BackGroundData.MusicKeyName;

	if (!CurBGM.empty())
	{
		std::string PrevBGM = m_SceneGlobalData.BackGroundData.PrevMusicKeyName;
		if (!PrevBGM.empty() && PrevBGM != CurBGM)
		{
			CResourceManager::GetInst()->SoundStop(PrevBGM);
		}

		CResourceManager::GetInst()->SoundPlay(CurBGM);
	}

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		// Pool에서 꺼낸 오브젝트들은 GetXXX 함수 호출해서 Start를 호출할 것이다
		if ((*iter)->IsInPool())
		{
			continue;
		}

		// 플레이어는 Start를 단 한번만 호출
		if ((*iter)->GetObjectType() == Object_Type::Player && (*iter)->IsStartCalled())
		{
			continue;
		}

		(*iter)->Start();
	}

	m_Start = true;

	m_SkyObject->Start();

	m_CameraManager->Start();
	m_Collision->Start();
	m_Viewport->Start();
	m_LightManager->Start();

	if (m_Mode->GetPlayerObject())
	{
		CCameraComponent* Camera = m_Mode->GetPlayerObject()->FindComponentFromType<CCameraComponent>();

		if (Camera)
		{
			m_CameraManager->SetCurrentCamera(Camera);
		}
	}

	m_NavManager->Start();
	m_Nav3DManager->Start();
}

void CScene::Update(float DeltaTime)
{
	if (!m_Play)
	{
		DeltaTime = 0.f;
	}

	m_Mode->Update(DeltaTime);

	//CSceneCollision::Collision에서 CheckColliderSection3D 호출해주고 있음
	//m_Collision->CheckColliderSection3D();

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			if ((*iter)->IsInPool())
				CObjectPool::GetInst()->ReturnToPool(*iter);

			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(DeltaTime);
		++iter;
	}

	m_CameraManager->Update(DeltaTime);

	if (m_Play)
	{
		m_Viewport->Update(DeltaTime);
		m_NavManager->Update(DeltaTime);
		m_Nav3DManager->Update(DeltaTime);
	}

	m_LightManager->Update(DeltaTime);
}

void CScene::PostUpdate(float DeltaTime)
{
	if (!m_Play)
	{
		DeltaTime = 0.f;
	}

	m_Mode->PostUpdate(DeltaTime);

	m_SkyObject->PostUpdate(DeltaTime);


	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			if((*iter)->IsInPool())
				CObjectPool::GetInst()->ReturnToPool(*iter);

			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->PostUpdate(DeltaTime);

		++iter;
	}

	m_CameraManager->PostUpdate(DeltaTime);

	if (m_Play)
	{
		m_Viewport->PostUpdate(DeltaTime);
	}

	m_RenderComponentList.clear();

	iter = m_ObjList.begin();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->AddCollision();

		const std::list<CSceneComponent*>& List = (*iter)->GetSceneComponents();

		auto iter1 = List.begin();
		auto iter1End = List.end();

		for (; iter1 != iter1End; ++iter1)
		{
			if ((*iter1)->GetRender() && !(*iter1)->GetCulling())
			{
				m_RenderComponentList.push_back(*iter1);
			}
		}
	}

	// 출력되는 물체를 정렬한다.
	if (m_RenderComponentList.size() >= 2)
	{
		m_RenderComponentList.sort(SortRenderList);
	}

	if (m_Play)
	{
		// 포함된 충돌체들을 이용해서 충돌처리를 진행한다.
		m_Collision->Collision(DeltaTime);
	}

	else
	{
		m_Collision->ClearAll();
	}

	iter = m_ObjList.begin();
	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->m_PrevFramePos = (*iter)->GetWorldPos();
	}
}

bool CScene::Save(const char* FileName, const std::string& PathName)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	char	FullPath[MAX_PATH] = {};

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	return SaveFullPath(FullPath);
}

bool CScene::SaveFullPath(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
		return false;

	size_t	SceneModeType = m_Mode->GetTypeID();

	fwrite(&SceneModeType, sizeof(size_t), 1, File);

	m_Mode->Save(File);

	size_t	ObjCount = m_ObjList.size();
	int ExcludeObjectCount = GetSaveExcludeObjectCount();
	ObjCount -= (size_t)ExcludeObjectCount;

	auto	iter = m_ObjList.begin();
	auto	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetParentObject())
			--ObjCount;
	}

	fwrite(&ObjCount, sizeof(size_t), 1, File);

	bool Success = false;

	iter = m_ObjList.begin();
	iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->IsExcludeFromSceneSave())
		{
			continue;
		}

		if ((*iter)->GetParentObject())
		{
			continue;
		}

		size_t	ObjType = (*iter)->GetTypeID();

		fwrite(&ObjType, sizeof(size_t), 1, File);

		//Success = (*iter)->Save(File);
		Success = (*iter)->SaveHierarchy(File);

		if (!Success)
		{
			fclose(File);
			return false;
		}
	}

	// Scene Global Data ( HDR Value, 전역 라이트 설정 등 )을 CSV로 저장
	std::string FileName = CEngineUtil::FilterFileName(FullPath);
	Success = SaveSceneGlobalDataCSV(FileName.c_str());

	if (!Success)
	{
		MessageBox(nullptr, TEXT("씬 글로벌 데이터 엑셀 파일 없음 - 기본 설정으로 로드함"), TEXT("Warning"), MB_OK);
	}

	fclose(File);
	return true;
}

bool CScene::Load(const char* FileName, const std::string& PathName)
{
	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	char	FullPath[MAX_PATH] = {};

	if (Info)
		strcpy_s(FullPath, Info->PathMultibyte);

	strcat_s(FullPath, FileName);

	return LoadFullPath(FullPath);
}

bool CScene::LoadFullPath(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
		return false;

	// LightManager에서 생성된 Global Light를 삭제하지 않기 위해
	// m_ObjList.clear();

	size_t	SceneModeType = 0;

	fread(&SceneModeType, sizeof(size_t), 1, File);

	// SceneMode 생성
	CSceneManager::GetInst()->CallCreateSceneMode(this, SceneModeType);
	
	m_Mode->Load(File);

	size_t	ObjCount = m_ObjList.size();

	fread(&ObjCount, sizeof(size_t), 1, File);

	bool Success = false;

	for (size_t i = 0; i < ObjCount; ++i)
	{
		size_t	ObjType = 0;
		fread(&ObjType, sizeof(size_t), 1, File);

		CGameObject* Obj = CSceneManager::GetInst()->CallCreateObject(this, ObjType);

		//Success = Obj->Load(File);
		// 여기서 NextScene 포인터를 Obj->LoadHierarchy안으로 넘겨줘야 한다
		if (!Obj)
			continue;

		Success = Obj->LoadHierarchy(File, this);

		CSceneManager::GetInst()->CallObjectDataSet(Obj, Obj->GetName());

		if (!Success)
		{
			fclose(File);
			return false;
		}

		Obj->SetScene(this);
	}

	std::string FileName = CEngineUtil::FilterFileName(FullPath);
	
	// 렌더 설정, 전역 조명 설정 등 로드
	// 실패 시 기본 설정으로 로드함
	Success = LoadSceneGlobalDataCSV(FileName.c_str());

	fclose(File);

	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd;)
	{
		if ((*iter)->GetName() == "BossBettySnowAttack" ||
			(*iter)->GetName() == "BettyAttackAfterEffect")
		{
			iter = m_ObjList.erase(iter);
			iterEnd = m_ObjList.end();
			continue;
		}
		++iter;
	}

	return true;
}

bool CScene::SaveSceneGlobalDataCSV(const char* FileName)
{
	// .scn을 제외한 파일 이름 생성
	std::string PureFileName;
	CEngineUtil::GetFileNameOnly(FileName, PureFileName);

	// AAA.scn -> AAA_GlobalData
	std::string CSVKeyName = PureFileName + "_GlobalData";
	bool Success = CResourceManager::GetInst()->CreateCSV(CSVKeyName);

	if (!Success)
	{
		return false;
	}

	CRenderManager* RMng = CRenderManager::GetInst();
	CLightComponent* GL = m_LightManager->GetGlobalLightComponent();

	CExcelData* Data = CResourceManager::GetInst()->FindCSV(CSVKeyName);

	Data->AddRow("Data");

	Data->AddLabel("AdaptationTime");
	float Val = RMng->GetAdaptationTime();
	Data->SetData("Data", "AdaptationTime", Val);

	Data->AddLabel("MiddleGray");
	Val = RMng->GetMiddleGray();
	Data->SetData("Data", "MiddleGray", Val);

	Data->AddLabel("LumWhite");
	Val = RMng->GetLumWhite();
	Data->SetData("Data", "LumWhite", Val);

	Data->AddLabel("BloomThreshold");
	Val = RMng->GetBloomThreshold();
	Data->SetData("Data", "BloomThreshold", Val);

	Data->AddLabel("BloomScale");
	Val = RMng->GetBloomScale();
	Data->SetData("Data", "BloomScale", Val);

	Data->AddLabel("DOFMax");
	Val = RMng->GetDOFMax();
	Data->SetData("Data", "DOFMax", Val);

	Data->AddLabel("DOFMin");
	Val = RMng->GetDOFMin();
	Data->SetData("Data", "DOFMin", Val);

	Data->AddLabel("FogType");
	Val = (float)RMng->GetFogType();
	Data->SetData("Data", "FogType", Val);

	Data->AddLabel("FogColorR");
	Vector3 FogCol = RMng->GetFogColor();
	Data->SetData("Data", "FogColorR", FogCol.x);

	Data->AddLabel("FogColorG");
	Data->SetData("Data", "FogColorG", FogCol.y);

	Data->AddLabel("FogColorB");
	Data->SetData("Data", "FogColorB", FogCol.z);

	Data->AddLabel("FogStart");
	Val = RMng->GetFogStart();
	Data->SetData("Data", "FogStart", Val);

	Data->AddLabel("FogEnd");
	Val = RMng->GetFogEnd();
	Data->SetData("Data", "FogEnd", Val);

	Data->AddLabel("FogDensity");
	Val = RMng->GetFogDensity();
	Data->SetData("Data", "FogDensity", Val);

	Data->AddLabel("GlobalLightRotX");
	Vector3 GLRot = GL->GetWorldRot();
	Data->SetData("Data", "GlobalLightRotX", GLRot.x);

	Data->AddLabel("GlobalLightRotY");
	Data->SetData("Data", "GlobalLightRotY", GLRot.y);

	Data->AddLabel("GlobalLightRotZ");
	Data->SetData("Data", "GlobalLightRotZ", GLRot.z);

	Data->AddLabel("GlobalLightColorR");
	Vector4 GLCol = GL->GetLightColor();
	Data->SetData("Data", "GlobalLightColorR", GLCol.x);

	Data->AddLabel("GlobalLightColorG");
	Data->SetData("Data", "GlobalLightColorG", GLCol.y);

	Data->AddLabel("GlobalLightColorB");
	Data->SetData("Data", "GlobalLightColorB", GLCol.z);

	Data->AddLabel("AmbientIntensity");
	Val = m_LightManager->GetGlobalLightAmbiendIntensity();
	Data->SetData("Data", "AmbientIntensity", Val);

	bool RenderSkyBox = CRenderManager::GetInst()->IsRenderSkyBox();
	Data->AddLabel("RenderSkyBox");
	Data->SetData("Data", "RenderSkyBox", RenderSkyBox);

	Vector4 ClearColor = CEngine::GetInst()->GetClearColor();
	Data->AddLabel("ClearColorR");
	Data->SetData("Data", "ClearColorR", ClearColor.x);
	Data->AddLabel("ClearColorG");
	Data->SetData("Data", "ClearColorG", ClearColor.y);
	Data->AddLabel("ClearColorB");
	Data->SetData("Data", "ClearColorB", ClearColor.z);

	Data->AddLabel("SkyBoxTexturePath");
	std::string SkyBoxTexPath = m_SceneGlobalData.BackGroundData.SkyBoxFileName;
	Data->SetData("Data", "SkyBoxTexturePath", SkyBoxTexPath);

	// AAA.scn 로 scn 저장하면 -> AAA_GlobalData.csv 파일명 csv 파일 생성
	// Excel/SceneGlobalData/ 경로에 저장한다
	char CSVFileName[MAX_PATH] = {};
	strcpy_s(CSVFileName, "//SceneGlobalData//");
	strcat_s(CSVFileName, PureFileName.c_str());
	strcat_s(CSVFileName, "_GlobalData.csv");

	Success = Data->SaveCSV(CSVFileName);

	if (!Success)
	{
		return false;
	}

	// 메모리 해제
	CResourceManager::GetInst()->DeleteCSV(CSVKeyName);

	return Success;
}

bool CScene::LoadSceneGlobalDataCSV(const char* FileName)
{
	// .scn을 제외한 파일 이름 생성
	std::string PureFileName;
	CEngineUtil::GetFileNameOnly(FileName, PureFileName);

	char CSVFileName[MAX_PATH] = {};
	strcpy_s(CSVFileName, "//SceneGlobalData//");
	strcat_s(CSVFileName, PureFileName.c_str());
	strcat_s(CSVFileName, "_GlobalData.csv");

	std::string outCSVKey;
	bool Success = CResourceManager::GetInst()->LoadCSV(outCSVKey, CSVFileName);

	if (!Success)
	{
		return false;
	}

	CExcelData* Data = CResourceManager::GetInst()->FindCSV(outCSVKey);

	// Load Data
	m_SceneGlobalData.HDRData.AdaptationTime = Data->FindDataFloat("Data", "AdaptationTime");
	m_SceneGlobalData.HDRData.MiddleGray = Data->FindDataFloat("Data", "MiddleGray");
	m_SceneGlobalData.HDRData.LumWhite = Data->FindDataFloat("Data", "LumWhite");
	m_SceneGlobalData.HDRData.BloomTreshold = Data->FindDataFloat("Data", "BloomThreshold");
	m_SceneGlobalData.HDRData.BloomScale = Data->FindDataFloat("Data", "BloomScale");
	m_SceneGlobalData.HDRData.DOFMax = Data->FindDataFloat("Data", "DOFMax");
	m_SceneGlobalData.HDRData.DOFMin = Data->FindDataFloat("Data", "DOFMin");
	m_SceneGlobalData.HDRData.FogType = (Fog_Type)Data->FindDataInt("Data", "FogType");
	m_SceneGlobalData.HDRData.FogColor.x = Data->FindDataFloat("Data", "FogColorR");
	m_SceneGlobalData.HDRData.FogColor.y = Data->FindDataFloat("Data", "FogColorG");
	m_SceneGlobalData.HDRData.FogColor.z = Data->FindDataFloat("Data", "FogColorB");
	m_SceneGlobalData.HDRData.FogStart = Data->FindDataFloat("Data", "FogStart");
	m_SceneGlobalData.HDRData.FogEnd = Data->FindDataFloat("Data", "FogEnd");
	m_SceneGlobalData.HDRData.FogDensity = Data->FindDataFloat("Data", "FogDensity");
	m_SceneGlobalData.GLightData.Rot.x = Data->FindDataFloat("Data", "GlobalLightRotX");
	m_SceneGlobalData.GLightData.Rot.y = Data->FindDataFloat("Data", "GlobalLightRotY");
	m_SceneGlobalData.GLightData.Rot.z = Data->FindDataFloat("Data", "GlobalLightRotZ");
	m_SceneGlobalData.GLightData.Color.x = Data->FindDataFloat("Data", "GlobalLightColorR");
	m_SceneGlobalData.GLightData.Color.y = Data->FindDataFloat("Data", "GlobalLightColorG");
	m_SceneGlobalData.GLightData.Color.z = Data->FindDataFloat("Data", "GlobalLightColorB");
	m_SceneGlobalData.GLightData.AmbientIntensity = Data->FindDataFloat("Data", "AmbientIntensity");
	m_SceneGlobalData.BackGroundData.RenderSkyBox = Data->FindDataBool("Data", "RenderSkyBox");
	m_SceneGlobalData.BackGroundData.ClearColor.x = Data->FindDataFloat("Data", "ClearColorR");
	m_SceneGlobalData.BackGroundData.ClearColor.y = Data->FindDataFloat("Data", "ClearColorG");
	m_SceneGlobalData.BackGroundData.ClearColor.z = Data->FindDataFloat("Data", "ClearColorB");
	m_SceneGlobalData.BackGroundData.ClearColor.w = 1.f;
	m_SceneGlobalData.BackGroundData.SkyBoxFileName = Data->FindData("Data", "SkyBoxTexturePath");
	m_SceneGlobalData.BackGroundData.MusicKeyName = Data->FindData("Data", "MusicKeyName");
	
	// 메모리 해제
	CResourceManager::GetInst()->DeleteCSV(outCSVKey);

	return true;
}

bool CScene::Picking(CGameObject*& Result)
{
	CCameraComponent* Camera = m_CameraManager->GetCurrentCamera();

	Ray	RayWorld = CInput::GetInst()->GetRay(Camera->GetViewMatrix());

	auto	iter = m_RenderComponentList.begin();
	auto	iterEnd = m_RenderComponentList.end();

	Vector3 HitPoint;

	for (; iter != iterEnd; ++iter)
	{
		SphereInfo Info = (*iter)->GetSphereInfo();

		if (CCollision::CollisionRayToSphere(HitPoint, RayWorld, Info))
		{
			Result = (*iter)->GetGameObject();
			return true;
		}
	}

	Result = nullptr;

	return false;
}

bool CScene::CheckSameName(const std::string& Name)
{
	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetName() == Name)
			return true;
	}

	return false;
}

bool CScene::IsExistSameName(const std::string& Name)
{
	CGameObject* First = FindObject(Name);

	if (!First)
	{
		return false;
	}

	std::string FirstName = First->GetName();

	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (First != (*iter) && FirstName == (*iter)->GetName())
		{
			return true;
		}
	}
	return false;
}

void CScene::UpdateSceneGlobalData()
{
	CRenderManager* RenderMng = CRenderManager::GetInst();

	RenderMng->SetAdaptationTime(m_SceneGlobalData.HDRData.AdaptationTime);
	RenderMng->SetMiddleGray(m_SceneGlobalData.HDRData.MiddleGray);
	RenderMng->SetLumWhite(m_SceneGlobalData.HDRData.LumWhite);
	RenderMng->SetBloomThreshold(m_SceneGlobalData.HDRData.BloomTreshold);
	RenderMng->SetBloomScale(m_SceneGlobalData.HDRData.BloomScale);
	RenderMng->SetDOFMax(m_SceneGlobalData.HDRData.DOFMax);
	RenderMng->SetDOFMin(m_SceneGlobalData.HDRData.DOFMin);
	RenderMng->SetFogType(m_SceneGlobalData.HDRData.FogType);
	RenderMng->SetFogColor(m_SceneGlobalData.HDRData.FogColor);
	RenderMng->SetFogStart(m_SceneGlobalData.HDRData.FogStart);
	RenderMng->SetFogEnd(m_SceneGlobalData.HDRData.FogEnd);
	RenderMng->SetFogDensity(m_SceneGlobalData.HDRData.FogDensity);
	RenderMng->SetRenderSkyBox(m_SceneGlobalData.BackGroundData.RenderSkyBox);

	Vector4 ClearColor = m_SceneGlobalData.BackGroundData.ClearColor;
	CEngine::GetInst()->SetClearColor(ClearColor);

	CLightComponent* GLight = m_LightManager->GetGlobalLightComponent();
	GLight->SetWorldRotation(m_SceneGlobalData.GLightData.Rot);

	Vector4 Col = Vector4(m_SceneGlobalData.GLightData.Color.x,
		m_SceneGlobalData.GLightData.Color.y, m_SceneGlobalData.GLightData.Color.z, 1.f);
	GLight->SetColor(Col);
	m_LightManager->SetGlogbalLightAmbientIntensity(m_SceneGlobalData.GLightData.AmbientIntensity);

	std::string SkyBoxFileName = m_SceneGlobalData.BackGroundData.SkyBoxFileName;
	if (!SkyBoxFileName.empty() && SkyBoxFileName != "NONE")
	{
		dynamic_cast<CSkyObject*>(m_SkyObject.Get())->SetSkyTexture(m_SceneGlobalData.BackGroundData.SkyBoxFileName.c_str());
	}
}

void CScene::GetAllObjectsPointer(std::vector<CGameObject*>& vecOutObj)
{
	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		vecOutObj.push_back(*iter);
	}
}

void CScene::GetAllIncludeSaveObjectsPointer(std::vector<CGameObject*>& vecOutObj)
{
	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->IsExcludeFromSceneSave())
			continue;

		vecOutObj.push_back(*iter);
	}
}

void CScene::CloneAllNoDestroyObjects(std::list<CSharedPtr<CGameObject>>& OutList)
{
	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->IsNoDestroyFromSceneChange())
		{
			OutList.push_back((*iter));
		}
	}
}

void CScene::AddObject(CGameObject* Object)
{
	Object->SetScene(this);
	m_ObjList.push_back(Object);
}

bool CScene::CameraMove(const Vector3& Direction, const Vector3& DestPos, float Speed, float DeltaTime)
{
	CCameraComponent* CurrentCamera = m_CameraManager->GetCurrentCamera();

	CurrentCamera->AddWorldPos(Direction.x * Speed * DeltaTime, 0.f, Direction.z * Speed * DeltaTime);

	Vector3 CurrentCameraPos = CurrentCamera->GetWorldPos();
	float Dist = Vector3(CurrentCameraPos.x, 0.f, CurrentCameraPos.z).Distance(Vector3(DestPos.x, 0.f, DestPos.z));

	if (Dist < 2.f)
		return true;

	return false;
}

bool CScene::CameraMove(double Time, const Vector3& DestPos, float DeltaTime)
{
	float TimeRatio = m_AccTime / Time;

	if (TimeRatio > 1.f)
		TimeRatio = 1.f;

	Vector3 LerpPos = m_OriginCamPos * (1 - TimeRatio) + DestPos * TimeRatio;

	CCameraComponent* CurrentCamera = m_CameraManager->GetCurrentCamera();

	CurrentCamera->SetWorldPos(LerpPos);

	m_AccTime += DeltaTime;

	if (TimeRatio == 1.f)
	{
		m_AccTime = 0.f;
		return true;
	}

	return false;
}

bool CScene::RestoreCamera(float Speed, float DeltaTime)
{
	CCameraComponent* CurrentCamera = m_CameraManager->GetCurrentCamera();

	Vector3 CurrentCamPos = CurrentCamera->GetWorldPos();

	if (!m_RestoreCamDirFix)
	{
		m_RestoreCamDir = m_OriginCamPos - CurrentCamPos;
		m_RestoreCamDir.y = 0.f;
		m_RestoreCamDir.Normalize();
		m_RestoreCamDirFix = true;
	}

	if (Vector3(CurrentCamPos.x, 0.f, CurrentCamPos.z).Distance(Vector3(m_OriginCamPos.x, 0.f, m_OriginCamPos.z)) < 0.5f)
	{
		m_RestoreCamDirFix = false;
		m_RestoreCamDir = Vector3(0.f, 0.f, 0.f);
		return true;
	}

	else
	{
		CurrentCamera->AddWorldPos(m_RestoreCamDir.x * Speed * DeltaTime, 0.f, m_RestoreCamDir.z * Speed * DeltaTime);
	}


	return false;
}

bool CScene::RestoreCamera(double Time, const Vector3 CurrentCamPos, float DeltaTime)
{
	CCameraComponent* CurrentCamera = m_CameraManager->GetCurrentCamera();

	float TimeRatio = m_AccTime / Time;

	if (TimeRatio > 1.f)
		TimeRatio = 1.f;

	Vector3 LerpPos = m_OriginCamPos * TimeRatio + CurrentCamPos * (1.f - TimeRatio);

	CurrentCamera->SetWorldPos(LerpPos);

	m_AccTime += DeltaTime;

	if (TimeRatio == 1.f)
	{
		m_AccTime = 0.f;
		return true;
	}

	return false;
}

bool CScene::SetSceneMode(size_t SceneModeTypeID)
{
	size_t PrevSceneModeType = m_Mode->GetTypeID();

	if (PrevSceneModeType == SceneModeTypeID)
	{
		return true;
	}

	CSceneManager::GetInst()->CallCreateSceneMode(this, SceneModeTypeID);

	// 생성 실패
	if (PrevSceneModeType == SceneModeTypeID)
	{
		return false;
	}

	return true;
}


int CScene::GetSaveExcludeObjectCount()
{
	int Count = 0;

	auto iter = m_ObjList.begin();
	auto iterEnd = m_ObjList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->IsExcludeFromSceneSave())
		{
			++Count;
		}
	}

	return Count;
}

bool CScene::SortRenderList(CSceneComponent* Src, CSceneComponent* Dest)
{
	SphereInfo	SrcInfo = Src->GetSphereInfoViewSpace();
	SphereInfo	DestInfo = Dest->GetSphereInfoViewSpace();

	return SrcInfo.Center.Length() - SrcInfo.Radius > DestInfo.Center.Length() - DestInfo.Radius;
}
