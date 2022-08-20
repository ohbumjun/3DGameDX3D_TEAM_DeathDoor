
#include "ResourceManager.h"
#include "../Animation/AnimationSequenceInstance.h"
#include "../Engine.h"
#include "../EngineUtil.h"
#include "../PathManager.h"
#include <filesystem>
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Scene/SceneResource.h"

DEFINITION_SINGLE(CResourceManager)

CResourceManager::CResourceManager()	:
	m_MeshManager(nullptr),
	m_ShaderManager(nullptr),
	m_MaterialManager(nullptr),
	m_TextureManager(nullptr),
	m_AnimationManager(nullptr),
	m_SoundManager(nullptr),
	m_FontManager(nullptr),
	m_ParticleManager(nullptr),
	m_AnimationManager3D(nullptr),
	m_ExcelManager(nullptr)
{
}

CResourceManager::~CResourceManager()
{
	SAFE_DELETE(m_ParticleManager);
	SAFE_DELETE(m_FontManager);
	SAFE_DELETE(m_SoundManager);
	SAFE_DELETE(m_AnimationManager);
	SAFE_DELETE(m_MaterialManager);
	SAFE_DELETE(m_ShaderManager);
	SAFE_DELETE(m_MeshManager);
	SAFE_DELETE(m_TextureManager);
	SAFE_DELETE(m_AnimationManager3D);
	SAFE_DELETE(m_ExcelManager);
}

bool CResourceManager::Init()
{
	m_MeshManager = new CMeshManager;

	if (!m_MeshManager->Init())
		return false;

	m_ShaderManager = new CShaderManager;

	if (!m_ShaderManager->Init())
		return false;

	m_TextureManager = new CTextureManager;

	if (!m_TextureManager->Init())
		return false;

	m_MaterialManager = new CMaterialManager;

	if (!m_MaterialManager->Init())
		return false;

	m_AnimationManager = new CAnimationManager;

	if (!m_AnimationManager->Init())
		return false;

	m_SoundManager = new CSoundManager;

	if (!m_SoundManager->Init())
		return false;

	m_FontManager = new CFontManager;

	if (!m_FontManager->Init())
		return false;

	m_ParticleManager = new CParticleManager;

	if (!m_ParticleManager->Init())
		return false;

	m_AnimationManager3D = new CAnimationManager3D;

	if (!m_AnimationManager3D->Init())
		return false;

	m_ExcelManager = new CExcelManager;

	if (!m_ExcelManager->Init())
		return false;

	return true;
}

void CResourceManager::Update()
{
	m_SoundManager->Update();
}

void CResourceManager::RenderTarget()
{
	CMesh* Mesh = m_MeshManager->FindMesh("WidgetMesh");
	CShader* Shader = m_ShaderManager->FindShader("RenderTargetShader");

	m_TextureManager->RenderTarget(Mesh, Shader);
}

bool CResourceManager::CreateMesh(Mesh_Type Type,
	const std::string& Name, void* VtxData, int Size,
	int Count, D3D11_USAGE Usage,
	D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData,
	int IdxSize, int IdxCount, D3D11_USAGE IdxUsage,
	DXGI_FORMAT Fmt, CScene* Scene)
{
	return m_MeshManager->CreateMesh(Type, Name, VtxData, Size, Count,
		Usage, Primitive, IdxData, IdxSize, IdxCount, IdxUsage,
		Fmt, Scene);
}

bool CResourceManager::CreateNavMesh(CNavMesh* NavMesh, const std::string& Name, void* VtxData, int Size, int Count,
	D3D11_USAGE Usage, D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData, int IdxSize, int IdxCount,
	D3D11_USAGE IdxUsage, DXGI_FORMAT Fmt, CScene* Scene)
{
	return m_MeshManager->CreateNavMesh(NavMesh, Name, VtxData, Size, Count,
		Usage, Primitive, IdxData, IdxSize, IdxCount, IdxUsage,
		Fmt, Scene);
}



