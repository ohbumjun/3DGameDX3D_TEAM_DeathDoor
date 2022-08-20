
#include "ShaderManager.h"
#include "ColorMeshShader.h"
#include "ConstantBuffer.h"
#include "Mesh2DShader.h"
#include "PosMeshShader.h"
#include "ColliderShader.h"
#include "ColliderPixelShader.h"
#include "WidgetShader.h"
#include "ProgressBarShader.h"
#include "NumberShader.h"
#include "ParticleUpdateShader.h"
#include "ParticleRenderShader.h"
#include "TileMapShader.h"
#include "Standard3DShader.h"
#include "AnimationUpdateShader.h"
#include "RenderTargetShader.h"
#include "LightAccShader.h"
#include "LightBlendShader.h"
#include "LightBlendRenderShader.h"
#include "SkyShader.h"
#include "LandScapeShader.h"
#include "DecalShader.h"
#include "DecalDebugShader.h"
#include "BillboardShader.h"
#include "Mesh3DNoLightShader.h"
#include "AnimEditorShader.h"
#include "Standard3DInstancingShader.h"
#include "Standard3DWireFrameShader.h"
#include "ShadowMapShader.h"
#include "ShadowMapInstancingShader.h"
#include "ShadowCBuffer.h"
#include "Transparent3DShader.h"
#include "TransparentInstancing3DShader.h"
#include "OutlineShader.h"
#include "GrayShader.h"
#include "HDRDownScaleFirstPassShader.h"
#include "HDRDownScaleSecondPassShader.h"
#include "HDRRenderShader.h"
#include "BloomShader.h"
#include "BlurVerticalShader.h"
#include "BlurHorizontalShader.h"
#include "ToonShader.h"
#include "WaterShader.h"
#include "LaserShader.h"
#include "BombShader.h"

// ParticleInfo.h
#include "../../ParticleInfo.h"

CShaderManager::CShaderManager()
{
}

CShaderManager::~CShaderManager()
{
	auto iter = m_mapShader.begin();
	auto iterEnd = m_mapShader.end();

	for (; iter != iterEnd;)
	{
		iter = m_mapShader.erase(iter);
		iterEnd = m_mapShader.end();
	}

	m_mapShader.clear();
}

