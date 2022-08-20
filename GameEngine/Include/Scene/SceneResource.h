#pragma once

#include "../GameInfo.h"
#include "../Resource/Mesh/Mesh.h"
#include "../Resource/Shader/Shader.h"
#include "../Resource/Material/Material.h"
#include "../Resource/Texture/Texture.h"
#include "../Resource/Animation/AnimationSequence2D.h"
#include "../Resource/Animation/AnimationSequence.h"
#include "../Resource/Animation/Skeleton.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Sound/Sound.h"
#include "../Resource/Particle/Particle.h"

class CSceneResource
{
	friend class CScene;

private:
	CSceneResource();
	~CSceneResource();

private:
	class CScene* m_Scene;

private:
	std::unordered_map<std::string, CSharedPtr<CMesh>>		m_mapMesh;
	std::unordered_map<std::string, CSharedPtr<CShader>>	m_mapShader;
	std::unordered_map<std::string, CSharedPtr<CMaterial>>	m_mapMaterial;
	std::unordered_map<std::string, CSharedPtr<CTexture>>	m_mapTexture;
	std::unordered_map<std::string, CSharedPtr<CAnimationSequence2D>>	m_mapSequence2D;
	std::unordered_map<std::string, CSharedPtr<CSound>>		m_mapSound;
	std::unordered_map<std::string, CSharedPtr<CParticle>>		m_mapParticle;
	std::unordered_map<std::string, CSharedPtr<CAnimationSequence>>	m_mapSequence;
	std::unordered_map<std::string, CSharedPtr<CSkeleton>>	m_mapSkeleton;

public:	// =================== Mesh =====================

	bool CreateMesh(Mesh_Type Type, const std::string& Name,
		void* VtxData, int Size, int Count, D3D11_USAGE Usage,
		D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData = nullptr, int IdxSize = 0, int IdxCount = 0,
		D3D11_USAGE IdxUsage = D3D11_USAGE_DEFAULT, DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN);
	bool CreateNavMesh(class CNavMesh* NavMesh, const std::string& Name,
		void* VtxData, int Size, int Count, D3D11_USAGE Usage,
		D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData = nullptr, int IdxSize = 0, int IdxCount = 0,
		D3D11_USAGE IdxUsage = D3D11_USAGE_DEFAULT, DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN);
	bool LoadMesh(std::string& OutName, Mesh_Type Type,const TCHAR* FileName,
		const std::string& PathName = MESH_PATH);
	bool LoadMesh(Mesh_Type Type, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = MESH_PATH);
	bool LoadMeshFullPath(std::string& OutName, Mesh_Type Type,
		const TCHAR* FullPath);
	bool LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
		const TCHAR* FullPath);
	bool LoadMeshMultibyte(std::string& OutName, Mesh_Type Type, const char* FileName,
		const std::string& PathName = MESH_PATH);
	bool LoadMeshMultibyte(Mesh_Type Type, const std::string& Name, const char* FileName,
		const std::string& PathName = MESH_PATH);
	bool LoadMeshFullPathMultibyte(std::string& OutName, Mesh_Type Type,
		const char* FullPath);
	bool LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name,
		const char* FullPath);
	bool SetMeshSkeleton(const std::string& Name, const std::string& SkeletonName);
	bool SetMeshSkeleton(const std::string& Name, class CSkeleton* Skeleton);

	CMesh* FindMesh(const std::string& Name);
	void ReleaseMesh(const std::string& Name);
	void GetAllAnimationMeshNames(std::vector<std::string>& vecAnimationMeshName);

public:	// =================== Shader =====================
	template <typename T>
	bool CreateShader(const std::string& Name)
	{
		if (FindShader(Name))
			return true;

		if (!CResourceManager::GetInst()->CreateShader<T>(Name))
			return false;

		m_mapShader.insert(std::make_pair(Name, CResourceManager::GetInst()->FindShader(Name)));

		return true;
	}
	CShader* FindShader(const std::string& Name);


public:	// =================== Material =====================
	CMaterial* FindMaterial(const std::string& Name);
	CMaterial* LoadMaterialFullPathMultibyte(const char* FullPath);
	void ReleaseMaterial(const std::string& Name);