bool CResourceManager::LoadMesh(std::string& OutName, Mesh_Type Type, const TCHAR* FileName, const std::string& PathName, CScene* Scene)
{
	return m_MeshManager->LoadMesh(OutName, Type, FileName, PathName, Scene);
}

bool CResourceManager::LoadMesh(Mesh_Type Type, const std::string& Name,
	const TCHAR* FileName, const std::string& PathName, class CScene* Scene)
{
	return m_MeshManager->LoadMesh(Type, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadMeshFullPath(std::string& OutName, Mesh_Type Type, const TCHAR* FullPath, CScene* Scene)
{
	return m_MeshManager->LoadMeshFullPath(OutName, Type, FullPath, Scene);
}

bool CResourceManager::LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
	const TCHAR* FullPath, class CScene* Scene)
{
	return m_MeshManager->LoadMeshFullPath(Type, Name, FullPath, Scene);
}

bool CResourceManager::LoadMeshMultibyte(std::string& OutName, Mesh_Type Type, const char* FileName, const std::string& PathName, CScene* Scene)
{
	return m_MeshManager->LoadMeshMultibyte(OutName, Type, FileName, PathName, Scene);
}

bool CResourceManager::LoadMeshMultibyte(Mesh_Type Type, const std::string& Name,
	const char* FileName, const std::string& PathName, class CScene* Scene)
{
	return m_MeshManager->LoadMeshMultibyte(Type, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadMeshFullPathMultibyte(std::string& OutName, Mesh_Type Type, const char* FullPath, CScene* Scene)
{
	return m_MeshManager->LoadMeshFullPathMultibyte(OutName, Type, FullPath, Scene);
}

bool CResourceManager::LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name, const char* FullPath, class CScene* Scene)
{
	return m_MeshManager->LoadMeshFullPathMultibyte(Type, Name, FullPath, Scene);
}

bool CResourceManager::SetMeshSkeleton(const std::string& Name, 
	const std::string& SkeletonName)
{
	CSkeleton* Skeleton = FindSkeleton(SkeletonName);

	if (!Skeleton)
		return false;

	return SetMeshSkeleton(Name, Skeleton);
}

bool CResourceManager::SetMeshSkeleton(const std::string& Name, CSkeleton* Skeleton)
{
	return m_MeshManager->SetMeshSkeleton(Name, Skeleton);
}

bool CResourceManager::ConvertFBXLocalFormatFullPathMultiByte(const char* FullPath)
{
	return m_MeshManager->ConvertFBXLocalFormatFullPathMultiByte(FullPath);
}

CMesh* CResourceManager::FindMesh(const std::string& Name)
{
	return m_MeshManager->FindMesh(Name);
}

void CResourceManager::ReleaseMesh(const std::string& Name)
{
	m_MeshManager->ReleaseMesh(Name);
}

std::pair<bool, std::string> CResourceManager::LoadMeshTextureBoneInfo(const char* ConstMeshFileName,  bool IsLastSqcFile)
{
	const PathInfo* Path = CPathManager::GetInst()->FindPath(MESH_PATH);

	char	FullPath[MAX_PATH] = {};

	if (Path)
		strcpy_s(FullPath, Path->PathMultibyte);

	TCHAR TCHARPureFileName[MAX_PATH] = {};
	int PureNameConvertLength = MultiByteToWideChar(CP_ACP, 0, ConstMeshFileName, -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, ConstMeshFileName, -1, TCHARPureFileName, PureNameConvertLength);

	char MeshFileName[MAX_PATH] = {};
	TCHAR MeshTCHARFileName[MAX_PATH] = {};

	char MeshExt[10] = ".msh";

	// Mesh File Name
	strcat_s(MeshFileName, ConstMeshFileName);
	strcat_s(MeshFileName, MeshExt);

	// Load Mesh
	std::string LoadedMeshName = ConstMeshFileName;
	LoadedMeshName.append("_mesh");

	int ConvertLength = MultiByteToWideChar(CP_ACP, 0, MeshFileName, -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, MeshFileName, -1, MeshTCHARFileName, ConvertLength);

	//MeshTCHARFileName 와 Path Name 을 더해서 해당 FullPath 를 만들어낸다.
	// 1. 기존 방식대로 Load Mesh 진행
	bool LoadMeshResult = LoadMesh(Mesh_Type::Animation, LoadedMeshName,
		MeshTCHARFileName, MESH_PATH);

	// 2. 없다면, 새롭게 MESH_PATH 전체를 돌면서 해당 FileName 이 존재하는지를 찾은 이후, FullPath 를 만들어서 새롭게 Load 시도
	if (!LoadMeshResult)
	{
		// 먼저 해당 경로에 파일이 존재하는지 확인하기
		auto FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, MeshFileName);

		TCHAR FulllErrorMessage[MAX_PATH] = {};
		TCHAR ErrorMessage[MAX_PATH] = TEXT(".msh Load Failure in Bin//Mesh");
		lstrcpy(FulllErrorMessage, TCHARPureFileName);
		lstrcat(FulllErrorMessage, ErrorMessage);

		// 해당 경로에 Mesh 파일이 존재하지 않는다면 Return
		if (!FoundResult.has_value())
		{
			if (IsLastSqcFile)
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);

			return std::make_pair(false, "");
		}

		// 위에서 얻어온 FullPath 로 한번더 Load 시도
		LoadMeshResult = LoadMeshFullPathMultibyte(Mesh_Type::Animation, LoadedMeshName, FoundResult.value().c_str());

		if (!LoadMeshResult)
		{
			if (IsLastSqcFile)
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);

			return std::make_pair(false, "");
		}
	}

	// 만약 Mesh Load 과정에서 필요한 Texture가 없다면 
	// ex) FBX Convert 이후, singed_spell2.sqc 가 있다면, 같은 경로내에 singed_spell2.fbm 이라는 디렉토리가 존재해야 한다.
	// 만약 해당 Folder 가 존재하지 않는다면, Mesh를 Load 하더라도 Texture 가 없다고 뜰 것이다
	char TextFolderExt[10] = ".fbm";
	// char TextFilePath[MAX_PATH] = {};
	// TCHAR MshTCHARFileName[MAX_PATH] = {};

	char MeshFileFullPath[MAX_PATH] = {};
	TCHAR MeshTCHARFileFullPath[MAX_PATH] = {};

	// Mesh File Path
	strcpy_s(MeshFileFullPath, FullPath);
	strcat_s(MeshFileFullPath, ConstMeshFileName);

	std::string TargetFolderPath = ConstMeshFileName;
	TargetFolderPath.append(TextFolderExt); //.fbm 붙여주기

	// strcpy_s(TextFilePath, MeshFileName);
	// strcat_s(TextFilePath, TextFolderExt); // 

	auto FoundTextureFolderResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, TargetFolderPath.c_str());

	if (FoundTextureFolderResult.has_value() == false)
	{
		if (IsLastSqcFile)
		{
			TCHAR FulllErrorMessage[MAX_PATH] = {};
			TCHAR ErrorMessage[MAX_PATH] = TEXT(".fbm Folder Does Not Exist in Bin//Mesh");

			lstrcpy(FulllErrorMessage, TCHARPureFileName);
			lstrcat(FulllErrorMessage, ErrorMessage);

			MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);
		}
		return std::make_pair(false, "");
	}

	// ex) singed_spell2.sqc 를 선택했다면
	// 같은 폴더 목록 내에서 singed_spell2.msh / singed_spell2.bne 를 Load 하여 세팅한다.
	// singed_spell2.msh -> singed_spell2_mesh 라는 이름으로
	// singed_spell2.bne -> singed_spell2_skeleton 이라는 이름으로


	// Bne (Skeleton) Load
	char BneExt[10] = ".bne";

	std::string LoadedBneName = ConstMeshFileName;
	LoadedBneName.append("_skeleton");

	char BneFileName[MAX_PATH] = {};
	TCHAR BneTCHARFileName[MAX_PATH] = {};

	strcpy_s(BneFileName, ConstMeshFileName);
	strcat_s(BneFileName, BneExt);

	ConvertLength = MultiByteToWideChar(CP_ACP, 0, BneFileName, -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, BneFileName, -1, BneTCHARFileName, ConvertLength);

	// BneTCHARFileName 와 Path Name 을 더해서 해당 FullPath 를 만들어낸다.
	// 1. 기존 방식대로 LoadSkeleton 진행
	// 2. 없다면, 새롭게 MESH_PATH 전체를 돌면서 해당 FileName 이 존재하는지를 찾은 이후, FullPath 를 만들어서 새롭게 Load 시도
	bool LoadSkeletonResult = LoadSkeleton(LoadedBneName,
		BneTCHARFileName, MESH_PATH);

	if (!LoadSkeletonResult)
	{
		// 먼저 해당 경로에 .bne 파일이 존재하는지 확인하기
		auto FoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, BneFileName);

		TCHAR FulllErrorMessage[MAX_PATH] = {};
		TCHAR ErrorMessage[MAX_PATH] = TEXT(".bne Load Failure in Bin//Mesh");
		lstrcpy(FulllErrorMessage, TCHARPureFileName);
		lstrcat(FulllErrorMessage, ErrorMessage);

		// 해당 경로에 Mesh 파일이 존재하지 않는다면 Return
		if (!FoundResult.has_value())
		{
			if (IsLastSqcFile)
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);

			return std::make_pair(false, "");
		}

		// 위에서 얻어온 FullPath 로 한번더 Load 시도
		LoadSkeletonResult = LoadSkeletonFullPathMultibyte(LoadedBneName, FoundResult.value().data());

		if (!LoadSkeletonResult)
		{
			if (IsLastSqcFile)
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);

			return std::make_pair(false, "");
		}
	}

	// Mesh 에 해당 Skeleton 세팅
	SetMeshSkeleton(LoadedMeshName, LoadedBneName);

	return std::make_pair(true, LoadedMeshName);;
}

