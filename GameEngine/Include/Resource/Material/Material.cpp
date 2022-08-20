
#include "Material.h"
#include "../../PathManager.h"
#include "../ResourceManager.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneResource.h"
#include "../../Render/RenderManager.h"
#include "../../Render/RenderState.h"
#include "../Shader/MaterialConstantBuffer.h"
#include "MaterialManager.h"
#include <filesystem>

CMaterial::CMaterial() :
	m_BaseColor(Vector4::White),
	m_AmbientColor(Vector4(0.2f, 0.2f, 0.2f, 1.f)),
	m_SpecularColor(Vector4::White),
	m_EmissiveColor(Vector4::Black),
	m_Animation3D(false),
	m_SpecularTex(false),
	m_EmissiveTex(false),
	m_RecieveDecal(false),
	m_Bump(false),
	m_Metallic(false),
	m_UVScale(Vector2(1.f, 1.f)),
	m_Scene(nullptr),
	m_CBuffer(nullptr),
	m_Opacity(1.f),
	m_RenderStateArray{},
	m_ShaderParams{}
{
	SetTypeID<CMaterial>();
}

CMaterial::CMaterial(const CMaterial& Material)
{
	*this = Material;

	m_RefCount = 0;

	m_CBuffer = Material.m_CBuffer->Clone();

	m_Scene = nullptr;

	m_RenderCallback.clear();
}

CMaterial::~CMaterial()
{
	auto    iter = m_RenderCallback.begin();
	auto    iterEnd = m_RenderCallback.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	SAFE_DELETE(m_CBuffer);
}

void CMaterial::CreateConstantBuffer()
{
	SAFE_DELETE(m_CBuffer);

	m_CBuffer = new CMaterialConstantBuffer;

	m_CBuffer->Init();

	m_CBuffer->SetBaseColor(m_BaseColor);
	m_CBuffer->SetAmbientColor(m_AmbientColor);
	m_CBuffer->SetSpecularColor(m_SpecularColor);
	m_CBuffer->SetEmissiveColor(m_EmissiveColor);
	m_CBuffer->SetOpacity(m_Opacity);
	m_CBuffer->SetMetallic(m_Metallic);
	m_CBuffer->SetUVScale(m_UVScale);

	m_CBuffer->UpdateCBuffer();
}

void CMaterial::EnableBump()
{
	m_Bump = true;

	m_CBuffer->SetBump(true);
}

void CMaterial::EnableAnimation3D()
{
	m_Animation3D = true;

	m_CBuffer->SetAnimation3D(true);
}

void CMaterial::EnableSpecularTex()
{
	m_SpecularTex = true;

	m_CBuffer->SetSpecularTex(true);
}

void CMaterial::EnableEmissiveTex()
{
	m_EmissiveTex = true;

	m_CBuffer->SetEmissiveTex(true);
}

void CMaterial::EnableDecal(bool Decal)
{
	m_RecieveDecal = Decal;

	m_CBuffer->SetReceiveDecal(Decal);
}

void CMaterial::SetRenderState(CRenderState* State)
{
	m_RenderStateArray[(int)State->GetType()] = State;
} 

void CMaterial::SetRenderState(const std::string& Name)
{
	CRenderState* State = CRenderManager::GetInst()->FindRenderState(Name);
	m_RenderStateArray[(int)State->GetType()] = State;
}

void CMaterial::SetTransparency(bool Enable)
{
	if (Enable)
	{
		m_RenderStateArray[(int)RenderState_Type::Blend] = CRenderManager::GetInst()->FindRenderState("AlphaBlend");
	}
	else
	{
		m_RenderStateArray[(int)RenderState_Type::Blend] = nullptr;
	}
}

void CMaterial::SetOpacity(float Opacity)
{
	m_Opacity = Opacity;

	if (m_Opacity < 0.f)
		m_Opacity = 0.f;

	else if (m_Opacity > 1.f)
		m_Opacity = 1.f;

	if (m_CBuffer)
	{
		m_CBuffer->SetOpacity(m_Opacity);
	}
}

void CMaterial::AddOpacity(float Opacity)
{
	m_Opacity += Opacity;

	if (m_Opacity < 0.f)
		m_Opacity = 0.f;

	else if (m_Opacity > 1.f)
		m_Opacity = 1.f;

	if (m_CBuffer)
	{
		m_CBuffer->SetOpacity(m_Opacity);
	}
}

void CMaterial::SetBaseColor(const Vector4& Color)
{
	m_BaseColor = Color;

	if (m_CBuffer)
	{
		m_CBuffer->SetBaseColor(m_BaseColor);
	}
}

void CMaterial::SetBaseColor(float r, float g, float b, float a)
{
	m_BaseColor = Vector4(r, g, b, a);

	if (m_CBuffer)
	{
		m_CBuffer->SetBaseColor(m_BaseColor);
	}
}

