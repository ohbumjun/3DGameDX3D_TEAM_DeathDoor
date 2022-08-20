#include "PostFXRenderer.h"
#include "../Device.h"
#include "../Engine.h"
#include "../Resource/Shader/HDRRenderCBuffer.h"
#include "../Resource/Shader/FogCBuffer.h"
#include "../Resource/Shader/DownScaleCBuffer.h"
#include "../Resource/Shader/ComputeShader.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Texture/RenderTarget.h"
#include "RenderState.h"
#include "RenderManager.h"
#include "RenderStateManager.h"

CPostFXRenderer::CPostFXRenderer()	:
	m_GBufferDepth(nullptr),
	m_PlayerStencil(nullptr),
	m_DownScaleFirstPassShader(nullptr),
	m_DownScaleSecondPassShader(nullptr),
	m_BloomShader(nullptr),
	m_BlurVerticalShader(nullptr),
	m_BlurHorizontalShader(nullptr),
	m_HDRRenderShader(nullptr),
	m_DownScaleCBuffer(nullptr),
	m_HDRRenderCBuffer(nullptr),
	m_FogCBuffer(nullptr),
	m_AlphaBlend(nullptr),
	m_DepthDisable(nullptr),
	m_DownScaleBuffer(nullptr),
	m_DownScaleUAV(nullptr),
	m_DownScaleSRV(nullptr),
	m_LuminanceAverageBuffer(nullptr),
	m_LuminanceAverageBufferUAV(nullptr),
	m_LuminanceAverageBufferSRV(nullptr),
	m_PrevFrameLumAverageBuffer(nullptr),
	m_PrevFrameLumAverageBufferUAV(nullptr),
	m_PrevFrameLumAverageBufferSRV(nullptr),
	m_DownScaleRT(nullptr),
	m_DownScaleRTUAV(nullptr),
	m_DownScaleRTSRV(nullptr),
	m_BloomRT(nullptr),
	m_BloomUAV(nullptr),
	m_BloomSRV(nullptr),
	m_Temp1RT(nullptr),
	m_Temp1UAV(nullptr),
	m_Temp1SRV(nullptr),
	m_Temp2RT(nullptr),
	m_Temp2UAV(nullptr),
	m_Temp2SRV(nullptr),
	m_AdaptationTime(3.f),
	m_AdaptationTimer(0.f),
	m_FirstFrame(true)
{
}

CPostFXRenderer::~CPostFXRenderer()
{
	SAFE_DELETE(m_DownScaleCBuffer);
	SAFE_DELETE(m_HDRRenderCBuffer);
	SAFE_DELETE(m_FogCBuffer);

	SAFE_RELEASE(m_DownScaleBuffer);
	SAFE_RELEASE(m_DownScaleUAV);
	SAFE_RELEASE(m_DownScaleSRV);

	SAFE_RELEASE(m_LuminanceAverageBuffer);
	SAFE_RELEASE(m_LuminanceAverageBufferUAV);
	SAFE_RELEASE(m_LuminanceAverageBufferSRV);

	SAFE_RELEASE(m_PrevFrameLumAverageBuffer);
	SAFE_RELEASE(m_PrevFrameLumAverageBufferUAV);
	SAFE_RELEASE(m_PrevFrameLumAverageBufferSRV);

	SAFE_RELEASE(m_DownScaleRT);
	SAFE_RELEASE(m_DownScaleRTUAV);
	SAFE_RELEASE(m_DownScaleRTSRV);

	SAFE_RELEASE(m_BloomRT);
	SAFE_RELEASE(m_BloomUAV);
	SAFE_RELEASE(m_BloomSRV);

	SAFE_RELEASE(m_Temp1RT);
	SAFE_RELEASE(m_Temp1UAV);
	SAFE_RELEASE(m_Temp1SRV);

	SAFE_RELEASE(m_Temp2RT);
	SAFE_RELEASE(m_Temp2UAV);
	SAFE_RELEASE(m_Temp2SRV);

 //	SAFE_RELEASE(m_Query);
 //	SAFE_RELEASE(m_Temp);
 //	SAFE_RELEASE(m_Temp2);
 //	SAFE_RELEASE(m_Temp2UAV);
}

