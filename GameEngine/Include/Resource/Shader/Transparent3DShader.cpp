#include "Transparent3DShader.h"
#include "../ResourceManager.h"
#include "TransparentInstancing3DShader.h"

CTransparent3DShader::CTransparent3DShader()
{
	SetTypeID<CTransparent3DShader>();
}

CTransparent3DShader::~CTransparent3DShader()
{
}

bool CTransparent3DShader::Init()
{
	if (!LoadVertexShader("Transparent3DVS", TEXT("Transparent3D.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("Transparent3DPS", TEXT("Transparent3D.fx"), SHADER_PATH))
		return false;

	AddInputDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0);
	AddInputDesc("BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!CreateInputLayout())
		return false;

	// Instancing Shader도 같이 만든다.
	bool Success = CResourceManager::GetInst()->CreateInstancingShader<CTransparent3DShader, CTransparentInstancing3DShader>(this, "TransparentInstancing3DShader");
	if (!Success)
	{
		return false;
	}

	return true;
}