void CMaterial::SetAmbientColor(const Vector4& Color)
{
	m_AmbientColor = Color;

	if (m_CBuffer)
	{
		m_CBuffer->SetAmbientColor(m_AmbientColor);
	}
}

void CMaterial::SetAmbientColor(float r, float g, float b, float a)
{
	m_AmbientColor = Vector4(r, g, b, a);

	if (m_CBuffer)
	{
		m_CBuffer->SetAmbientColor(m_AmbientColor);
	}
}

void CMaterial::SetSpecularColor(const Vector4& Color)
{
	m_SpecularColor = Color;

	if (m_CBuffer)
	{
		m_CBuffer->SetSpecularColor(m_SpecularColor);
	}
}

void CMaterial::SetSpecularColor(float r, float g, float b, float a)
{
	m_SpecularColor = Vector4(r, g, b, a);

	if (m_CBuffer)
	{
		m_CBuffer->SetSpecularColor(m_SpecularColor);
	}
}

void CMaterial::SetEmissiveColor(const Vector4& Color)
{
	m_EmissiveColor = Color;

	if (m_CBuffer)
	{
		m_CBuffer->SetEmissiveColor(m_EmissiveColor);
	}
}

void CMaterial::SetEmissiveColor(float r, float g, float b, float a)
{
	m_EmissiveColor = Vector4(r, g, b, a);

	if (m_CBuffer)
	{
		m_CBuffer->SetEmissiveColor(m_EmissiveColor);
	}
}

void CMaterial::SetSpecularPower(float Power)
{
	m_SpecularColor.w = Power;

	if (m_CBuffer)
	{
		m_CBuffer->SetSpecularColor(m_SpecularColor);
	}
}

void CMaterial::SetMetallic(bool Metallic)
{
	m_Metallic = Metallic;

	if (m_CBuffer)
	{
		m_CBuffer->SetMetallic(Metallic);
	}
}

void CMaterial::SetUVScale(const Vector2& UVScale)
{
	m_UVScale = UVScale;

	if (m_CBuffer)
	{
		m_CBuffer->SetUVScale(m_UVScale);
	}
}

void CMaterial::AddTexture(int Register, int ShaderType, const std::string& Name, CTexture* Texture)
{
	m_TextureInfo.push_back(MaterialTextureInfo());

	int	Index = (int)m_TextureInfo.size() - 1;

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;
}

void CMaterial::AddTexture(int Register, int ShaderType, const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTexture(Name, FileName, PathName))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTexture(Name, FileName, PathName))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo.push_back(MaterialTextureInfo());

	int	Index = (int)m_TextureInfo.size() - 1;

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;

	// Full Path 정보 저장
	const PathInfo* FoundPath = CPathManager::GetInst()->FindPath(PathName);

	char FullPathMultibyte[MAX_PATH] = {};
	TCHAR FullPath[MAX_PATH] = {};

	if (FoundPath)
		lstrcpy(FullPath, FoundPath->Path);

	lstrcat(FullPath, FileName);

#ifdef UNICODE
	// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1,
		FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
	strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::AddTextureFullPath(int Register, int ShaderType,
	const std::string& Name, const TCHAR* FullPath)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTextureFullPath(Name, FullPath))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTextureFullPath(Name, FullPath))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo.push_back(MaterialTextureInfo());

	int	Index = (int)m_TextureInfo.size() - 1;

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;

	char FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE
	// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1,
		FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
	strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::AddTexture(int Register, int ShaderType,
	const std::string& Name, const std::vector<TCHAR*>& vecFileName,
	const std::string& PathName)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTexture(Name, vecFileName, PathName))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTexture(Name, vecFileName, PathName))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo.push_back(MaterialTextureInfo());

	int	Index = (int)m_TextureInfo.size() - 1;

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;

	// Full Path 정보 저장
	const PathInfo* FoundPath = CPathManager::GetInst()->FindPath(PathName);

	char FullPathMultibyte[MAX_PATH] = {};
	TCHAR FullPath[MAX_PATH] = {};

	if (FoundPath)
		lstrcpy(FullPath, FoundPath->Path);

	if (vecFileName.size() > 0)
		lstrcat(FullPath, vecFileName[0]);

#ifdef UNICODE
	// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1,
		FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
	strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::AddTextureArrayFullPath(int Register, int ShaderType,
	const std::string& Name, const std::vector<TCHAR*>& vecFullPath)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTextureArrayFullPath(Name, vecFullPath))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTextureArrayFullPath(Name, vecFullPath))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo.push_back(MaterialTextureInfo());

	int	Index = (int)m_TextureInfo.size() - 1;

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;
}