bool CShaderManager::Init()
{
	if (!CreateShader<CColorMeshShader>("ColorMeshShader"))
		return false;

	if (!CreateShader<CPosMeshShader>("PosMeshShader"))
		return false;

	if (!CreateShader<CMesh2DShader>("Mesh2DShader"))
		return false;

	if (!CreateShader<CColliderShader>("ColliderShader"))
		return false;

	if (!CreateShader<CColliderPixelShader>("ColliderPixelShader"))
		return false;

	if (!CreateShader<CWidgetShader>("WidgetShader"))
		return false;

	if (!CreateShader<CProgressBarShader>("ProgressBarShader"))
		return false;

	if (!CreateShader<CNumberShader>("NumberShader"))
		return false;

	if (!CreateShader<CParticleUpdateShader>("ParticleUpdateShader"))
		return false;

	if (!CreateShader<CParticleRenderShader>("ParticleRenderShader"))
		return false;

	if (!CreateShader<CTileMapShader>("TileMapShader"))
		return false;

	if (!CreateShader<CAnimationUpdateShader>("AnimationUpdateShader"))
		return false;

	if (!CreateShader<CRenderTargetShader>("RenderTargetShader"))
		return false;

	if (!CreateShader<CLightAccShader>("LightAccShader"))
		return false;

	if (!CreateShader<CLightBlendShader>("LightBlendShader"))
		return false;

	if (!CreateShader<CLightBlendRenderShader>("LightBlendRenderShader"))
		return false;

	if (!CreateShader<CSkyShader>("SkyShader"))
		return false;

	if (!CreateShader<CLandScapeShader>("LandScapeShader"))
		return false;

	if (!CreateShader<CMesh3DNoLightShader>("Mesh3DNoLightShader"))
		return false;


	if (!CreateShader<CDecalShader>("DecalShader"))
		return false;


	if (!CreateShader<CDecalDebugShader>("DecalDebugShader"))
		return false;


	if (!CreateShader<CBillboardShader>("BillboardShader"))
		return false;

	if (!CreateShader<CAnimEditorShader>("AnimEditorShader"))
		return false;

	// Instancing Shader는 이 Shader가 만들어질 때 같이 생성됨
	if (!CreateShader<CStandard3DShader>("Standard3DShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CTransparent3DShader>("Transparent3DShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CStandard3DWireFrameShader>("Standard3DWireFrameShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CShadowMapShader>("ShadowMapShader"))
		return false;

	if (!CreateShader<CHDRDownScaleFirstPassShader>("HDRDownScaleFirstPassShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CHDRDownScaleSecondPassShader>("HDRDownScaleSecondPassShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CHDRRenderShader>("HDRRenderShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CBloomShader>("BloomShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CBlurVerticalShader>("BlurVerticalShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CBlurHorizontalShader>("BlurHorizontalShader"))
	{
		assert(false);
		return false;
	}

	CreateConstantBuffer("ToonCBuffer", sizeof(ToonCBuffer), 11,
		(int)Buffer_Shader_Type::Pixel);

	if (!CreateShader<CToonShader>("ToonShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CWaterShader>("WaterShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CLaserShader>("LaserShader"))
	{
		assert(false);
		return false;
	}

	if (!CreateShader<CBombShader>("BombShader"))
	{
		assert(false);
		return false;
	}

	// =================== 상수버퍼 ===================
	CreateConstantBuffer("TransformCBuffer", sizeof(TransformCBuffer), 0,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("MaterialCBuffer", sizeof(MaterialCBuffer), 1,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("Standard2DCBuffer", sizeof(Standard2DCBuffer), 2,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("GlobalCBuffer", sizeof(GlobalCBuffer), 3,
		(int)Buffer_Shader_Type::All);

	CreateConstantBuffer("PaperBurnCBuffer", sizeof(PaperBurnCBuffer), 4,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("LightCBuffer", sizeof(LightCBuffer), 5,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("Animation2DCBuffer", sizeof(Animation2DCBuffer), 10,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("ColliderCBuffer", sizeof(ColliderCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("WidgetCBuffer", sizeof(WidgetCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("ProgressBarCBuffer", sizeof(ProgressBarCBuffer), 12,
		(int)Buffer_Shader_Type::Graphic);

	// Particle CBuffer 의 경우, 기존의 Compute 뿐만 아니라, Graphic 에서도 사용할 수 있게 한다.
	// (왜냐하면, Pixel Shader 에서도 사용가능해야 하기 때문이다.)
	CreateConstantBuffer("ParticleCBuffer", sizeof(ParticleCBuffer), 7,
		(int)Buffer_Shader_Type::All);

	CreateConstantBuffer("ParticleTempValCBuffer", sizeof(ParticleTempValCBuffer), 9,
		(int)Buffer_Shader_Type::All);

	CreateConstantBuffer("TileMapCBuffer", sizeof(TileMapCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("AnimationCBuffer", sizeof(AnimationCBuffer), 11,
		(int)Buffer_Shader_Type::Compute);

	CreateConstantBuffer("LandScapeCBuffer", sizeof(LandScapeCBuffer), 11,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("LightForwardCBuffer", sizeof(LightForwardCBuffer), 12,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("InstancingCBuffer", sizeof(InstancingCBuffer), 6,
		(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("ShadowCBuffer", sizeof(ShadowCBuffer), 10,
		(int)Buffer_Shader_Type::Graphic);

	CreateConstantBuffer("DownScaleCBuffer", sizeof(DownScaleCBuffer), 10,
		(int)Buffer_Shader_Type::Compute);

	CreateConstantBuffer("HDRRenderCBuffer", sizeof(HDRRenderCBuffer), 10,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("FogCBuffer", sizeof(FogCBuffer), 11,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("GlobalLightCBuffer", sizeof(GlobalLightCBuffer), 11,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("WaterCBuffer", sizeof(WaterCBuffer), 13,
		(int)Buffer_Shader_Type::Pixel);

	CreateConstantBuffer("FadeCBuffer", sizeof(FadeCBuffer), 8,
		(int)Buffer_Shader_Type::Pixel);

	return true;
}

CShader* CShaderManager::FindShader(const std::string& Name)
{
	auto	iter = m_mapShader.find(Name);

	if (iter == m_mapShader.end())
		return nullptr;

	return iter->second;
}

CGraphicShader* CShaderManager::FindInstancingShader(CGraphicShader* Shader)
{
	auto iter = m_mapInstancingShader.find(Shader);

	if (iter == m_mapInstancingShader.end())
	{
		return nullptr;
	}

	return iter->second;
}

void CShaderManager::ReleaseShader(const std::string& Name)
{
	auto	iter = m_mapShader.find(Name);

	if (iter != m_mapShader.end())
	{
		if (iter->second->GetRefCount() == 1)
		{
			m_mapShader.erase(iter);
			if (m_ChangeCallBack)
			{
				m_ChangeCallBack();
			}
		}
	}
}

bool CShaderManager::CreateConstantBuffer(const std::string& Name, int Size, int Register, 
	int ConstantBufferShaderType)
{
	CConstantBuffer* Buffer = FindConstantBuffer(Name);

	if (Buffer)
		return true;

	Buffer = new CConstantBuffer;

	Buffer->SetName(Name);

	if (!Buffer->Init(Size, Register, ConstantBufferShaderType))
	{
		SAFE_RELEASE(Buffer);
		return false;
	}

	m_mapConstantBuffer.insert(std::make_pair(Name, Buffer));

	return true;
}

CConstantBuffer* CShaderManager::FindConstantBuffer(const std::string& Name)
{
	auto	iter = m_mapConstantBuffer.find(Name);

	if (iter == m_mapConstantBuffer.end())
		return nullptr;

	return iter->second;
}
