
#include "SkyObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Material/Material.h"
#include "../PathManager.h"
#include "../EngineUtil.h"

CSkyObject::CSkyObject()
{
	SetTypeID<CSkyObject>();
}

CSkyObject::CSkyObject(const CSkyObject& obj) :
	CGameObject(obj)
{
	m_Mesh = (CStaticMeshComponent*)FindComponent("Mesh");
}

CSkyObject::~CSkyObject()
{
}

bool CSkyObject::Init()
{
	m_Mesh = CreateComponent<CStaticMeshComponent>("Mesh");

	m_Mesh->SetLayerName("Back");

	m_Mesh->SetMesh("SpherePos");

	CMaterial* Material = m_Scene->GetResource()->FindMaterial("SkyMaterial");

	// FrontFace Culling 으로 안에서 밖으로 바라보는 시점에서도 Sky Mesh 출력을 수행하도록 한다.
	// SkyDepth State에서는 최종 투영 변환된 z값의 출력 상한을 1과 같거나 작은 경우로 설정한다.
	// Shader에서 이 메쉬의 z값을 무조건 1로 변경해줌.
	Material->SetRenderState("FrontFaceCull");
	Material->SetRenderState("SkyDepth");

	m_Mesh->AddMaterial(Material);

	m_Mesh->SetRelativeScale(100000.f, 100000.f, 100000.f);

	return true;
}

void CSkyObject::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CSkyObject::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CSkyObject* CSkyObject::Clone()
{
	return new CSkyObject(*this);
}

bool CSkyObject::SetSkyTexture(const char* FileName, const std::string& PathName)
{
	char FullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(PathName);

	if (Info)
	{
		strcpy_s(FullPath, Info->PathMultibyte);
	}

	strcat_s(FullPath, FileName);

	return SetSkyTextureFullPath(FullPath);
}

bool CSkyObject::SetSkyTextureFullPath(const char* FullPath)
{
	CSceneResource* Resource = m_Scene->GetResource();

	// 기존 Sky Material과 Texture 삭제
	CMaterial* Mat = m_Mesh->GetMaterial();
	std::string MatName = Mat->GetName();

	CTexture* Tex = Mat->GetTexture();
	std::string TexName = Tex->GetName();

	m_Mesh->ClearMaterial();

	Resource->ReleaseMaterial(MatName);
	CResourceManager::GetInst()->ReleaseTexture(TexName);

	TCHAR FullPathWideChar[MAX_PATH] = {};
	int Length = MultiByteToWideChar(CP_ACP, 0, FullPath, -1, nullptr, 0);
	MultiByteToWideChar(CP_ACP, 0, FullPath, Length, FullPathWideChar, Length);
	
	bool Success = Resource->CreateMaterial<CMaterial>("SkyMaterial");
	Mat = Resource->FindMaterial("SkyMaterial");
	CResourceManager::GetInst()->ReleaseTexture("NewSky");
	Mat->ClearTexture();
	Mat->AddTextureFullPath(20, (int)Buffer_Shader_Type::Pixel, "NewSky", FullPathWideChar);
	Mat->SetRenderState("FrontFaceCull");
	Mat->SetRenderState("SkyDepth");
	Mat->SetShader("SkyShader");
	m_Mesh->AddMaterial(Mat);

	// Scene Save Data에 Skybox 경로 업데이트
	std::string FileName = CEngineUtil::ExtractFilePathFromFullPath(FullPath, TEXTURE_PATH);
	SceneSaveGlobalData GlobalData = m_Scene->GetSceneSaveGlobalData();
	GlobalData.BackGroundData.SkyBoxFileName = FileName;
	m_Scene->SetSceneSaveGlobalData(GlobalData);

	return Success;
}

CTexture* CSkyObject::GetSkyTexture()
{
	CMaterial* Mat = m_Mesh->GetMaterial();
	CTexture* Tex = Mat->GetTexture();
	return Tex;
}
