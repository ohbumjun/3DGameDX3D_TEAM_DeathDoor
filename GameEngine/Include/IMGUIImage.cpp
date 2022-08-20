
#include "IMGUIImage.h"
#include "Resource/Texture/Texture.h"
#include "Resource/ResourceManager.h"
#include "Render\RenderManager.h"
#include "Input.h"
#include "Timer.h"
#include "Device.h"

CIMGUIImage::CIMGUIImage()	:
	m_ImageStart{},
	m_ImageEnd{},
	m_Tint{ 1.f, 1.f, 1.f, 1.f },
	m_BorderColor{},
	m_IsRenderTargetImage(false)
{
}

CIMGUIImage::~CIMGUIImage()
{
	SAFE_RELEASE(m_TexResource);
	SAFE_RELEASE(m_ShaderResourceView);
}

void CIMGUIImage::SetTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	CResourceManager::GetInst()->LoadTexture(Name, FileName, PathName);

	m_Texture = CResourceManager::GetInst()->FindTexture(Name);

	if (!m_Texture)
		return;

	SetImageEnd((float)m_Texture->GetWidth(), (float)m_Texture->GetHeight());
}

void CIMGUIImage::SetTexture(const std::string& Name)
{
	m_Texture = CResourceManager::GetInst()->FindTexture(Name);

	if (!m_Texture)
		return;

	SetImageEnd((float)m_Texture->GetWidth(), (float)m_Texture->GetHeight());
}

void CIMGUIImage::SetTextureFullPath(const std::string& Name, const TCHAR* FullPath)
{
	CResourceManager::GetInst()->LoadTextureFullPath(Name, FullPath);

	m_Texture = CResourceManager::GetInst()->FindTexture(Name);

	if (!m_Texture)
		return;

	SetImageEnd((float)m_Texture->GetWidth(), (float)m_Texture->GetHeight());
}

void CIMGUIImage::SetTexture(CTexture* Texture)
{
	m_Texture = Texture;

	if (!m_Texture)
		return;

	SetImageEnd((float)m_Texture->GetWidth(), (float)m_Texture->GetHeight());
}

void CIMGUIImage::CreateTexture(unsigned int Width, unsigned int Height)
{
	SAFE_RELEASE(m_TexResource);
	SAFE_RELEASE(m_ShaderResourceView);

	// Target용 Texture 생성
	D3D11_TEXTURE2D_DESC	Desc = {};

	Desc.Width = Width;
	Desc.Height = Height;
	Desc.ArraySize = 1;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count = 4;
	Desc.SampleDesc.Quality = 0;
	Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	// Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Desc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateTexture2D(&Desc, nullptr, &m_TexResource)))
		return ;

	m_TexResource->QueryInterface(__uuidof(IDXGISurface), (void**)&m_Surface);

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(
		m_TexResource, nullptr, &m_ShaderResourceView)))
		return ;

	SAFE_RELEASE(m_Surface);
}

bool CIMGUIImage::Init()
{
	SetTexture("DefaultUI", TEXT("DefaultUI.png"));

	SetImageStart(0.f, 0.f);
	SetImageEnd(100.f, 100.f);

	return true;
}

void CIMGUIImage::Render()
{
	if (ImGui::CollapsingHeader(m_TableTitle.c_str()))
	{
		if (m_Texture)
		{
			unsigned int	Width = m_Texture->GetWidth();
			unsigned int	Height = m_Texture->GetHeight();

			ImVec2	StartUV = ImVec2(m_ImageStart.x / Width, m_ImageStart.y / Height);
			ImVec2	EndUV = ImVec2(m_ImageEnd.x / Width, m_ImageEnd.y / Height);

			ImGui::Image(m_Texture->GetResource(), m_Size, StartUV, EndUV, m_Tint, m_BorderColor);

			// 일반 Texture Rendering
			// if (!m_IsRenderTargetImage)
			// {
			// 	ImGui::Image(m_Texture->GetResource(), m_Size, StartUV, EndUV, m_Tint, m_BorderColor);
			// }
			// // Render Target 가져와서 Rendering
			// else
			// {
			// 	m_CopyTargetTexResource = m_Texture->GetTextureResource();
			// 
			// 	if (!m_CopyTargetTexResource)
			// 		return;
			// 
			// 	if (!m_TexResource)
			// 	{
			// 		CreateTexture(Width, Height);
			// 	}
			// 
			// 	// Target Texture 복사 
			// 	// SAFE_RELEASE(m_TexResource);
			// 
			// 	CDevice::GetInst()->GetContext()->CopyResource(m_TexResource, m_CopyTargetTexResource);
			// 
			// 	ImGui::Image(m_ShaderResourceView, m_Size, StartUV, EndUV, m_Tint, m_BorderColor);
			// }
			
		}
	}
}

