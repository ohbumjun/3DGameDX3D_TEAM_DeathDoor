#include "WaterShader.h"
#include "../Texture/RenderTarget.h"
#include "ShadowCBuffer.h"
#include "../ResourceManager.h"
#include "../../Render/RenderManager.h"

CWaterShader::CWaterShader()
{
	SetTypeID<CWaterShader>();
}

CWaterShader::~CWaterShader()
{
}

bool CWaterShader::Init()
{
	if (!LoadVertexShader("WaterVS", TEXT("Water.fx"), SHADER_PATH))
		return false;

	if (!LoadPixelShader("WaterPS", TEXT("Water.fx"), SHADER_PATH))
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

	return true;
}

void CWaterShader::SetShaderResources()
{
	CShadowCBuffer* ShadowCBuffer = CRenderManager::GetInst()->GetShadowCBuffer();
	CRenderTarget* DepthTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer2");
	CRenderTarget* ShadowTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("ShadowMap");

	ShadowCBuffer->UpdateCBuffer();
	DepthTarget->SetTargetShader(11);
	ShadowTarget->SetTargetShader(12);
}

void CWaterShader::ResetShaderResources()
{
	CRenderTarget* DepthTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer2");
	CRenderTarget* ShadowTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("ShadowMap");

	DepthTarget->ResetTargetShader(11);
	ShadowTarget->ResetTargetShader(12);
}
