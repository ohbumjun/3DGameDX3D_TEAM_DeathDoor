#pragma once

#include "IMGUIWidget.h"

class CIMGUIImage :
    public CIMGUIWidget
{
public:
	CIMGUIImage();
	virtual ~CIMGUIImage();

protected:
	std::string m_TableTitle;
	CSharedPtr<class CTexture>	m_Texture;
	ImVec2			m_ImageStart;
	ImVec2			m_ImageEnd;
	ImVec4			m_Tint;
	ImVec4			m_BorderColor;
protected :
	ID3D11Texture2D* m_CopyTargetTexResource;
	ID3D11ShaderResourceView* m_ShaderResourceView;
	ID3D11Texture2D* m_TexResource;
	IDXGISurface* m_Surface;
	bool m_IsRenderTargetImage;
public:
	void SetImageStart(float x, float y)
	{
		m_ImageStart = ImVec2(x, y);
	}

	void SetImageEnd(float x, float y)
	{
		m_ImageEnd = ImVec2(x, y);
	}

	void SetTint(unsigned char r, unsigned char g, unsigned char b)
	{
		m_Tint = ImVec4(r / 255.f, g / 255.f, b / 255.f, 1.f);
	}

	void SetBorderColor(unsigned char r, unsigned char g, unsigned char b)
	{
		m_BorderColor = ImVec4(r / 255.f, g / 255.f, b / 255.f, 1.f);
	}

	void SetRenderTargetImage(bool Enable)
	{
		m_IsRenderTargetImage = Enable;
	}
	void SetTableTitle(const std::string& Title)
	{
		m_TableTitle = Title;
	}

public:
	void SetTexture(const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = TEXTURE_PATH);
	void SetTexture(const std::string& Name);
	void SetTextureFullPath(const std::string& Name, const TCHAR* FullPath);
	void SetTexture(class CTexture* Texture);
	// Render Target 을 그려내기 위한 세팅
public : 
	void CreateTexture(unsigned int Width, unsigned int Height);
public:
	virtual bool Init();
	virtual void Render();
};