bool CPostFXRenderer::Init()
{
	Resolution RS = CDevice::GetInst()->GetResolution();
	unsigned int PixelCount = RS.Width * RS.Height;
	unsigned int GroupCount = (unsigned int)ceil((float)(PixelCount / 16.f) / 1024.f);

	// Depth Buffer
	m_GBufferDepth = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer2");
	m_PlayerStencil = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("PlayerTarget");

	// 1. DownScale Luminance Buffer
	D3D11_BUFFER_DESC DownScaleBufferDesc = {};
	DownScaleBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	DownScaleBufferDesc.StructureByteStride = sizeof(float);
	DownScaleBufferDesc.ByteWidth = GroupCount * DownScaleBufferDesc.StructureByteStride;
	DownScaleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	DownScaleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBuffer(&DownScaleBufferDesc, nullptr, &m_DownScaleBuffer)))
	{
		assert(false);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC DownScaleUAVDesc = {};
	DownScaleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	DownScaleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DownScaleUAVDesc.Buffer.NumElements = GroupCount;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_DownScaleBuffer, &DownScaleUAVDesc, &m_DownScaleUAV)))
	{
		assert(false);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC DownScaleSRVDesc = {};
	DownScaleSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	DownScaleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	DownScaleSRVDesc.Buffer.NumElements = GroupCount;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(m_DownScaleBuffer, &DownScaleSRVDesc, &m_DownScaleSRV)))
	{
		assert(false);
		return false;
	}

	// 2. Average Luminance Buffer
	D3D11_BUFFER_DESC AverageBufferDesc = {};
	AverageBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	AverageBufferDesc.StructureByteStride = sizeof(float);
	AverageBufferDesc.ByteWidth = sizeof(float);
	AverageBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	AverageBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBuffer(&AverageBufferDesc, nullptr, &m_LuminanceAverageBuffer)))
	{
		assert(false);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC AverageUAVDesc = {};
	AverageUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	AverageUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	AverageUAVDesc.Buffer.NumElements = 1;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_LuminanceAverageBuffer, &AverageUAVDesc, &m_LuminanceAverageBufferUAV)))
	{
		assert(false);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC AverageSRVDesc = {};
	AverageSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	AverageSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	AverageSRVDesc.Buffer.NumElements = 1;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(m_LuminanceAverageBuffer, &AverageSRVDesc, &m_LuminanceAverageBufferSRV)))
	{
		assert(false);
		return false;
	}

	// 3. Prev Frame Luminance Resource ( Same As Above )
	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBuffer(&AverageBufferDesc, nullptr, &m_PrevFrameLumAverageBuffer)))
	{
		assert(false);
		return false;
	}

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_PrevFrameLumAverageBuffer, &AverageUAVDesc, &m_PrevFrameLumAverageBufferUAV)))
	{
		assert(false);
		return false;
	}

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(m_PrevFrameLumAverageBuffer, &AverageSRVDesc, &m_PrevFrameLumAverageBufferSRV)))
	{
		assert(false);
		return false;
	}

	/// Bloom
	// DownScale Texture 생성 -> 백버퍼의 1/16 크기로 다운스케일된 텍스쳐
	D3D11_TEXTURE2D_DESC DsTd = {};
	DsTd.Width = RS.Width / 4;
	DsTd.Height = RS.Height / 4;
	DsTd.MipLevels = 1;
	DsTd.ArraySize = 1;
	DsTd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DsTd.SampleDesc.Count = 1;
	DsTd.SampleDesc.Quality = 0;
	DsTd.Usage = D3D11_USAGE_DEFAULT;
	DsTd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	// 텍스쳐 생성 - Bloom, Temp Texture 모두 같은 크기
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateTexture2D(&DsTd, nullptr, &m_DownScaleRT));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateTexture2D(&DsTd, nullptr, &m_BloomRT));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateTexture2D(&DsTd, nullptr, &m_Temp1RT));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateTexture2D(&DsTd, nullptr, &m_Temp2RT));

	// SRV, UAV 생성
	D3D11_UNORDERED_ACCESS_VIEW_DESC DsUAV = {};
	DsUAV.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DsUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	DsUAV.Buffer.FirstElement = 0;
	DsUAV.Buffer.NumElements = PixelCount / 16;
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_DownScaleRT, &DsUAV, &m_DownScaleRTUAV));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_BloomRT, &DsUAV, &m_BloomUAV));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_Temp1RT, &DsUAV, &m_Temp1UAV));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_Temp2RT, &DsUAV, &m_Temp2UAV));

	D3D11_SHADER_RESOURCE_VIEW_DESC DsSRV = {};
	DsSRV.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DsSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	DsSRV.Texture2D.MipLevels = 1;
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(m_DownScaleRT, &DsSRV, &m_DownScaleRTSRV));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(m_BloomRT, &DsSRV, &m_BloomSRV));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(m_Temp1RT, &DsSRV, &m_Temp1SRV));
	FAILED_CHECK(CDevice::GetInst()->GetDevice()->CreateShaderResourceView(m_Temp2RT, &DsSRV, &m_Temp2SRV));

 //	///  Debug Section
 //
 //	// Temp
 //	D3D11_BUFFER_DESC TempBufferDes = {};
 //	TempBufferDes.StructureByteStride = sizeof(float);
 //	TempBufferDes.ByteWidth = sizeof(float);
 //	TempBufferDes.Usage = D3D11_USAGE_STAGING;
 //	TempBufferDes.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
 //	TempBufferDes.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
 //
 //	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBuffer(&TempBufferDes, nullptr, &m_Temp)))
 //	{
 //		assert(false);
 //		return false;
 //	}
 //
 //	TempBufferDes.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
 //	TempBufferDes.StructureByteStride = sizeof(float);
 //	TempBufferDes.ByteWidth = sizeof(float);
 //	TempBufferDes.Usage = D3D11_USAGE_DEFAULT;
 //	TempBufferDes.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
 //	TempBufferDes.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
 //
 //	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBuffer(&TempBufferDes, nullptr, &m_Temp2)))
 //	{
 //		assert(false);
 //		return false;
 //	}
 //
 //	AverageUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
 //	AverageUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
 //	AverageUAVDesc.Buffer.NumElements = 1;
 //
 //	if (FAILED(CDevice::GetInst()->GetDevice()->CreateUnorderedAccessView(m_Temp2, &AverageUAVDesc, &m_Temp2UAV)))
 //	{
 //		assert(false);
 //		return false;
 //	}
 //
 //	
 //	D3D11_QUERY_DESC QueryDesc = {};
 //	QueryDesc.Query = D3D11_QUERY_EVENT;
 //	QueryDesc.MiscFlags = 0;
 //	CDevice::GetInst()->GetDevice()->CreateQuery(&QueryDesc, &m_Query);
 //
 //	///  Debug Section End

	// DownScale Constant Buffer
	Resolution DownScaleRS;
	DownScaleRS.Width = RS.Width / 4;
	DownScaleRS.Height = RS.Height / 4;

	m_DownScaleCBuffer = new CDownScaleCBuffer;
	m_DownScaleCBuffer->Init();
	m_DownScaleCBuffer->SetResolution(DownScaleRS);
	m_DownScaleCBuffer->SetDomain(PixelCount / 16);
	m_DownScaleCBuffer->SetGroupSize(GroupCount);

	// Render Constant Buffer
	m_HDRRenderCBuffer = new CHDRRenderCBuffer;
	m_HDRRenderCBuffer->Init();

	// Fog Constant Buffer
	m_FogCBuffer = new CFogCBuffer;
	m_FogCBuffer->Init();

	// Shader
	m_DownScaleFirstPassShader = (CComputeShader*)CResourceManager::GetInst()->FindShader("HDRDownScaleFirstPassShader");
	m_DownScaleSecondPassShader = (CComputeShader*)CResourceManager::GetInst()->FindShader("HDRDownScaleSecondPassShader");
	m_BloomShader = (CComputeShader*)CResourceManager::GetInst()->FindShader("BloomShader");
	m_BlurVerticalShader = (CComputeShader*)CResourceManager::GetInst()->FindShader("BlurVerticalShader");
	m_BlurHorizontalShader = (CComputeShader*)CResourceManager::GetInst()->FindShader("BlurHorizontalShader");
	m_HDRRenderShader = CResourceManager::GetInst()->FindShader("HDRRenderShader");
	m_BombEffectShader = CResourceManager::GetInst()->FindShader("BombShader");

	// RenderState
	m_DepthDisable = CRenderManager::GetInst()->GetRenderStateManager()->FindRenderState("DepthDisable");
	m_AlphaBlend = CRenderManager::GetInst()->GetRenderStateManager()->FindRenderState("AlphaBlend");

	return true;
}