void CMaterial::AddTextureArray(int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTextureArray(Name, vecFileName, PathName))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTextureArray(Name, vecFileName, PathName))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo.push_back(MaterialTextureInfo());

	int	Index = (int)m_TextureInfo.size() - 1;

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;

	// Full Path 정보 저장
	const PathInfo* FoundPath = CPathManager::GetInst()->FindPath(PathName);

	char FullPathMultibyte[MAX_PATH] = {};
	TCHAR FullPath[MAX_PATH] = {};

	if (FoundPath)
		lstrcpy(FullPath, FoundPath->Path);

	if (vecFileName.size() > 0)
		lstrcat(FullPath, vecFileName[0]);

#ifdef UNICODE
	// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1,
		FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
	strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::SetTexture(int Index, int Register, int ShaderType, const std::string& Name, CTexture* Texture)
{
	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;
}

void CMaterial::SetTexture(int Index, int Register, int ShaderType, const std::string& Name, 
	const TCHAR* FileName, const std::string& PathName)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTexture(Name, FileName, PathName))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTexture(Name, FileName, PathName))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = Texture;
	m_TextureInfo[Index].ShaderType = ShaderType;

	// Full Path 정보 저장
	const PathInfo* FoundPath = CPathManager::GetInst()->FindPath(PathName);

	char FullPathMultibyte[MAX_PATH] = {};
	TCHAR FullPath[MAX_PATH] = {};

	if (FoundPath)
		lstrcpy(FullPath, FoundPath->Path);

	lstrcat(FullPath, FileName);

#ifdef UNICODE
	// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1,
		FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
	strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::SetTextureFullPath(int Index, int Register, int ShaderType,
	const std::string& Name, const TCHAR* FullPath)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTextureFullPath(
			Name, FullPath))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTextureFullPath(Name,
			FullPath))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = CResourceManager::GetInst()->FindTexture(Name);
	m_TextureInfo[Index].ShaderType = ShaderType;

	// Full Path 정보 저장
	char FullPathMultibyte[MAX_PATH] = {};

#ifdef UNICODE
	// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1,
		FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
	strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::SetTexture(int Index, int Register,
	int ShaderType, const std::string& Name,
	const std::vector<TCHAR*>& vecFileName,
	const std::string& PathName)
{
	/*if (!CResourceManager::GetInst()->LoadTexture(Name, FileName, PathName))
		return;

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = CResourceManager::GetInst()->FindTexture(Name);
	m_TextureInfo[Index].ShaderType = ShaderType;*/
}

void CMaterial::SetTextureArrayFullPath(int Index, int Register,
	int ShaderType, const std::string& Name,
	const std::vector<TCHAR*>& vecFullPath)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTextureArrayFullPath(
			Name, vecFullPath))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTextureArrayFullPath(Name,
			vecFullPath))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = CResourceManager::GetInst()->FindTexture(Name);
	m_TextureInfo[Index].ShaderType = ShaderType;

	// Full Path 정보 저장
	char FullPathMultibyte[MAX_PATH] = {};

	if (vecFullPath.size() > 0)
	{

#ifdef UNICODE
		// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, vecFullPath[0], -1, nullptr, 0, nullptr, nullptr);

		WideCharToMultiByte(CP_ACP, 0, vecFullPath[0], -1,
			FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
		strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE
	}

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::SetTextureArray(int Index, int Register,
	int ShaderType, const std::string& Name,
	const std::vector<TCHAR*>& vecFileName,
	const std::string& PathName)
{
	CTexture* Texture = nullptr;

	if (!m_Scene)
	{
		if (!CResourceManager::GetInst()->LoadTextureArray(
			Name, vecFileName, PathName))
			return;

		Texture = CResourceManager::GetInst()->FindTexture(Name);
	}

	else
	{
		if (!m_Scene->GetResource()->LoadTextureArray(Name,
			vecFileName, PathName))
			return;

		Texture = m_Scene->GetResource()->FindTexture(Name);
	}

	m_TextureInfo[Index].Register = Register;
	m_TextureInfo[Index].Name = Name;
	m_TextureInfo[Index].Texture = CResourceManager::GetInst()->FindTexture(Name);
	m_TextureInfo[Index].ShaderType = ShaderType;

	// Full Path 정보 저장
	char FullPathMultibyte[MAX_PATH] = {};

	const PathInfo* FoundPath = CPathManager::GetInst()->FindPath(PathName);

	TCHAR FullPath[MAX_PATH] = {};

	if (FoundPath)
		lstrcpy(FullPath, FoundPath->Path);

	if (vecFileName.size() > 0)
		lstrcat(FullPath, vecFileName[0]);

#ifdef UNICODE
	// 유니코드 문자열을 멀티바이트 문자열로 변환한다.
	int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FullPath, -1,
		FullPathMultibyte, MAX_PATH, nullptr, nullptr);
#else
	strcpy_s(FullPathMultibyte, FullPath);
#endif // UNICODE

	m_TextureInfo[Index].SavedFullPath = FullPathMultibyte;
}