std::pair<bool, std::string> CResourceManager::LoadMeshTextureBoneInfo(CAnimationSequenceInstance* Instance)
{
	std::unordered_map<std::string, CAnimationSequenceData*>& mapAnimInfo = Instance->GetAnimationSequenceMap();

	auto iter = mapAnimInfo.begin();
	auto iterEnd = mapAnimInfo.end();

	int MapSize = (int)mapAnimInfo.size();
	int MapCnt = 0;

	for (; iter != iterEnd; ++iter)
	{
		MapCnt++;

		// const char* ConstSqcFileName = Instance->GetCurrentAnimation()->GetAnimationSequence()->GetSequenceFileNameMultibyte();
		const char* ConstSqcFileName = iter->second->GetAnimationSequence()->GetSequenceFileNameMultibyte();

		char SqcFileName[MAX_PATH] = {};
		strcpy_s(SqcFileName, ConstSqcFileName);

		// SqcFileName이 시퀀스중에서 처음으로 텍스쳐를 가지고있는 시퀀스 이름?
		const std::pair<bool, std::string>& Result = LoadMeshTextureBoneInfo(SqcFileName, MapCnt == MapSize);

		if (Result.first == false)
		{
			if (MapCnt == MapSize)
				return std::make_pair(false, "");
			else
				continue;
		}
		// 찾았다면 Return
		else
			return Result;
	}

	return std::make_pair(false, "");
}