void CPostFXRenderer::ComputeAdaptation(float DeltaTime)
{
	// Adaptation 계수 계산
	float AdaptNorm;

	m_AdaptationTimer += DeltaTime;

	if (m_FirstFrame)
	{
		AdaptNorm = 1.f;
		m_FirstFrame = false;
	}
	else
	{
		if (m_AdaptationTimer >= m_AdaptationTime)
		{
			m_AdaptationTimer = 0.f;
		}
		
		AdaptNorm = m_AdaptationTime > 0.0001f ? (m_AdaptationTimer / m_AdaptationTime) : 0.9999f;
	}

	m_DownScaleCBuffer->SetAdaptation(AdaptNorm);
}

void CPostFXRenderer::SetMiddleGray(float Gray)
{
	m_HDRRenderCBuffer->SetMiddleGray(Gray);
}

void CPostFXRenderer::SetLumWhite(float White)
{
	m_HDRRenderCBuffer->SetLumWhite(White);
}

void CPostFXRenderer::SetBloomThreshold(float Threshold)
{
	m_DownScaleCBuffer->SetBloomThreshold(Threshold);
}

void CPostFXRenderer::SetBloomScale(float Scale)
{
	m_HDRRenderCBuffer->SetBloomScale(Scale);
}

void CPostFXRenderer::SetDOFMin(float Min)
{
	m_HDRRenderCBuffer->SetDOFMin(Min);
}

