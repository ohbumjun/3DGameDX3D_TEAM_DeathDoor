#pragma once

#include "../../Ref.h"

struct TextureResourceInfo
{
	ScratchImage*	Image;
	ID3D11ShaderResourceView* SRV;
	unsigned int	Width;
	unsigned int	Height;
	TCHAR* FileName;
	char* PathName;
	TCHAR* FullPath;

	// 아래 변수는 현재 RenderTarget.cpp 의 CreateTarget 에서만 세팅해주는 중 (Editor 에서 Render Target 가져올 때 사용)
	ID3D11Texture2D* TextureResource;

	TextureResourceInfo()	:
		Image(nullptr),
		SRV(nullptr),
		FileName(nullptr),
		PathName(nullptr),
		FullPath(nullptr),
		Width(0),
		Height(0)
	{
	}

	~TextureResourceInfo()
	{
		SAFE_RELEASE(SRV);
		SAFE_DELETE_ARRAY(FileName);
		SAFE_DELETE_ARRAY(PathName);
		SAFE_DELETE_ARRAY(FullPath);
		SAFE_DELETE(Image);
	}
};

class CTexture :
    public CRef
{
	friend class CTextureManager;
	friend class CIMGUIImageButton;

protected:
	CTexture();
	virtual ~CTexture();

protected:
	class CScene* m_Scene;
	std::vector<TextureResourceInfo*>	m_vecTextureInfo;
	Image_Type	m_ImageType;
	ID3D11ShaderResourceView* m_ArraySRV;

public:
	const std::vector<TextureResourceInfo*>& GetVecTextureInfo() const
	{
		return m_vecTextureInfo;
	}

	ID3D11Texture2D* GetTextureResource(int Index = 0) const
	{
		return m_vecTextureInfo[Index]->TextureResource;
	}

	ID3D11ShaderResourceView* GetResource(int Index = 0)	const
	{
		return m_vecTextureInfo[Index]->SRV;
	}

	ScratchImage* GetScratchImage(int Index = 0) const
	{
		return m_vecTextureInfo[Index]->Image;
	}

	Image_Type GetImageType()	const
	{
		return m_ImageType;
	}

	unsigned int GetWidth(int Index = 0)	const
	{
		return m_vecTextureInfo[Index]->Width;
	}

	unsigned int GetHeight(int Index = 0)	const
	{
		return m_vecTextureInfo[Index]->Height;
	}

	size_t GetImageCount()	const
	{
		return m_vecTextureInfo.size();
	}

public:
	bool LoadTexture(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTexture(std::string& OutName, const TCHAR* FileName,
		const std::string& PathName = TEXTURE_PATH);
	// Default : Bin 폴더 뒷부분을 FileName 으로 세팅해준다.
	bool LoadTextureFullPath(const std::string& Name, const TCHAR* FullPath);
	bool LoadTextureFullPathMultibyte(const std::string& Name, const char* FullPathMultibyte);
	bool LoadTexture(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
	bool LoadTextureArray(const std::string& Name, const std::vector<TCHAR*>& vecFileName,
		const std::string& PathName = TEXTURE_PATH);
	bool LoadTextureArrayFullPath(const std::string& Name, const std::vector<TCHAR*>& vecFullPath);

private:
	bool CreateResource(int Index);
	bool CreateResourceArray();

public:
	void SetShader(int Register, int ShaderType, int Index);
	void ResetShader(int Register, int ShaderType, int Index);
	bool Save(FILE* pFile);
};