CShader* CResourceManager::FindShader(const std::string& Name)
{
	return m_ShaderManager->FindShader(Name);
}

CGraphicShader* CResourceManager::FindInstancingShader(CGraphicShader* Shader)
{
	return m_ShaderManager->FindInstancingShader(Shader);
}

void CResourceManager::ReleaseShader(const std::string& Name)
{
	m_ShaderManager->ReleaseShader(Name);
}

bool CResourceManager::CreateConstantBuffer(const std::string& Name, int Size, int Register,
	int ConstantBufferShaderType)
{
	return m_ShaderManager->CreateConstantBuffer(Name, Size, Register, ConstantBufferShaderType);
}

CConstantBuffer* CResourceManager::FindConstantBuffer(const std::string& Name)
{
	return m_ShaderManager->FindConstantBuffer(Name);
}

CMaterial* CResourceManager::FindMaterial(const std::string& Name)
{
	return m_MaterialManager->FindMaterial(Name);
}

CMaterialConstantBuffer* CResourceManager::GetMaterialConstantBuffer() const
{
	return m_MaterialManager->GetMaterialConstantBuffer();
}

CMaterial* CResourceManager::LoadMaterialFullPathMultibyte(const char* FullPath)
{
	return m_MaterialManager->LoadMaterialFullPathMultibyte(FullPath);
}