void CPostFXRenderer::SetDOFMax(float Max)
{
	m_HDRRenderCBuffer->SetDOFMax(Max);
}

void CPostFXRenderer::SetFogColor(const Vector3& Color)
{
	m_FogCBuffer->SetFogColor(Color);
}

void CPostFXRenderer::SetFogType(Fog_Type Type)
{
	m_FogCBuffer->SetFogType(Type);
}

void CPostFXRenderer::SetFogStart(float Start)
{
	m_FogCBuffer->SetFogStart(Start);
}

void CPostFXRenderer::SetFogEnd(float End)
{
	m_FogCBuffer->SetFogEnd(End);
}

void CPostFXRenderer::SetFogDensity(float Density)
{
	m_FogCBuffer->SetFogDensity(Density);
}

float CPostFXRenderer::GetMiddleGray() const
{
	return m_HDRRenderCBuffer->GetMiddleGray();
}

float CPostFXRenderer::GetLumWhite() const
{
	return m_HDRRenderCBuffer->GetLumWhite();
}

float CPostFXRenderer::GetBloomThreshold() const
{
	return m_DownScaleCBuffer->GetBloomThreshold();
}

float CPostFXRenderer::GetBloomScale() const
{
	return m_HDRRenderCBuffer->GetBloomScale();
}

float CPostFXRenderer::GetDOFMin() const
{
	return m_HDRRenderCBuffer->GetDOFMin();
}

float CPostFXRenderer::GetDOFMax() const
{
	return m_HDRRenderCBuffer->GetDOFMax();
}

const Vector3& CPostFXRenderer::GetFogColor() const
{
	return m_FogCBuffer->GetFogColor();
}

Fog_Type CPostFXRenderer::GetFogType() const
{
	return m_FogCBuffer->GetFogType();
}

float CPostFXRenderer::GetFogStart() const
{
	return m_FogCBuffer->GetFogStart();
}

float CPostFXRenderer::GetFogEnd() const
{
	return m_FogCBuffer->GetFogEnd();
}

float CPostFXRenderer::GetFogDensity() const
{
	return m_FogCBuffer->GetFogDensity();
}