void CMaterial::ClearTexture()
{
	m_TextureInfo.clear();
}

void CMaterial::SetTextureInfoResource(int Index, CTexture* Texture)
{
	if (!Texture)
		return;

	if (Index >= m_TextureInfo.size())
		return;

	m_TextureInfo[Index].Texture = Texture;
}

void CMaterial::SetTextureInfoRegister(int Index, int Register)
{
	if (Index >= m_TextureInfo.size())
		return;

	m_TextureInfo[Index].Register = Register;
}

void CMaterial::SetPaperBurn(bool Enable)
{
	m_CBuffer->SetPaperBurn(Enable);
}

bool CMaterial::CheckMaterial(CMaterial* Material)
{
	size_t Size = m_TextureInfo.size();
	size_t Size1 = Material->m_TextureInfo.size();

	if (Size != Size1)
	{
		return false;
	}

	for (size_t i = 0; i < Size; ++i)
	{
		for (size_t j = 0; j < Size1; ++j)
		{
			if (m_TextureInfo[i].Texture != m_TextureInfo[j].Texture)
			{
				return false;
			}
		}
	}
	return true;
}

void CMaterial::SetShader(const std::string& Name)
{
	CGraphicShader* Shader = (CGraphicShader*)CResourceManager::GetInst()->FindShader(Name);
	SetShader(Shader);
}

void CMaterial::SetShader(CGraphicShader* Shader)
{
	if (!Shader)
		return;

	m_PrevShader = m_Shader;
	m_Shader = Shader;

	UpdateShaderParams();
}

void CMaterial::RevertShader()
{
	if (!m_PrevShader)
	{
		m_Shader = (CGraphicShader*)CResourceManager::GetInst()->FindShader("Standard3DShader");
	}
	else
	{
		m_Shader = m_PrevShader;
		m_PrevShader = nullptr;
	}

	UpdateShaderParams();
}

void CMaterial::Render()
{
	if (m_Shader)
	{
		m_Shader->SetShader();
		m_Shader->SetShaderParams(m_ShaderParams);
	}

	if (m_CBuffer)
	{
		m_CBuffer->SetBaseColor(m_BaseColor);
		m_CBuffer->SetAmbientColor(m_AmbientColor);
		m_CBuffer->SetSpecularColor(m_SpecularColor);
		m_CBuffer->SetEmissiveColor(m_EmissiveColor);
		m_CBuffer->SetOpacity(m_Opacity);
		m_CBuffer->SetMetallic(m_Metallic);
		m_CBuffer->SetUVScale(m_UVScale);

		m_CBuffer->UpdateCBuffer();
	}

	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		if (m_RenderStateArray[i])
			m_RenderStateArray[i]->SetState();
	}

	size_t	Size = m_TextureInfo.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_TextureInfo[i].Texture->SetShader(m_TextureInfo[i].Register, m_TextureInfo[i].ShaderType, 0);
	}

	auto    iter = m_RenderCallback.begin();
	auto    iterEnd = m_RenderCallback.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Func();
	}
}

void CMaterial::RenderTexture()
{
	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		if (m_RenderStateArray[i])
			m_RenderStateArray[i]->SetState();
	}

	size_t	Size = m_TextureInfo.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_TextureInfo[i].Texture->SetShader(m_TextureInfo[i].Register, m_TextureInfo[i].ShaderType, 0);
	}

	auto    iter = m_RenderCallback.begin();
	auto    iterEnd = m_RenderCallback.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Func();
	}
}

void CMaterial::UpdateCBuffer()
{
	if (m_CBuffer)
	{
		m_CBuffer->SetBaseColor(m_BaseColor);
		m_CBuffer->SetAmbientColor(m_AmbientColor);
		m_CBuffer->SetSpecularColor(m_SpecularColor);
		m_CBuffer->SetEmissiveColor(m_EmissiveColor);
		m_CBuffer->SetOpacity(m_Opacity);
		m_CBuffer->SetMetallic(m_Metallic);
		m_CBuffer->SetUVScale(m_UVScale);

		m_CBuffer->UpdateCBuffer();
	}
}

void CMaterial::Reset()
{
	size_t	Size = m_TextureInfo.size();

	m_Shader->ResetShaderResources();

	for (size_t i = 0; i < Size; ++i)
	{
		m_TextureInfo[i].Texture->ResetShader(m_TextureInfo[i].Register, m_TextureInfo[i].ShaderType, 0);
	}

	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		if (m_RenderStateArray[i])
			m_RenderStateArray[i]->ResetState();
	}
}