public:
	template <typename T>
	bool CreateMaterial(const std::string& Name)
	{
		if (FindMaterial(Name))
			return true;

		if (!CResourceManager::GetInst()->CreateMaterial<T>(Name))
			return false;

		m_mapMaterial.insert(std::make_pair(Name, CResourceManager::GetInst()->FindMaterial(Name)));

		return true;
	}

	template <typename T>
	T* CreateMaterialEmpty()
	{
		return CResourceManager::GetInst()->CreateMaterialEmpty<T>();
	}

public:	// =================== Texture =====================
	bool LoadTexture(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath);
	bool LoadTextureFullPath(std::string& OutName, const TCHAR* FullPath);
	bool LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	bool CreateTarget(const std::string& Name, unsigned int Width,
		unsigned int Height, DXGI_FORMAT PixelFormat, DXGI_FORMAT DepthFormat = DXGI_FORMAT_UNKNOWN);
	bool LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	class CTexture* FindTexture(const std::string& Name);
	void ReleaseTexture(const std::string& Name);

public:	// =================== Sequence2D =====================
	bool CreateAnimationSequence2D(const std::string& Name, const std::string& TextureName,
		const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
	bool CreateAnimationSequence2D(const std::string& Name, class CTexture* Texture);
	void AddAnimationSequence2DFrame(const std::string& Name, const Vector2& Start, const Vector2& Size);
	void AddAnimationSequence2DFrame(const std::string& Name, float StartX, float StartY, float Width, float Height);
	bool SaveSequence2DFullPath(const std::string& Name, const char* FullPath);
	bool LoadSequence2DFullPath(const char* FullPath);
	bool LoadSequence2DFullPath(std::string& resultName, const char* FullPath);
	bool SaveSequence2D(const std::string& Name, const char* FileName, const std::string& PathName = ANIMATION_PATH);
	bool LoadSequence2D(const char* FileName, const std::string& PathName = ANIMATION_PATH);
	bool LoadSequence2D(std::string& resultName, const char* FileName, const std::string& PathName = ANIMATION_PATH);
	CAnimationSequence2D* FindAnimationSequence2D(const std::string& Name);
	class CAnimation2DConstantBuffer* GetAnimation2DCBuffer()	const;

public:	// =================== Sound =====================
	bool LoadSound(const std::string& ChannelGroupName, bool Loop, const std::string& Name, const char* FileName,
		const std::string& PathName = SOUND_PATH);
	bool CreateSoundChannelGroup(const std::string& Name);
	bool SetVolume(int Volume);
	bool SetVolume(const std::string& ChannelGroupName, int Volume);
	bool SoundPlay(const std::string& Name);
	bool SoundStop(const std::string& Name);
	bool SoundPause(const std::string& Name);
	bool SoundResume(const std::string& Name);
	class CSound* FindSound(const std::string& Name);
	FMOD::ChannelGroup* FindChannelGroup(const std::string& Name);


public:	// =================== Particle =====================
	bool CreateParticle(const std::string& Name);
	CParticle* FindParticle(const std::string& Name);

	template <typename T>
	T* CreateParticleEmpty()
	{
		return CResourceManager::GetInst()->CreateParticleEmpty<T>();
	}

public:	// =================== Sequence3D =====================
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		struct _tagFbxAnimationClip* Clip);
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		int StartFrame, int EndFrame, float PlayTime,
		const std::vector<BoneKeyFrame*>& vecFrame);
	bool LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH);
	bool LoadAnimationSequenceFullPath(bool Loop, const std::string& Name, const TCHAR* FullPath);
	bool LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH);
	bool LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name, const char* FullPath);
	bool LoadAnimationSequenceFullPathMultibyteSetOriginFileName(bool Loop, std::string& Name, const char* FullPath);
	void AddSequenceInfo(CAnimationSequence* Sequence);
	CAnimationSequence* FindAnimationSequence(const std::string& Name);
	void DeleteSequence(const CAnimationSequence* const Sequence);

	bool LoadSkeleton(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH);
	bool LoadSkeletonFullPath(const std::string& Name, const TCHAR* FullPath);
	bool LoadSkeletonMultibyte(const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH);
	bool LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath);
	bool AddSocket(const std::string& SkeletonName, const std::string& BoneName, const std::string& SocketName,
		const Vector3& Offset = Vector3::Zero, const Vector3& OffsetRot = Vector3::Zero);
	CSkeleton* FindSkeleton(const std::string& Name);
	void GetAllSkeletonNames(std::vector<std::string>& vecSkeletonName);
};