void CPostFXRenderer::Render(float DeltaTime, CRenderTarget* LDRTarget, bool BombEffect)
{
	// 적응 계수 계산
	ComputeAdaptation(DeltaTime);

	// 스크린 휘도 계산
	ComputeHDR(LDRTarget);

	// Bloom 픽셀 계산
	ComputeBloom();

	// Blur 처리
	Blur();

	if (BombEffect)
	{
		RenderBombEffect(LDRTarget);
	}
	else
	{
		// 백버퍼 렌더
		RenderFinal(LDRTarget);
	}
}

void CPostFXRenderer::ComputeHDR(CRenderTarget* LDRTarget)
{
	ID3D11DeviceContext* Context = CDevice::GetInst()->GetContext();

	/////////////////
	// First  Path //
	/////////////////

	// HDR Target
	LDRTarget->SetShader(120, (int)Buffer_Shader_Type::Compute, 0);

	// CBuffer
	m_DownScaleCBuffer->UpdateCBuffer();

	// DownScale 결과 버퍼
	unsigned int Count = -1;
	Context->CSSetUnorderedAccessViews(7, 1, &m_DownScaleUAV, &Count);

	// 축소된 씬 저장할 텍스쳐 바인드
	Context->CSSetUnorderedAccessViews(6, 1, &m_DownScaleRTUAV, &Count);

	// Context->CSSetUnorderedAccessViews(6, 1, &m_Temp2UAV, &Count);

	// Excute
	m_DownScaleFirstPassShader->SetShader();
	unsigned int GroupSize = m_DownScaleCBuffer->GetGroupSize();
	m_DownScaleFirstPassShader->Excute(GroupSize, 1, 1);

	// DEBUG
	 //Context->End(m_Query);
 //
 //	while(Context->GetData(m_Query, NULL, 0, 0) == S_FALSE)
 //	{
 //		int a = 1;
 //	}
 //
	// DEBUG
	ID3D11UnorderedAccessView* UAV = nullptr;
	Context->CSSetUnorderedAccessViews(7, 1, &UAV, &Count);
	Context->CSSetUnorderedAccessViews(6, 1, &UAV, &Count);
	//Context->CSSetUnorderedAccessViews(6, 1, &UAV, &Count);

	// Context->CopyResource(m_Temp2, m_DownScaleBuffer);
 //	D3D11_MAPPED_SUBRESOURCE Map2 = {};
 //	Context->Map(m_DownScaleBuffer, 0, D3D11_MAP_READ, 0, &Map2);
 //	memcpy(m_TempBuf, Map2.pData, (UINT)sizeof(float) * 57);
 //	Context->Unmap(m_DownScaleBuffer, 0);

 //	D3D11_MAPPED_SUBRESOURCE Map2 = {};
 //	float Temp2 = 2.f;
 //	Context->Map(m_Temp2, 0, D3D11_MAP_READ, 0, &Map2);
 //	memcpy(&Temp2, Map2.pData, sizeof(float));
 //	Context->Unmap(m_Temp2, 0);

	/////////////////
	// Second Path //
	/////////////////

	// 씬 평균 휘도를 하나의 픽셀에 저장하기 위한 Output Buffer
	Context->CSSetUnorderedAccessViews(7, 1, &m_LuminanceAverageBufferUAV, &Count);

	// 이전 프레임 휘도 정보
	Context->CSSetShaderResources(22, 1, &m_PrevFrameLumAverageBufferSRV);

	// CBuffer
	m_DownScaleCBuffer->UpdateCBuffer();

	// First Path에서 64개 이하로 압축된 씬 휘도 정보
	Context->CSSetShaderResources(21, 1, &m_DownScaleSRV);

	// Excute
	m_DownScaleSecondPassShader->SetShader();
	m_DownScaleSecondPassShader->Excute(1, 1, 1);

 //	// DEBUG
 //	Context->End(m_Query);
 //
 //	while(Context->GetData(m_Query, NULL, 0, 0) == S_FALSE)
 //	{
 //		int a = 1;
 //	}
 //
 //	// DEBUG

	// Reset
	Context->CSSetUnorderedAccessViews(7, 1, &UAV, &Count);

	ID3D11ShaderResourceView* SRV = nullptr;
	Context->CSSetShaderResources(21, 1, &SRV);
	Context->CSSetShaderResources(22, 1, &SRV);

	// !!!!!! 두번째 패스 돌아가기 전까지 해제하면 안됨 !!!!!!!
	LDRTarget->ResetShader(120, (int)Buffer_Shader_Type::Compute, 0);
	
 //	Context->CopyResource(m_Temp, m_LuminanceAverageBuffer);
 //	D3D11_MAPPED_SUBRESOURCE Map = {};
 //	float CurLumAv;
 //	Context->Map(m_Temp, 0, D3D11_MAP_READ, 0, &Map);
 //	memcpy(&CurLumAv, Map.pData, sizeof(float));
 //	Context->Unmap(m_Temp, 0);
}

