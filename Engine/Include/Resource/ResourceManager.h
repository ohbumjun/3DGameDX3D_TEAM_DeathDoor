#pragma once

#include "Mesh/MeshManager.h"
#include "Shader/ShaderManager.h"
#include "Material/MaterialManager.h"
#include "Texture/TextureManager.h"
#include "Animation/AnimationManager.h"
#include "Animation/AnimationManager3D.h"
#include "Sound/SoundManager.h"
#include "Font/FontManager.h"
#include "Particle/ParticleManager.h"
#include "Excel/ExcelManager.h"

class CResourceManager
{
private:
	CMeshManager* m_MeshManager;
	CShaderManager* m_ShaderManager;
	CMaterialManager* m_MaterialManager;
	CTextureManager* m_TextureManager;
	CAnimationManager* m_AnimationManager;
	CAnimationManager3D* m_AnimationManager3D;
	CSoundManager* m_SoundManager;
	CFontManager* m_FontManager;
	CParticleManager* m_ParticleManager;
	CExcelManager* m_ExcelManager;

public:
	bool Init();
	void Update();
	void RenderTarget();

public:
	template <typename T>
	void AddMeshResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_MeshManager->AddResourceChangeCallBack<T>(Obj, Func);
	}

	template <typename T>
	void AddMaterialResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_MaterialManager->AddResourceChangeCallBack<T>(Obj, Func);
	}

	template <typename T>
	void AddTextureResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_TextureManager->AddResourceChangeCallBack<T>(Obj, Func);
	}

	template <typename T>
	void AddAnimation3DResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_AnimationManager3D->AddResourceChangeCallBack<T>(Obj, Func);
	}

	template <typename T>
	void AddSoundResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_SoundManager->AddResourceChangeCallBack<T>(Obj, Func);
	}

	template <typename T>
	void AddParticleResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_ParticleManager->AddResourceChangeCallBack<T>(Obj, Func);
	}

	template <typename T>
	void AddShaderResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_ShaderManager->AddResourceChangeCallBack<T>(Obj, Func);
	}

public:
	 CTextureManager* GetTextureManager() const
	{
		return m_TextureManager;
	}
	 CMaterialManager* GetMaterialManager() const
	{
		return m_MaterialManager;
	}
	 CShaderManager* GetShaderManager() const
	{
		return m_ShaderManager;
	}
	 CParticleManager* GetParticleManager() const
	{
		return m_ParticleManager;
	}
	 CExcelManager* GetExcelManager() const
	 {
		 return m_ExcelManager;
	 }
	 CMeshManager* GetMeshManager() const
	 {
		 return m_MeshManager;
	 }

