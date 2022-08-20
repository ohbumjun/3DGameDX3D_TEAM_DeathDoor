
#include "Standard3DShader.h"
#include "../ResourceManager.h"
#include "Standard3DInstancingShader.h"

CStandard3DShader::CStandard3DShader()
{
	SetTypeID<CStandard3DShader>();
}

CStandard3DShader::~CStandard3DShader()
{
}

bool CStandard3DShader::Init()
{
	if (!LoadVertexShader("Standard3DVS", TEXT("Standard3D.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("Standard3DPS", TEXT("Standard3D.fx"), SHADER_PATH))
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


	// Standard3D Instancing Shader도 같이 만든다.
	bool Success = CResourceManager::GetInst()->CreateInstancingShader<CStandard3DShader, CStandard3DInstancingShader>(this, "Standard3DInstancingShader");
	if (!Success)
	{
		return false;
	}

	return true;
}