void CPostFXRenderer::ComputeBloom()
{
	ID3D11DeviceContext* Context = CDevice::GetInst()->GetContext();
	unsigned int Count = -1;

	// 해상도 축소된 씬과 평균 휘도로 평균 휘도 이상인 픽셀을 검출한다.
	Context->CSSetShaderResources(20, 1, &m_DownScaleRTSRV);
	Context->CSSetShaderResources(10, 1, &m_LuminanceAverageBufferSRV);
	Context->CSSetUnorderedAccessViews(7, 1, &m_Temp1UAV, &Count);

	int GroupCount = m_DownScaleCBuffer->GetGroupSize();
	m_BloomShader->Excute(GroupCount, 1, 1);

	// Reset 
	ID3D11UnorderedAccessView* UAV = nullptr;
	ID3D11ShaderResourceView* SRV = nullptr;
	Context->CSSetUnorderedAccessViews(7, 1, &UAV, &Count);
	Context->CSSetShaderResources(20, 1, &SRV);
	Context->CSSetShaderResources(10, 1, &SRV);
}

void CPostFXRenderer::Blur()
{
	Resolution RS = CDevice::GetInst()->GetResolution();
	ID3D11DeviceContext* Context = CDevice::GetInst()->GetContext();
	unsigned int Count = -1;
	ID3D11UnorderedAccessView* UAV = nullptr;
	ID3D11ShaderResourceView* SRV = nullptr;

	// 1. 수직 블러 적용
	// Bloom() 에서 검출한 평균 휘도 이상의 픽셀 정보를 가지고 수직 Blur 처리하고, Temp Texture에 쓴다.
	Context->CSSetShaderResources(20, 1, &m_Temp1SRV);
	Context->CSSetUnorderedAccessViews(7, 1, &m_Temp2UAV, &Count);

	m_BlurVerticalShader->Excute((unsigned int)ceil(RS.Width / 4.f), (unsigned int)ceil((RS.Height / 4.f) / (128.f - 12.f)), 1);

	Context->CSSetShaderResources(20, 1, &SRV);
	Context->CSSetUnorderedAccessViews(7, 1, &UAV, &Count);

	// 2. 수평 블러 적용
	// 수직 블러 처리된 Temp Texture를 Input으로 수평 블러 처리해서, 마지막 Bloom Texture에 쓴다.
	Context->CSSetShaderResources(20, 1, &m_Temp2SRV);
	Context->CSSetUnorderedAccessViews(7, 1, &m_BloomUAV, &Count);

	m_BlurHorizontalShader->Excute((unsigned int)ceil((RS.Width / 4.f) / (128.f - 12.f)), (unsigned int)ceil(RS.Height / 4.f), 1);

	Context->CSSetShaderResources(20, 1, &SRV);
	Context->CSSetUnorderedAccessViews(7, 1, &UAV, &Count);
}