CMaterial* CMaterial::Clone()	const
{
	return new CMaterial(*this);
}

bool CMaterial::Save(FILE* File)
{
	// 새로 추가
	CRef::Save(File);

	std::string	ShaderName ;

	if (m_Shader)
	{
		ShaderName = m_Shader->GetName();
	}

	int	Length = (int)ShaderName.length();

	fwrite(&Length, sizeof(int), 1, File);
	fwrite(ShaderName.c_str(), sizeof(char), Length, File);

	fwrite(&m_BaseColor, sizeof(Vector4), 1, File);
	fwrite(&m_AmbientColor, sizeof(Vector4), 1, File);
	fwrite(&m_SpecularColor, sizeof(Vector4), 1, File);
	fwrite(&m_EmissiveColor, sizeof(Vector4), 1, File);
	fwrite(&m_Opacity, sizeof(float), 1, File);
	fwrite(&m_Animation3D, sizeof(bool), 1, File);
	fwrite(&m_SpecularTex, sizeof(bool), 1, File);
	fwrite(&m_EmissiveTex, sizeof(bool), 1, File);
	fwrite(&m_Bump, sizeof(bool), 1, File);
	fwrite(&m_Metallic, sizeof(bool), 1, File);
	fwrite(&m_UVScale, sizeof(Vector2), 1, File);

	// 이 부분까지는 문제 X
	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		bool	StateEnable = false;

		if (m_RenderStateArray[i])
			StateEnable = true;

		fwrite(&StateEnable, sizeof(bool), 1, File);

		if (m_RenderStateArray[i])
		{
			std::string	StateName = m_RenderStateArray[i]->GetName();

			int	Length = (int)StateName.length();

			fwrite(&Length, sizeof(int), 1, File);
			fwrite(StateName.c_str(), sizeof(char), Length, File);
		}
	}

	int	TextureCount = (int)m_TextureInfo.size();
	fwrite(&TextureCount, sizeof(int), 1, File);

	for (int i = 0; i < TextureCount; ++i)
	{
		int	Length = (int)m_TextureInfo[i].Name.length();

		fwrite(&Length, sizeof(int), 1, File);
		fwrite(m_TextureInfo[i].Name.c_str(), sizeof(char), Length, File);

		fwrite(&m_TextureInfo[i].SamplerType, sizeof(Sampler_Type), 1, File);
		fwrite(&m_TextureInfo[i].Register, sizeof(int), 1, File);
		fwrite(&m_TextureInfo[i].ShaderType, sizeof(int), 1, File);

		// Saved FullPath 정보 저장
		int	FullPathLength = (int)m_TextureInfo[i].SavedFullPath.length();
		fwrite(&FullPathLength, sizeof(int), 1, File);
		fwrite(m_TextureInfo[i].SavedFullPath.c_str(), sizeof(char), FullPathLength, File);

		m_TextureInfo[i].Texture->Save(File);
	}

	// Shader Parameter 저장
	fwrite(&m_ShaderParams, sizeof(ShaderParams), 1, File);

	return true;
}