void CResourceManager::ReleaseMaterial(const std::string& Name)
{
	m_MaterialManager->ReleaseMaterial(Name);
}

bool CResourceManager::LoadTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	return m_TextureManager->LoadTexture(Name, FileName, PathName);
}

bool CResourceManager::LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath)
{
	return m_TextureManager->LoadTextureFullPath(Name, FullPath);
}

bool CResourceManager::LoadTextureFullPath(std::string& OutName, const TCHAR* FullPath)
{
	return m_TextureManager->LoadTextureFullPath(OutName, FullPath);
}

bool CResourceManager::LoadTextureFullPathMultibyte(const std::string& Name, const char* FullPath)
{
	return m_TextureManager->LoadTextureFullPathMultibyte(Name, FullPath);
}

bool CResourceManager::LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	return m_TextureManager->LoadTexture(Name, vecFileName, PathName);
}

bool CResourceManager::LoadTextureFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath)
{
	return m_TextureManager->LoadTextureFullPath(Name, vecFullPath);
}

bool CResourceManager::CreateTarget(const std::string& Name, unsigned int Width, 
	unsigned int Height, DXGI_FORMAT PixelFormat, bool MultiSample, 
	DXGI_FORMAT DepthFormat)
{
	return m_TextureManager->CreateTarget(Name, Width, Height, PixelFormat, MultiSample, DepthFormat);
}

bool CResourceManager::LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	return m_TextureManager->LoadTextureArray(Name, vecFileName, PathName);
}

bool CResourceManager::LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath)
{
	return m_TextureManager->LoadTextureArrayFullPath(Name, vecFullPath);
}

CTexture* CResourceManager::FindTexture(const std::string& Name)
{
	return m_TextureManager->FindTexture(Name);
}

void CResourceManager::ReleaseTexture(const std::string& Name)
{
	m_TextureManager->ReleaseTexture(Name);
}

bool CResourceManager::CreateAnimationSequence2D(const std::string& Name,
	const std::string& TextureName, const TCHAR* FileName, const std::string& PathName)
{
	return m_AnimationManager->CreateAnimationSequence2D(Name, TextureName, FileName, PathName);
}

bool CResourceManager::CreateAnimationSequence2D(const std::string& Name, CTexture* Texture)
{
	return m_AnimationManager->CreateAnimationSequence2D(Name, Texture);
}

void CResourceManager::AddAnimationSequence2DFrame(const std::string& Name, const Vector2& Start,
	const Vector2& Size)
{
	m_AnimationManager->AddFrame(Name, Start, Size);
}

void CResourceManager::AddAnimationSequence2DFrame(const std::string& Name, float StartX,
	float StartY, float Width, float Height)
{
	m_AnimationManager->AddFrame(Name, StartX, StartY, Width, Height);
}

bool CResourceManager::SaveSequence2DFullPath(const std::string& Name, const char* FullPath)
{
	return m_AnimationManager->SaveSequenceFullPath(Name, FullPath);
}