void CPostFXRenderer::RenderFinal(CRenderTarget* LDRTarget)
{
	ID3D11DeviceContext* Context = CDevice::GetInst()->GetContext();

	m_HDRRenderCBuffer->UpdateCBuffer();
	m_FogCBuffer->UpdateCBuffer();

	ID3D11ShaderResourceView* arrSRV[6] = {};
	arrSRV[0] = LDRTarget->GetResource(0);				// HDR로 변환할 Final Screen (LDR 렌더타겟)
	arrSRV[1] = m_LuminanceAverageBufferSRV;			// 평균 휘도
	arrSRV[2] = m_BloomSRV;								// Blur 처리된 Bloom Texture
	arrSRV[3] = m_GBufferDepth->GetResource(0);			// DOF 처리를 위한 씬 깊이 텍스쳐
	arrSRV[4] = m_DownScaleRTSRV;						// DOF Blur를 위한 1 / 16 크기 씬
	arrSRV[5] = m_PlayerStencil->GetResource(0);

	Context->PSSetShaderResources(10, 6, arrSRV);

	m_HDRRenderShader->SetShader();
	
	m_DepthDisable->SetState();
	m_AlphaBlend->SetState();

	// Null Buffer 출력
	UINT Offset = 0;
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Context->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	Context->Draw(4, 0);

	// Reset 
	m_DepthDisable->ResetState();
	m_AlphaBlend->ResetState();

	for (int i = 0; i < 6; ++i)
	{
		arrSRV[i] = nullptr;
	}
	Context->PSSetShaderResources(10, 6, arrSRV);

	// 현재 프레임 평균 휘도를 저장
	SavePrevLum();

 //	D3D11_MAPPED_SUBRESOURCE Map = {};
 //	float CurLumAv;
 //	Context->Map(m_LuminanceAverageBuffer, 0, D3D11_MAP_READ, 0, &Map);
 //	memcpy(&CurLumAv, Map.pData, sizeof(float));
 //	Context->Unmap(m_LuminanceAverageBuffer, 0);
 //
 //	D3D11_MAPPED_SUBRESOURCE Map2 = {};
 //	float PrevLumAv;
 //	Context->Map(m_PrevFrameLumAverageBuffer, 0, D3D11_MAP_READ, 0, &Map2);
 //	memcpy(&PrevLumAv, Map2.pData, sizeof(float));
 //	Context->Unmap(m_PrevFrameLumAverageBuffer, 0);
 //
 //	if (abs(PrevLumAv - CurLumAv) > 0.2)
 //	{
 //		int a = 0;
 //	}
}

void CPostFXRenderer::RenderBombEffect(CRenderTarget* LDRTarget)
{
	ID3D11DeviceContext* Context = CDevice::GetInst()->GetContext();

	m_HDRRenderCBuffer->UpdateCBuffer();
	m_FogCBuffer->UpdateCBuffer();

	LDRTarget->SetTargetShader(10);
	Context->PSSetShaderResources(12, 1, &m_BloomSRV);
	ID3D11ShaderResourceView* DepthSrv = m_GBufferDepth->GetResource(0);
	Context->PSSetShaderResources(13, 1, &DepthSrv);

	m_BombEffectShader->SetShader();
	
	m_DepthDisable->SetState();

	// Null Buffer 출력
	UINT Offset = 0;
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Context->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	Context->Draw(4, 0);

	// Reset 
	m_DepthDisable->ResetState();

	ID3D11ShaderResourceView* SRV = nullptr;
	LDRTarget->ResetTargetShader(10);
	Context->PSSetShaderResources(12, 1, &SRV);
	Context->PSSetShaderResources(13, 1, &SRV);
}

void CPostFXRenderer::SavePrevLum()
{
	// 현재 프레임 평균 휘도를 저장
	ID3D11Buffer* TempBuffer = m_PrevFrameLumAverageBuffer;
	ID3D11UnorderedAccessView* TempUAV = m_PrevFrameLumAverageBufferUAV;
	ID3D11ShaderResourceView* TempSRV = m_PrevFrameLumAverageBufferSRV;

	m_PrevFrameLumAverageBuffer = m_LuminanceAverageBuffer;
	m_PrevFrameLumAverageBufferUAV = m_LuminanceAverageBufferUAV;
	m_PrevFrameLumAverageBufferSRV = m_LuminanceAverageBufferSRV;

	m_LuminanceAverageBuffer = TempBuffer;
	m_LuminanceAverageBufferUAV = TempUAV;
	m_LuminanceAverageBufferSRV = TempSRV;
}
