
#include "MaterialManager.h"
#include "../ResourceManager.h"
#include "../Shader/MaterialConstantBuffer.h"

CMaterialManager::CMaterialManager()	:
	m_CBuffer(nullptr)
{
}

CMaterialManager::~CMaterialManager()
{
	SAFE_DELETE(m_CBuffer);

	auto iter = m_mapMaterial.begin();
	auto iterEnd = m_mapMaterial.end();

	for (; iter != iterEnd;)
	{
		iter = m_mapMaterial.erase(iter);
		iterEnd = m_mapMaterial.end();
	}

	m_mapMaterial.empty();
}

bool CMaterialManager::Init()
{
	m_CBuffer = new CMaterialConstantBuffer;

	m_CBuffer->Init();

	// Color Material
	CreateMaterial<CMaterial>("Color");

	CSharedPtr<CMaterial>	Mtrl = FindMaterial("Color");

	Mtrl->SetShader("ColorMeshShader");

	const char* Practice = "hellos";
	size_t length = strlen(Practice);

	// std::string_view ExtractName(std::string_view fileName)
	// {
	// 	return fileName.substr(fileName.find('r')));
	// };

	// ParticleEditorBaseGround Material
	CreateMaterial<CMaterial>("ParticleEditorBaseGround");

	Mtrl = FindMaterial("ParticleEditorBaseGround");

	Mtrl->SetShader("Mesh2DShader");

	CTexture* Texture = CResourceManager::GetInst()->FindTexture("DefaultBurnTexture");

	Mtrl->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "DefaultBurnTexture", Texture);

	// Base Texture Material
	CreateMaterial<CMaterial>("BaseTexture");

	Mtrl = FindMaterial("BaseTexture");

	Mtrl->SetShader("Mesh2DShader");

	Texture = CResourceManager::GetInst()->FindTexture("EngineTexture");

	Mtrl->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "EngineTexture", Texture);

	// Sky Material
	CreateMaterial<CMaterial>("SkyMaterial");

	Mtrl = FindMaterial("SkyMaterial");

	Mtrl->SetShader("SkyShader");

	Mtrl->AddTexture(20, (int)Buffer_Shader_Type::Pixel, "DefaultSky", TEXT("Sky/Sky.dds"));

	// Default Decal Material
	CreateMaterial<CMaterial>("DefaultDecal");

	Mtrl = FindMaterial("DefaultDecal");

	Mtrl->SetShader("DecalShader");

	Mtrl->AddTexture(0, (int)Buffer_Shader_Type::Pixel,
		"DefaultDecal", TEXT("Decal/Decal.png"));
	Mtrl->AddTexture(1, (int)Buffer_Shader_Type::Pixel,
		"DefaultDecalNormal", TEXT("Decal/Decal_NRM.png"));
	Mtrl->AddTexture(2, (int)Buffer_Shader_Type::Pixel,
		"DefaultDecalSpecular", TEXT("Decal/Decal_SPEC.png"));

	Mtrl->EnableBump();
	Mtrl->EnableSpecularTex();

	CreateMaterial<CMaterial>("DebugDecal");

	Mtrl = FindMaterial("DebugDecal");

	Mtrl->SetShader("DecalDebugShader");

	// BillBoard Material
	CreateMaterial<CMaterial>("Billboard");

	Mtrl = FindMaterial("Billboard");

	Mtrl->SetShader("BillboardShader");

	Texture = CResourceManager::GetInst()->FindTexture("EngineTexture");

	Mtrl->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "EngineTexture", Texture);

	return true;
}

CMaterial* CMaterialManager::FindMaterial(const std::string& Name)
{
	auto	iter = m_mapMaterial.find(Name);

	if (iter == m_mapMaterial.end())
		return nullptr;

	return iter->second;
}

void CMaterialManager::ReleaseMaterial(const std::string& Name)
{
	auto	iter = m_mapMaterial.find(Name);

	if (iter != m_mapMaterial.end())
	{
		if (iter->second->GetRefCount() == 1)
		{
			if (m_ChangeCallBack)
			{
				m_ChangeCallBack();
			}
			m_mapMaterial.erase(iter);
		}
	}
}

CMaterial* CMaterialManager::LoadMaterialFullPathMultibyte(const char* FullPath)
{
	// 현재 자체적으로 만들어서 사용하는 Material 들의 경우
	// .mtrl 파일 이름이 곧, Material 의 이름이다.
	// 따라서, 해당 FullPath 에서 이름을 추출하여, 해당 Material 이 이미 Load되어 있는지 확인하는 과정을 거칠 것이다.

	char FileName[MAX_PATH] = {};

	_splitpath_s(FullPath, nullptr, 0, nullptr, 0, FileName, MAX_PATH, nullptr, 0);

	const char* MaterialName = FileName;

	CMaterial* FoundMaterial = FindMaterial(MaterialName);

	if (FoundMaterial)
		return FoundMaterial;

	CMaterial* LoadedMaterial = CreateMaterialEmpty<CMaterial>();


	if (!LoadedMaterial->LoadFullPath(FullPath))
	{
		SAFE_DELETE(LoadedMaterial);
		return nullptr;
	}

	// LoadedMaterial->SetName(NewMaterialName);
	
	m_mapMaterial.insert(std::make_pair(MaterialName, LoadedMaterial));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}

	return LoadedMaterial;
}

void CMaterialManager::AddMaterial(CMaterial* Material)
{
	CMaterial* FoundMaterial = FindMaterial(Material->GetName());

	if (FoundMaterial)
		return;

	m_mapMaterial.insert(std::make_pair(Material->GetName(), Material));

	if (m_ChangeCallBack)
	{
		m_ChangeCallBack();
	}
}