bool CMaterial::Load(FILE* File)
{
	// 새로 추가
	CRef::Load(File);

	CreateConstantBuffer();

	char	ShaderName[256] = {};

	int	Length = 0;

	fread(&Length, sizeof(int), 1, File);
	fread(ShaderName, sizeof(char), Length, File);

	SetShader(ShaderName);

	fread(&m_BaseColor, sizeof(Vector4), 1, File);
	fread(&m_AmbientColor, sizeof(Vector4), 1, File);
	fread(&m_SpecularColor, sizeof(Vector4), 1, File);
	fread(&m_EmissiveColor, sizeof(Vector4), 1, File);
	fread(&m_Opacity, sizeof(float), 1, File);
	fread(&m_Animation3D, sizeof(bool), 1, File);
	fread(&m_SpecularTex, sizeof(bool), 1, File);
	fread(&m_EmissiveTex, sizeof(bool), 1, File);
	fread(&m_Bump, sizeof(bool), 1, File);
	fread(&m_Metallic, sizeof(bool), 1, File);
	fread(&m_UVScale, sizeof(Vector2), 1, File);

	if (m_SpecularColor.w == 0.f)
	{
		m_SpecularColor.w = 1.f;
	}

	m_CBuffer->SetAnimation3D(m_Animation3D);
	m_CBuffer->SetBump(m_Bump);
	m_CBuffer->SetSpecularTex(m_SpecularTex);
	m_CBuffer->SetEmissiveTex(m_EmissiveTex);

	m_CBuffer->SetBaseColor(m_BaseColor);
	m_CBuffer->SetAmbientColor(m_AmbientColor);
	m_CBuffer->SetSpecularColor(m_SpecularColor);
	m_CBuffer->SetEmissiveColor(m_EmissiveColor);
	m_CBuffer->SetOpacity(m_Opacity);
	m_CBuffer->SetMetallic(m_Metallic);
	m_CBuffer->SetUVScale(m_UVScale);

	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		bool	StateEnable = true;
		fread(&StateEnable, sizeof(bool), 1, File);

		if (StateEnable)
		{
			char	StateName[256] = {};
			
			int  Length = 0;

			fread(&Length, sizeof(int), 1, File);
			fread(StateName, sizeof(char), Length, File);

			m_RenderStateArray[i] = CRenderManager::GetInst()->FindRenderState(StateName);
		}
	}

	int	TextureCount = 0;
	fread(&TextureCount, sizeof(int), 1, File);

	for (int i = 0; i < TextureCount; ++i)
	{
		m_TextureInfo.push_back(MaterialTextureInfo());

		Length = 0;

		char	TextureName[256] = {};

		fread(&Length, sizeof(int), 1, File);
		fread(TextureName, sizeof(char), Length, File);

		m_TextureInfo[i].Name = TextureName;

		fread(&m_TextureInfo[i].SamplerType, sizeof(Sampler_Type), 1, File);
		fread(&m_TextureInfo[i].Register, sizeof(int), 1, File);
		fread(&m_TextureInfo[i].ShaderType, sizeof(int), 1, File);

		// Saved FullPath 정보 저장
		int	FullPathLength = 0;
		fread(&FullPathLength, sizeof(int), 1, File);

		char	TextureSavedFullPath[256] = {};
		fread(TextureSavedFullPath, sizeof(char), FullPathLength, File);
		m_TextureInfo[i].SavedFullPath = TextureSavedFullPath;

		int	TexNameLength = 0;
		fread(&TexNameLength, sizeof(int), 1, File);
		char	TexName[256] = {};
		fread(TexName, sizeof(char), TexNameLength, File);

		Image_Type	ImageType;
		fread(&ImageType, sizeof(Image_Type), 1, File);

		int	InfoCount = 0;

		fread(&InfoCount, sizeof(int), 1, File);

		std::vector<std::wstring>	vecFullPath;
		std::vector<std::wstring>	vecFileName;
		std::string	PathName;

		for (int i = 0; i < InfoCount; ++i)
		{
			int	PathSize = 0;

			fread(&PathSize, sizeof(int), 1, File);

			TCHAR	FullPath[MAX_PATH] = {};
			fread(FullPath, sizeof(TCHAR), PathSize, File);
			vecFullPath.push_back(FullPath);

			fread(&PathSize, sizeof(int), 1, File);

			TCHAR	TexFileName[MAX_PATH] = {};
			fread(TexFileName, sizeof(TCHAR), PathSize, File);
			vecFileName.push_back(TexFileName);

			fread(&PathSize, sizeof(int), 1, File);

			char	TexPathName[MAX_PATH] = {};
			fread(TexPathName, sizeof(char), PathSize, File);

			PathName = TexPathName;
		}

		switch (ImageType)
		{
		case Image_Type::Atlas:
			if (vecFileName.size() == 1)
			{
				if (m_Scene)
				{
					m_Scene->GetResource()->LoadTexture(TexName, vecFileName[0].c_str(), PathName);
				}
				else
				{
					CResourceManager::GetInst()->LoadTexture(TexName, vecFileName[0].c_str(), PathName);
				}
			}
			else
			{
			}
			break;
		case Image_Type::Frame:
			if (vecFileName.size() == 1)
			{
			}
			else
			{
			}
			break;
		case Image_Type::Array:
			if (vecFileName.size() == 1)
			{
			}
			else
			{
			}
			break;
		}

		if (m_Scene)
		{
			m_TextureInfo[i].Texture = m_Scene->GetResource()->FindTexture(TexName);
		}

		else
		{
			m_TextureInfo[i].Texture = CResourceManager::GetInst()->FindTexture(TexName);
		}
	}

	fread(&m_ShaderParams, sizeof(ShaderParams), 1, File);

	return true;
}