public:	// =================== Mesh =====================
	bool CreateMesh(Mesh_Type Type, const std::string& Name,
		void* VtxData, int Size, int Count, D3D11_USAGE Usage,
		D3D11_PRIMITIVE_TOPOLOGY Primitive, void* IdxData = nullptr, int IdxSize = 0, int IdxCount = 0,
		D3D11_USAGE IdxUsage = D3D11_USAGE_DEFAULT, DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN,
		class CScene* Scene = nullptr);
	bool CreateNavMesh(class CNavMesh* NavMesh, const std::string& Name, void* VtxData,
		int Size, int Count, D3D11_USAGE Usage, D3D11_PRIMITIVE_TOPOLOGY Primitive,
		void* IdxData, int IdxSize, int IdxCount, D3D11_USAGE IdxUsage, DXGI_FORMAT Fmt, class CScene* Scene = nullptr);
	bool LoadMesh(std::string& OutName, Mesh_Type Type,const TCHAR* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMesh(Mesh_Type Type, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPath(std::string& OutName,Mesh_Type Type,
		const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshFullPath(Mesh_Type Type, const std::string& Name,
		const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshMultibyte(std::string& OutName, Mesh_Type Type, const char* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshMultibyte(Mesh_Type Type, const std::string& Name, const char* FileName,
		const std::string& PathName = MESH_PATH, class CScene* Scene = nullptr);
	bool LoadMeshFullPathMultibyte(std::string& OutName, Mesh_Type Type,
		const char* FullPath, class CScene* Scene = nullptr);
	bool LoadMeshFullPathMultibyte(Mesh_Type Type, const std::string& Name,
		const char* FullPath, class CScene* Scene = nullptr);
	bool SetMeshSkeleton(const std::string& Name, const std::string& SkeletonName);
	bool SetMeshSkeleton(const std::string& Name, class CSkeleton* Skeleton);
	bool ConvertFBXLocalFormatFullPathMultiByte(const char* FullPath);

	class CMesh* FindMesh(const std::string& Name);

	void ReleaseMesh(const std::string& Name);

	// Editor Manager 상에서, 특정 sqc, msh, bne 을 로드할 때 사용하는 함수
	// 저장한 Mesh 이름과 성공 여부 bool 을 리턴한다.
	std::pair<bool, std::string> LoadMeshTextureBoneInfo(const char* ConstMeshFileName, bool IsLastSqcFile = true);
	std::pair<bool, std::string> LoadMeshTextureBoneInfo(class CAnimationSequenceInstance* Instance);
	
public:	// =================== Shader =====================
	template <typename T>
	bool CreateShader(const std::string& Name)
	{
		return m_ShaderManager->CreateShader<T>(Name);
	}
	
	template <typename Origin, typename Instancing>
	bool CreateInstancingShader(Origin* OriginShader, const std::string& InstancingShaderName)
	{
		return m_ShaderManager->CreateInstancingShader<Origin, Instancing>(OriginShader, InstancingShaderName);
	}

	class CShader* FindShader(const std::string& Name);
	class CGraphicShader* FindInstancingShader(class CGraphicShader* Shader);

	void ReleaseShader(const std::string& Name);

	bool CreateConstantBuffer(const std::string& Name, int Size, int Register,
		int ConstantBufferShaderType = (int)Buffer_Shader_Type::All);

	class CConstantBuffer* FindConstantBuffer(const std::string& Name);


public:	// =================== Material =====================
	CMaterial* FindMaterial(const std::string& Name);
	CMaterialConstantBuffer* GetMaterialConstantBuffer()	const;
	CMaterial* LoadMaterialFullPathMultibyte(const char* FullPath);
	void ReleaseMaterial(const std::string& Name);

public:
	template <typename T>
	T* CreateMaterial(const std::string& Name)
	{
		return m_MaterialManager->CreateMaterial<T>(Name);
	}

	template <typename T>
	T* CreateMaterialEmpty()
	{
		return m_MaterialManager->CreateMaterialEmpty<T>();
	}

public:	// =================== Texture =====================
	bool LoadTexture(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath);
	bool LoadTextureFullPath(std::string& OutName, const TCHAR* FullPath);
	bool LoadTextureFullPathMultibyte(const std::string& Name, const char* FullPath);
	bool LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	bool CreateTarget(const std::string& Name, unsigned int Width,
		unsigned int Height, DXGI_FORMAT PixelFormat, bool MultiSample = true,
		DXGI_FORMAT DepthFormat = DXGI_FORMAT_UNKNOWN);
	class CTexture* FindTexture(const std::string& Name);
	bool LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	void ReleaseTexture(const std::string& Name);

public:	// =================== Sequence2D =====================
	bool CreateAnimationSequence2D(const std::string& Name, const std::string& TextureName,
		const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
	bool CreateAnimationSequence2D(const std::string& Name, class CTexture* Texture);
	void AddAnimationSequence2DFrame(const std::string& Name, const Vector2& Start, const Vector2& Size);
	void AddAnimationSequence2DFrame(const std::string& Name, float StartX, float StartY, float Width, float Height);
	bool SaveSequence2DFullPath(const std::string& Name, const char* FullPath);
	bool LoadSequence2DFullPath(std::string& resultName, const char* FullPath, class CScene* Scene = nullptr);
	bool LoadSequence2DFullPath(const char* FullPath, class CScene* Scene = nullptr);
	bool SaveSequence2D(const std::string& Name, const char* FileName, const std::string& PathName = ANIMATION_PATH);
	bool LoadSequence2D(const char* FileName, const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadSequence2D(std::string& resultName, const char* FileName, const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	CAnimationSequence2D* FindAnimationSequence2D(const std::string& Name);
	void ReleaseAnimationSequence2D(const std::string& Name);
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
	void ReleaseSound(const std::string& Name);


public:	// =================== Font =====================
	bool CreateFontFile(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = FONT_PATH);
	bool LoadFont(const std::string& Name, const TCHAR* FontName, int Weight, float FontSize,
		const TCHAR* LocalName, int Stretch = DWRITE_FONT_STRETCH_NORMAL);
	const TCHAR* GetFontFaceName(const std::string& Name);
	const char* GetFontFaceNameMultibyte(const std::string& Name);
	bool CreateFontColor(float r, float g, float b, float a);
	bool CreateFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool CreateFontColor(const Vector4& Color);
	bool CreateFontColor(unsigned int Color);
	ID2D1SolidColorBrush* FindFontColor(float r, float g, float b, float a);
	ID2D1SolidColorBrush* FindFontColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	ID2D1SolidColorBrush* FindFontColor(const Vector4& Color);
	ID2D1SolidColorBrush* FindFontColor(unsigned int Color);
	unsigned int CreateFontColorKey(float r, float g, float b, float a);
	unsigned int CreateFontColorKey(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	unsigned int CreateFontColorKey(const Vector4& Color);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* Text, IDWriteTextFormat* Font, float Width,
		float Height);
	IDWriteTextLayout* CreateTextLayout(const TCHAR* Text, const std::string& FontName, float Width,
		float Height);

	IDWriteFontCollection1* FindFontFile(const std::string& Name);
	IDWriteTextFormat* FindFont(const std::string& Name);


public:	// =================== Particle =====================
	bool CreateParticle(const std::string& Name);
	CParticle* FindParticle(const std::string& Name);
	void ReleaseParticle(const std::string& Name);
	template <typename T>
	T* CreateParticleEmpty()
	{
		return m_ParticleManager->CreateParticleEmpty<T>();
	}
public: // =================== Sequence 3D =====================
	bool LoadAnimationSequenceConvertFBX(const std::string& Name, bool bLoop, struct _tagFbxAnimationClip* pClip);
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		struct _tagFbxAnimationClip* Clip, class CScene* Scene = nullptr);
	bool LoadAnimationSequence(const std::string& Name, bool Loop,
		int StartFrame, int EndFrame, float PlayTime,
		const std::vector<BoneKeyFrame*>& vecFrame, class CScene* Scene = nullptr);
	bool LoadAnimationSequence(bool Loop, const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPath(bool Loop, const std::string& Name, const TCHAR* FullPath, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceMultibyte(bool Loop, const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPathMultibyte(bool Loop, const std::string& Name, const char* FullPath, class CScene* Scene = nullptr);
	bool LoadAnimationSequenceFullPathMultibyteSetOriginFileName(bool Loop, std::string& Name, const char* FullPath);
	CAnimationSequence* FindAnimationSequence(const std::string& Name);
	CAnimationSequence* CreateBasicAnimationSequence(const std::string& Name);
	void ReleaseAnimationSequence3D(class CAnimationSequence* ExistingSequence);
	void ReleaseAnimationSequence3D(const std::string& Name);
	void DeleteSequence3D(const std::string& Name);
	bool EditSequenceClip(class CAnimationSequence* ExistingSequence, const std::string& NewName,
		int StartFrame, int EndFrame, const char* SaveFullPathMultibyte);
	void LoadAniFile();
	bool LoadSkeleton(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPath(const std::string& Name, const TCHAR* FullPath,
		class CScene* Scene = nullptr);
	bool LoadSkeletonMultibyte(const std::string& Name, const char* FileName,
		const std::string& PathName = ANIMATION_PATH,
		class CScene* Scene = nullptr);
	bool LoadSkeletonFullPathMultibyte(const std::string& Name, const char* FullPath,
		class CScene* Scene = nullptr);
	CSkeleton* FindSkeleton(const std::string& Name);
	bool AddSocket(const std::string& SkeletonName, const std::string& BoneName, const std::string& SocketName,
		const Vector3& Offset = Vector3::Zero, const Vector3& OffsetRot = Vector3::Zero);
	void ReleaseSkeleton(const std::string& Name);

public:	// =================== Excel =====================
	bool CreateCSV(const std::string& name);
	CExcelData* FindCSV(const std::string& name);
	bool DeleteCSV(const std::string& name);
	bool SaveCSV(const std::string& name, const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool SaveCSVFullPath(const std::string& name, const char* fullPath);
	bool LoadCSV(const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool LoadCSV(std::string& outName, const char* fileName, const std::string& pathName = EXCEL_PATH);
	bool LoadCSVFullPath(const char* fullPath);
	bool LoadCSVFullPath(std::string& outName, const char* fullPath);

	DECLARE_SINGLE(CResourceManager)
};
