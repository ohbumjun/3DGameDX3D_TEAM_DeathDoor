#include "ToonInstancingShader.h"
#include "ToonCBuffer.h"

CToonInstancingShader::CToonInstancingShader()	:
	m_CBuffer(nullptr)
{
	SetTypeID<CToonInstancingShader>();
}

CToonInstancingShader::~CToonInstancingShader()
{
	SAFE_DELETE(m_CBuffer);
}

bool CToonInstancingShader::Init()
{
	if (!LoadVertexShader("ToonInstancingVS", TEXT("Toon.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("ToonInstancingPS", TEXT("Toon.fx"), SHADER_PATH))
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

	return true;
}

void CToonInstancingShader::SetShaderParams(const ShaderParams& Params)
{
	m_CBuffer->SetType(Params.ToonType);
	m_CBuffer->UpdateCBuffer();
}