bool CMaterial::SaveMaterial(FILE* File)
{
	// 총 4번의 fwrite
	// Ref::Save, MaterialSaveLoadStruct, TextureCount TextureSaveLoadStruct

	// 새로 추가
	CRef::Save(File);

	MaterialSaveLoadStruct SaveStruct;

	std::string	ShaderName;

	if (m_Shader)
		ShaderName = m_Shader->GetName();

	int	Length = (int)ShaderName.length();
	SaveStruct.ShaderNameLength = Length;
	//fwrite(&Length, sizeof(int), 1, File);

	// fwrite(ShaderName.c_str(), sizeof(char), Length, File);
	strcpy_s(SaveStruct.ShaderName, ShaderName.c_str());

	// fwrite(&m_BaseColor, sizeof(Vector4), 1, File);
	SaveStruct.BaseColor = m_BaseColor; //
	// fwrite(&m_AmbientColor, sizeof(Vector4), 1, File);
	SaveStruct.AmbientColor = m_AmbientColor;
	// fwrite(&m_SpecularColor, sizeof(Vector4), 1, File);
	SaveStruct.SpecularColor = m_SpecularColor;
	// fwrite(&m_EmissiveColor, sizeof(Vector4), 1, File);
	SaveStruct.EmissiveColor = m_EmissiveColor;
	// fwrite(&m_Opacity, sizeof(float), 1, File);
	SaveStruct.Opacity = m_Opacity;
	// fwrite(&m_Animation3D, sizeof(bool), 1, File);
	SaveStruct.Animation3D = m_Animation3D;
	// fwrite(&m_SpecularTex, sizeof(bool), 1, File);
	SaveStruct.SpecularTex = m_SpecularTex;
	// fwrite(&m_EmissiveTex, sizeof(bool), 1, File);
	SaveStruct.EmissiveTex = m_EmissiveTex;
	// fwrite(&m_Bump, sizeof(bool), 1, File);
	SaveStruct.Bump = m_Bump;
	SaveStruct.Metallic = m_Metallic;
	SaveStruct.ShaderParams = m_ShaderParams;
	SaveStruct.UVScale = m_UVScale;

	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		bool	StateEnable = false;

		if (m_RenderStateArray[i])
			StateEnable = true;

		SaveStruct.RenderStateSaveLoad[i].StateEnable = StateEnable;

		if (StateEnable)
		{
			std::string	StateName = m_RenderStateArray[i]->GetName();

			int	Length = (int)StateName.length();

			SaveStruct.RenderStateSaveLoad[i].StateNameLength = Length;
			strcpy_s(SaveStruct.RenderStateSaveLoad[i].StateName, StateName.c_str());
		}
	}

	fwrite(&SaveStruct, sizeof(MaterialSaveLoadStruct), 1, File);

	int	TextureCount = (int)m_TextureInfo.size();
	fwrite(&TextureCount, sizeof(int), 1, File);

	for (int i = 0; i < TextureCount; ++i)
	{
		TextureSaveLoadStruct TextureStruct;

		int	Length = (int)m_TextureInfo[i].Name.length();
		TextureStruct.TextureNameLength = Length;

		strcpy_s(TextureStruct.TextureName, m_TextureInfo[i].Name.c_str());
		TextureStruct.SamplerType = m_TextureInfo[i].SamplerType;
		TextureStruct.Register = m_TextureInfo[i].Register;
		TextureStruct.ShaderType = m_TextureInfo[i].ShaderType;

		// Saved FullPath 정보 저장
		int	FullPathLength = (int)m_TextureInfo[i].SavedFullPath.length();
		TextureStruct.SaveFullPathLength = FullPathLength;

		strcpy_s(TextureStruct.SaveFullPathInfo, m_TextureInfo[i].SavedFullPath.c_str());

		// Texture Struct 정보 저장하기
		fwrite(&TextureStruct, sizeof(TextureSaveLoadStruct), 1, File);

		// Texture 정보 저장
		m_TextureInfo[i].Texture->Save(File);
	}

	return true;
}

