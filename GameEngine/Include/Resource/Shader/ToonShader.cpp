#include "ToonShader.h"
#include "ToonCBuffer.h"
#include "../ResourceManager.h"
#include "ToonInstancingShader.h"

CToonShader::CToonShader()	:
	m_CBuffer(nullptr)
{
	SetTypeID<CToonShader>();
}

CToonShader::~CToonShader()
{
	SAFE_DELETE(m_CBuffer);
}

bool CToonShader::Init()
{
	if (!LoadVertexShader("ToonVS", TEXT("Toon.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("ToonPS", TEXT("Toon.fx"), SHADER_PATH))
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

	m_CBuffer = new CToonCBuffer;
	m_CBuffer->Init();

	// Instancing Shader도 같이 만든다.
	bool Success = CResourceManager::GetInst()->CreateInstancingShader<CToonShader, CToonInstancingShader>(this, "ToonInstancingShader");
	if (!Success)
	{
		return false;
	}

	return true;
}

void CToonShader::SetShaderParams(const ShaderParams& Params)
{
	m_CBuffer->SetType(Params.ToonType);
	m_CBuffer->UpdateCBuffer();
}