bool CResourceManager::LoadSequence2DFullPath(std::string& resultName, const char* FullPath, class CScene* Scene)
{
	return m_AnimationManager->LoadSequenceFullPath(resultName, FullPath, Scene);
}

bool CResourceManager::LoadSequence2DFullPath(const char* FullPath, CScene* Scene)
{
	std::string	Name;
	return m_AnimationManager->LoadSequenceFullPath(Name, FullPath, Scene);
}

bool CResourceManager::SaveSequence2D(const std::string& Name, const char* FileName, const std::string& PathName)
{
	return m_AnimationManager->SaveSequence(Name, FileName, PathName);
}

bool CResourceManager::LoadSequence2D(const char* FileName, const std::string& PathName, CScene* Scene)
{
	std::string	Name;
	return m_AnimationManager->LoadSequence(Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadSequence2D(std::string& resultName, const char* FileName, const std::string& PathName, CScene* Scene)
{
	return m_AnimationManager->LoadSequence(resultName, FileName, PathName, Scene);
}

CAnimationSequence2D* CResourceManager::FindAnimationSequence2D(const std::string& Name)
{
	return m_AnimationManager->FindSequence(Name);
}

void CResourceManager::ReleaseAnimationSequence2D(const std::string& Name)
{
	m_AnimationManager->ReleaseSequence(Name);
}

CAnimation2DConstantBuffer* CResourceManager::GetAnimation2DCBuffer() const
{
	return m_AnimationManager->GetAnimation2DCBuffer();
}

bool CResourceManager::LoadSound(const std::string& ChannelGroupName, bool Loop, const std::string& Name,
	const char* FileName, const std::string& PathName)
{
	return m_SoundManager->LoadSound(ChannelGroupName, Loop, Name, FileName, PathName);
}

bool CResourceManager::CreateSoundChannelGroup(const std::string& Name)
{
	return m_SoundManager->CreateSoundChannelGroup(Name);
}

bool CResourceManager::SetVolume(int Volume)
{
	return m_SoundManager->SetVolume(Volume);
}

bool CResourceManager::SetVolume(const std::string& ChannelGroupName, int Volume)
{
	return m_SoundManager->SetVolume(ChannelGroupName, Volume);
}

bool CResourceManager::SoundPlay(const std::string& Name)
{
	return m_SoundManager->SoundPlay(Name);
}

bool CResourceManager::SoundStop(const std::string& Name)
{
	return m_SoundManager->SoundStop(Name);
}

bool CResourceManager::SoundPause(const std::string& Name)
{
	return m_SoundManager->SoundPause(Name);
}

bool CResourceManager::SoundResume(const std::string& Name)
{
	return m_SoundManager->SoundResume(Name);
}

CSound* CResourceManager::FindSound(const std::string& Name)
{
	return m_SoundManager->FindSound(Name);
}

FMOD::ChannelGroup* CResourceManager::FindChannelGroup(const std::string& Name)
{
	return m_SoundManager->FindChannelGroup(Name);
}

void CResourceManager::ReleaseSound(const std::string& Name)
{
	m_SoundManager->ReleaseSound(Name);
}

bool CResourceManager::CreateFontFile(const std::string& Name, const TCHAR* FileName, 
	const std::string& PathName)
{
	return m_FontManager->CreateFontFile(Name, FileName, PathName);
}

bool CResourceManager::LoadFont(const std::string& Name, const TCHAR* FontName, 
	int Weight, float FontSize, const TCHAR* LocalName, int Stretch)
{
	return m_FontManager->LoadFont(Name, FontName, Weight, FontSize, LocalName, Stretch);
}

const TCHAR* CResourceManager::GetFontFaceName(const std::string& Name)
{
	return m_FontManager->GetFontFaceName(Name);
}

const char* CResourceManager::GetFontFaceNameMultibyte(const std::string& Name)
{
	return m_FontManager->GetFontFaceNameMultibyte(Name);
}

bool CResourceManager::CreateFontColor(float r, float g, float b, float a)
{
	return m_FontManager->CreateFontColor(r, g, b, a);
}

bool CResourceManager::CreateFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return m_FontManager->CreateFontColor(r, g, b, a);
}

bool CResourceManager::CreateFontColor(const Vector4& Color)
{
	return m_FontManager->CreateFontColor(Color);
}

bool CResourceManager::CreateFontColor(unsigned int Color)
{
	return m_FontManager->CreateFontColor(Color);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(float r, float g, float b, float a)
{
	return m_FontManager->FindFontColor(r, g, b, a);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return m_FontManager->FindFontColor(r, g, b, a);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(const Vector4& Color)
{
	return m_FontManager->FindFontColor(Color);
}

ID2D1SolidColorBrush* CResourceManager::FindFontColor(unsigned int Color)
{
	return m_FontManager->FindFontColor(Color);
}

unsigned int CResourceManager::CreateFontColorKey(float r, float g, float b, float a)
{
	return m_FontManager->CreateFontColorKey(r, g, b, a);
}

unsigned int CResourceManager::CreateFontColorKey(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return m_FontManager->CreateFontColorKey(r, g, b, a);
}

unsigned int CResourceManager::CreateFontColorKey(const Vector4& Color)
{
	return m_FontManager->CreateFontColorKey(Color);
}

IDWriteTextLayout* CResourceManager::CreateTextLayout(const TCHAR* Text, IDWriteTextFormat* Font,
	float Width, float Height)
{
	return m_FontManager->CreateTextLayout(Text, Font, Width, Height);
}

IDWriteTextLayout* CResourceManager::CreateTextLayout(const TCHAR* Text, const std::string& FontName, 
	float Width, float Height)
{
	return m_FontManager->CreateTextLayout(Text, FontName, Width, Height);
}

IDWriteFontCollection1* CResourceManager::FindFontFile(const std::string& Name)
{
	return m_FontManager->FindFontFile(Name);
}

IDWriteTextFormat* CResourceManager::FindFont(const std::string& Name)
{
	return m_FontManager->FindFont(Name);
}

bool CResourceManager::CreateParticle(const std::string& Name)
{
	return m_ParticleManager->CreateParticle(Name);
}

CParticle* CResourceManager::FindParticle(const std::string& Name)
{
	return m_ParticleManager->FindParticle(Name);
}

void CResourceManager::ReleaseParticle(const std::string& Name)
{
	m_ParticleManager->ReleaseParticle(Name);
}

bool CResourceManager::LoadAnimationSequenceConvertFBX(const std::string& Name, bool bLoop, _tagFbxAnimationClip* pClip)
{
	return m_AnimationManager3D->LoadSequenceConvertFBX(Name, bLoop, pClip);
}

bool CResourceManager::LoadAnimationSequence(const std::string& Name, bool Loop,
	_tagFbxAnimationClip* Clip, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequence(Name, Loop, Clip, Scene);
}

bool CResourceManager::LoadAnimationSequence(const std::string& Name, bool Loop, int StartFrame,
	int EndFrame, float PlayTime, const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequence(Name, Loop, StartFrame, EndFrame, 
		PlayTime, vecFrame, Scene);
}

bool CResourceManager::LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
	const std::string& PathName, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequence(Loop, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadAnimationSequenceFullPath(bool Loop, const std::string& Name,
	const TCHAR* FullPath, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequenceFullPath(Loop, Name, FullPath, Scene);
}

bool CResourceManager::LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name,
	const char* FileName, const std::string& PathName, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequenceMultibyte(Loop, Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name,
	const char* FullPath, class CScene* Scene)
{
	return m_AnimationManager3D->LoadAnimationSequenceFullPathMultibyte(Loop, Name, FullPath, Scene);
}

bool CResourceManager::LoadAnimationSequenceFullPathMultibyteSetOriginFileName(bool Loop, std::string& Name, const char* FullPath)
{
	return m_AnimationManager3D->LoadAnimationSequenceFullPathMultibyteSetOriginFileName(Loop, Name, FullPath);
}

CAnimationSequence* CResourceManager::FindAnimationSequence(const std::string& Name)
{
	return m_AnimationManager3D->FindAnimationSequence(Name);
}

CAnimationSequence* CResourceManager::CreateBasicAnimationSequence(const std::string& Name)
{
	return m_AnimationManager3D->CreateBasicAnimationSequence(Name);
}

void CResourceManager::ReleaseAnimationSequence3D(CAnimationSequence* ExistingSequence)
{
	m_AnimationManager3D->ReleaseAnimationSequence3D(ExistingSequence);
}

void CResourceManager::ReleaseAnimationSequence3D(const std::string& Name)
{
	m_AnimationManager3D->ReleaseSequence(Name);
}

void CResourceManager::DeleteSequence3D(const std::string& Name)
{
	m_AnimationManager3D->DeleteSequence(Name);
}

bool CResourceManager::EditSequenceClip(CAnimationSequence* ExistingSequence, const std::string& NewName, int StartFrame, int EndFrame, const char* SaveFullPathMultibyte)
{
	return m_AnimationManager3D->EditSequenceClip(ExistingSequence, NewName, StartFrame, EndFrame, SaveFullPathMultibyte);
}

void CResourceManager::LoadAniFile()
{
	m_MeshManager->LoadAniFile();
}

bool CResourceManager::LoadSkeleton(const std::string& Name, 
	const TCHAR* FileName, const std::string& PathName, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeleton(Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadSkeletonFullPath(const std::string& Name,
	const TCHAR* FullPath, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeletonFullPath(Name, FullPath, Scene);
}

bool CResourceManager::LoadSkeletonMultibyte(const std::string& Name, const char* FileName, const std::string& PathName, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeletonMultibyte(Name, FileName, PathName, Scene);
}

bool CResourceManager::LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath, CScene* Scene)
{
	return m_AnimationManager3D->LoadSkeletonFullPathMultibyte(Name, FullPath, Scene);
}

CSkeleton* CResourceManager::FindSkeleton(const std::string& Name)
{
	return m_AnimationManager3D->FindSkeleton(Name);
}

bool CResourceManager::AddSocket(const std::string& SkeletonName, const std::string& BoneName, const std::string& SocketName, const Vector3& Offset, const Vector3& OffsetRot)
{
	return m_AnimationManager3D->AddSocket(SkeletonName, BoneName, SocketName, Offset, OffsetRot);
}

void CResourceManager::ReleaseSkeleton(const std::string& Name)
{
	m_AnimationManager3D->ReleaseSkeleton(Name);
}

bool CResourceManager::CreateCSV(const std::string& name)
{
	return m_ExcelManager->CreateCSV(name);
}

CExcelData* CResourceManager::FindCSV(const std::string& name)
{
	return m_ExcelManager->FindCSV(name);
}

bool CResourceManager::DeleteCSV(const std::string& name)
{
	return m_ExcelManager->DeleteCSV(name);
}

bool CResourceManager::SaveCSV(const std::string& name, const char* fileName, const std::string& pathName)
{
	return m_ExcelManager->SaveCSV(name, fileName, pathName);
}

bool CResourceManager::SaveCSVFullPath(const std::string& name, const char* fullPath)
{
	return m_ExcelManager->SaveCSVFullPath(name, fullPath);
}

bool CResourceManager::LoadCSV(const char* fileName, const std::string& pathName)
{
	return m_ExcelManager->LoadCSV(fileName, pathName);
}

bool CResourceManager::LoadCSV(std::string& outName, const char* fileName, const std::string& pathName)
{
	return m_ExcelManager->LoadCSV(outName, fileName, pathName);
}

bool CResourceManager::LoadCSVFullPath(const char* fullPath)
{
	return m_ExcelManager->LoadCSVFullPath(fullPath);
}

bool CResourceManager::LoadCSVFullPath(std::string& outName, const char* fullPath)
{
	return m_ExcelManager->LoadCSVFullPath(outName, fullPath);
}