bool CMaterial::LoadMaterial(FILE* File)
{
	// 새로 추가
	CRef::Load(File);

	CreateConstantBuffer();

	MaterialSaveLoadStruct SaveStruct;
	fread(&SaveStruct, sizeof(MaterialSaveLoadStruct), 1, File);

	char	ShaderName[256] = {};

	int	Length = 0;

	SetShader(SaveStruct.ShaderName);

	m_BaseColor = SaveStruct.BaseColor;
	m_AmbientColor = SaveStruct.AmbientColor;
	m_SpecularColor = SaveStruct.SpecularColor;
	m_EmissiveColor = SaveStruct.EmissiveColor;
	m_Opacity = SaveStruct.Opacity;
	m_Animation3D = SaveStruct.Animation3D;
	m_SpecularTex = SaveStruct.SpecularTex;
	m_EmissiveTex = SaveStruct.EmissiveTex;
	m_Bump = SaveStruct.Bump;
	m_Metallic = SaveStruct.Metallic;
	m_ShaderParams = SaveStruct.ShaderParams;
	m_UVScale = SaveStruct.UVScale;

	if (m_SpecularColor.w == 0.f)
	{
		m_SpecularColor.w = 1.f;
	}

	m_CBuffer->SetAnimation3D(m_Animation3D);
	m_CBuffer->SetBump(m_Bump);
	m_CBuffer->SetSpecularTex(m_SpecularTex);
	m_CBuffer->SetEmissiveTex(m_EmissiveTex);

	m_CBuffer->SetBaseColor(m_BaseColor);
	m_CBuffer->SetAmbientColor(m_AmbientColor);
	m_CBuffer->SetSpecularColor(m_SpecularColor);
	m_CBuffer->SetEmissiveColor(m_EmissiveColor);
	m_CBuffer->SetOpacity(m_Opacity);
	m_CBuffer->SetMetallic(m_Metallic);
	m_CBuffer->SetUVScale(m_UVScale);

	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		bool	StateEnable = SaveStruct.RenderStateSaveLoad[i].StateEnable;

		if (StateEnable)
		{
			m_RenderStateArray[i] = CRenderManager::GetInst()->FindRenderState(SaveStruct.RenderStateSaveLoad[i].StateName);
		}
	}

	int	TextureCount = 0;
	fread(&TextureCount, sizeof(int), 1, File);

	for (int i = 0; i < TextureCount; ++i)
	{
		m_TextureInfo.push_back(MaterialTextureInfo());

		TextureSaveLoadStruct TextureLoadStruct;

		fread(&TextureLoadStruct, sizeof(TextureSaveLoadStruct), 1, File);

		m_TextureInfo[i].Name = TextureLoadStruct.TextureName;
		m_TextureInfo[i].SamplerType = TextureLoadStruct.SamplerType;
		m_TextureInfo[i].Register = TextureLoadStruct.Register;
		m_TextureInfo[i].ShaderType = TextureLoadStruct.ShaderType;
		m_TextureInfo[i].SavedFullPath = TextureLoadStruct.SaveFullPathInfo;

		// 이후는 Texture Load 파트 
		int	TexNameLength = 0;
		fread(&TexNameLength, sizeof(int), 1, File);
		char	TexName[256] = {};
		fread(TexName, sizeof(char), TexNameLength, File);

		Image_Type	ImageType;
		fread(&ImageType, sizeof(Image_Type), 1, File);

		int	InfoCount = 0;

		fread(&InfoCount, sizeof(int), 1, File);

		std::vector<std::wstring>	vecFullPath;
		std::vector<std::wstring>	vecFileName;
		std::string	PathName;

		for (int i = 0; i < InfoCount; ++i)
		{
			int	PathSize = 0;

			fread(&PathSize, sizeof(int), 1, File);

			TCHAR	FullPath[MAX_PATH] = {};
			fread(FullPath, sizeof(TCHAR), PathSize, File);
			vecFullPath.push_back(FullPath);

			fread(&PathSize, sizeof(int), 1, File);

			TCHAR	TexFileName[MAX_PATH] = {};
			fread(TexFileName, sizeof(TCHAR), PathSize, File);
			vecFileName.push_back(TexFileName);

			fread(&PathSize, sizeof(int), 1, File);

			char	TexPathName[MAX_PATH] = {};
			fread(TexPathName, sizeof(char), PathSize, File);

			PathName = TexPathName;
		}

		switch (ImageType)
		{
		case Image_Type::Atlas:
			if (vecFileName.size() == 1)
			{
				if (m_Scene)
				{
					m_Scene->GetResource()->LoadTexture(TexName, vecFileName[0].c_str(), PathName);
				}
				else
				{
					CResourceManager::GetInst()->LoadTexture(TexName, vecFileName[0].c_str(), PathName);
				}
			}
			else
			{
			}
			break;
		case Image_Type::Frame:
			if (vecFileName.size() == 1)
			{
			}
			else
			{
			}
			break;
		case Image_Type::Array:
			if (vecFileName.size() == 1)
			{
			}
			else
			{
			}
			break;
		}

		if (m_Scene)
		{
			m_TextureInfo[i].Texture = m_Scene->GetResource()->FindTexture(TexName);
		}

		else
		{
			m_TextureInfo[i].Texture = CResourceManager::GetInst()->FindTexture(TexName);
		}
	}


	return true;
}

bool CMaterial::SaveFullPath(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "wb");

	if (!File)
	{
		assert(false);
		return false;
	}

	bool Result = SaveMaterial(File);

	fclose(File);

	return Result;
}

bool CMaterial::LoadFullPath(const char* FullPath)
{
	FILE* File = nullptr;

	fopen_s(&File, FullPath, "rb");

	if (!File)
	{
		assert(false);
		return false;
	}

	bool Result = LoadMaterial(File);

	fclose(File);

	return Result;
}

void CMaterial::UpdateShaderParams()
{
	m_ShaderParams.Type = m_Shader->GetTypeID();
}

void CMaterial::SetShaderParams(const ShaderParams& Params)
{
	m_ShaderParams = Params;
}

