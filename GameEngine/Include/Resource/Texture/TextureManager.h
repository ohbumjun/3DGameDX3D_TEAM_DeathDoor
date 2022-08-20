#pragma once

#include "../../GameInfo.h"

class CTextureManager
{
	friend class CResourceManager;

private:
	CTextureManager();
	~CTextureManager();

private:
	std::unordered_map<std::string, CSharedPtr<class CTexture>>	m_mapTexture;
	std::unordered_map<std::string, ID3D11SamplerState*>	m_mapSampler;
	class CWidgetConstantBuffer* m_CBuffer;
	std::function<void()> m_ChangeCallBack;

public :
	const std::unordered_map<std::string, CSharedPtr<class CTexture>>& GetMapTexture() const
	{
		return m_mapTexture;
	}

public:
	bool Init();
	void RenderTarget(class CMesh* Mesh, class CShader* Shader);

	bool LoadTexture(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath);
	bool LoadTextureFullPath(std::string& OutName, const TCHAR* FullPath);
	bool LoadTextureFullPathMultibyte(const std::string& Name, const char* FullPath);
	bool LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	bool CreateTarget(const std::string& Name, unsigned int Width,
		unsigned int Height, DXGI_FORMAT PixelFormat, bool MultiSample = true, DXGI_FORMAT DepthFormat = DXGI_FORMAT_UNKNOWN);
	bool LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	class CTexture* FindTexture(const std::string& Name);
	void ReleaseTexture(const std::string& Name);

public:
	/*
	D3D11_FILTER Filter;
    D3D11_TEXTURE_ADDRESS_MODE AddressU;
    D3D11_TEXTURE_ADDRESS_MODE AddressV;
    D3D11_TEXTURE_ADDRESS_MODE AddressW;
    FLOAT MipLODBias;
    UINT MaxAnisotropy;
    D3D11_COMPARISON_FUNC ComparisonFunc;
    FLOAT BorderColor[ 4 ];
    FLOAT MinLOD;
    FLOAT MaxLOD;
	*/
	bool CreateSampler(const std::string& Name, D3D11_FILTER Filter,
		D3D11_TEXTURE_ADDRESS_MODE AddressU,
		D3D11_TEXTURE_ADDRESS_MODE AddressV,
		D3D11_TEXTURE_ADDRESS_MODE AddressW,
		float BorderColor[4]);
	ID3D11SamplerState* FindSampler(const std::string& Name);
	void SetSampler(const std::string& Name, int Register,
		int ShaderType = (int)Buffer_Shader_Type::All);

public:
	template <typename T>
	void AddResourceChangeCallBack(T* Obj, void(T::* Func)())
	{
		m_ChangeCallBack = std::bind(Func, Obj);
	}
};

