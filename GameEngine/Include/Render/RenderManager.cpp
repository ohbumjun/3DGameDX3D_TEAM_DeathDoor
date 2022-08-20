
#include "RenderManager.h"
#include "../GameObject/GameObject.h"
#include "../Component/StaticMeshComponent.h"
#include "../Component/AnimationMeshComponent.h"
#include "../Component/SceneComponent.h"
#include "RenderStateManager.h"
#include "../Resource/Shader/Standard2DConstantBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Mesh/AnimationMesh.h"
#include "RenderState.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Scene/Viewport.h"
#include "../Engine.h"
#include "../Device.h"
#include "../Resource/Shader/Shader.h"
#include "../Component/PaperBurnComponent.h"
#include "../Resource/Shader/ShadowCBuffer.h"
#include "../Resource/Shader/OutlineConstantBuffer.h"
#include "PostFXRenderer.h"
#include "../GameObject/SkyObject.h"
#include "../Resource/Shader/ConstantBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Shader/FadeCBuffer.h"

DEFINITION_SINGLE(CRenderManager)

CRenderManager::CRenderManager()	:
	m_RenderStateManager(nullptr),
	m_Standard2DCBuffer(nullptr),
	m_Shadow(true),
	m_ShadowLightDistance(15.f),
	m_ShadowCBuffer(nullptr),
	m_DebugRender(false),
	m_PostFXRenderer(nullptr),
	m_PostProcessing(true),
	m_RenderSkyBox(true),
	m_FadeCBuffer(nullptr),
	m_BombEffect(false)
{
}

CRenderManager::~CRenderManager()
{
	SAFE_DELETE(m_ShadowCBuffer);
	//SAFE_DELETE(m_OutlineCBuffer);

	auto	iter = m_RenderLayerList.begin();
	auto	iterEnd = m_RenderLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE((*iter));
	}

	m_RenderLayerList.clear();

	SAFE_DELETE(m_Standard2DCBuffer);
	SAFE_DELETE(m_RenderStateManager);
	SAFE_DELETE(m_PostFXRenderer);
	SAFE_DELETE(m_FadeCBuffer);
}

void CRenderManager::StartFadeEffect(FadeEffecType Type, bool Stay)
{
	m_CurFadeEffectType = Type;
	m_FadeEffectStart = true;
	m_FadeEffectTimer = 0.f;

	m_FadeCBuffer->SetFadeStart(true);
	m_FadeCBuffer->SetFadeStartColor(m_FadeInfo.StartColor);
	m_FadeCBuffer->SetFadeEndColor(m_FadeInfo.EndColor);
	m_FadeInfo.Stay = Stay;
}

void CRenderManager::EnableBombEffect(float Time)
{
	if (m_BombEffect)
	{
		return;
	}

	CScene* Scene = CSceneManager::GetInst()->GetScene();

	if (Scene)
	{
		// ������ ����
		CLightComponent* GlobalLight = Scene->GetLightManager()->GetGlobalLightComponent();

		m_OriginGLightColor = GlobalLight->GetLightColor();
		GlobalLight->SetColor(Vector4::Black);

		m_BombEffect = true;
		m_BombEffectTime = Time;
		m_BombEffectTimer = 0.f;
	}
}

float CRenderManager::GetMiddleGray() const
{
	return m_PostFXRenderer->GetMiddleGray();
}

float CRenderManager::GetLumWhite() const
{
	return m_PostFXRenderer->GetLumWhite();
}

float CRenderManager::GetBloomThreshold() const
{
	return m_PostFXRenderer->GetBloomThreshold();
}

float CRenderManager::GetBloomScale() const
{
	return m_PostFXRenderer->GetBloomScale();
}

float CRenderManager::GetDOFMin() const
{
	return m_PostFXRenderer->GetDOFMin();
}

float CRenderManager::GetDOFMax() const
{
	return m_PostFXRenderer->GetDOFMax();
}

const Vector3& CRenderManager::GetFogColor() const
{
	return m_PostFXRenderer->GetFogColor();
}

Fog_Type CRenderManager::GetFogType() const
{
	return m_PostFXRenderer->GetFogType();
}

float CRenderManager::GetFogStart() const
{
	return m_PostFXRenderer->GetFogStart();
}

float CRenderManager::GetFogEnd() const
{
	return m_PostFXRenderer->GetFogEnd();
}

float CRenderManager::GetFogDensity() const
{
	return m_PostFXRenderer->GetFogDensity();
}

float CRenderManager::GetShadowBias() const
{
	return m_ShadowCBuffer->GetBias();
}

bool CRenderManager::GetDebugRender() const
{
	return m_DebugRender;
}

void CRenderManager::SetMiddleGray(float Gray)
{
	m_PostFXRenderer->SetMiddleGray(Gray);
}

void CRenderManager::SetLumWhite(float White)
{
	m_PostFXRenderer->SetLumWhite(White);
}

void CRenderManager::SetBloomThreshold(float Threshold)
{
	m_PostFXRenderer->SetBloomThreshold(Threshold);
}

void CRenderManager::SetBloomScale(float Scale)
{
	m_PostFXRenderer->SetBloomScale(Scale);
}

void CRenderManager::SetDOFMin(float Min)
{
	m_PostFXRenderer->SetDOFMin(Min);
}

void CRenderManager::SetDOFMax(float Max)
{
	m_PostFXRenderer->SetDOFMax(Max);
}

void CRenderManager::SetFogColor(const Vector3& Color)
{
	m_PostFXRenderer->SetFogColor(Color);
}

void CRenderManager::SetFogType(Fog_Type Type)
{
	m_PostFXRenderer->SetFogType(Type);
}

void CRenderManager::SetFogStart(float Start)
{
	m_PostFXRenderer->SetFogStart(Start);
}

void CRenderManager::SetFogEnd(float End)
{
	m_PostFXRenderer->SetFogEnd(End);
}

void CRenderManager::SetFogDensity(float Density)
{
	m_PostFXRenderer->SetFogDensity(Density);
}

void CRenderManager::SetShadowBias(float Bias)
{
	m_ShadowCBuffer->SetBias(Bias);
}

void CRenderManager::SetAdaptationTime(float Time)
{
	m_PostFXRenderer->SetAdaptationTime(Time);
}

float CRenderManager::GetAdaptationTime() const
{
	return m_PostFXRenderer->GetAdaptationTime();
}

void CRenderManager::AddRenderList(CSceneComponent* Component)
{
	if (!Component->IsEnable())
		return;

	RenderLayer* Layer = nullptr;

	auto	iter = m_RenderLayerList.begin();
	auto	iterEnd = m_RenderLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Name == Component->GetLayerName())
		{
			Layer = *iter;
			break;
		}
	}

	if (!Layer)
		return;

	Layer->RenderList.push_back(Component);
}

void CRenderManager::CreateLayer(const std::string& Name, int Priority)
{
	RenderLayer* Layer = new RenderLayer;
	Layer->Name = Name;
	Layer->LayerPriority = Priority;

	m_RenderLayerList.push_back(Layer);

	sort(m_RenderLayerList.begin(), m_RenderLayerList.end(), CRenderManager::Sortlayer);
}

void CRenderManager::SetLayerPriority(const std::string& Name, int Priority)
{
	auto	iter = m_RenderLayerList.begin();
	auto	iterEnd = m_RenderLayerList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Name == Name)
		{
			(*iter)->LayerPriority = Priority;
			break;
		}
	}

	sort(m_RenderLayerList.begin(), m_RenderLayerList.end(), CRenderManager::Sortlayer);
}

bool CRenderManager::Init()
{
	m_RenderStateManager = new CRenderStateManager;

	m_RenderStateManager->Init();

	m_Standard2DCBuffer = new CStandard2DConstantBuffer;

	m_Standard2DCBuffer->Init();

	// �⺻ ���̾� ����
	RenderLayer* Layer = new RenderLayer;
	Layer->Name = "Back";
	Layer->LayerPriority = (int)RenderLayerType::Back;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Default";
	Layer->LayerPriority = (int)RenderLayerType::Default;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "PostDefault";
	Layer->LayerPriority = (int)RenderLayerType::PostDefault;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Transparency";
	Layer->LayerPriority = (int)RenderLayerType::Transparency;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Decal";
	Layer->LayerPriority = (int)RenderLayerType::Decal;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Particle";
	Layer->LayerPriority = (int)RenderLayerType::Particle;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "PostParticle";
	Layer->LayerPriority = (int)RenderLayerType::PostParticle;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "ScreenWidgetComponent";
	Layer->LayerPriority = (int)RenderLayerType::ScreenWidgetComponent;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "AnimationEditorLayer";
	Layer->LayerPriority = (int)RenderLayerType::AnimationEditor;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "ParticleEditorLayer";
	Layer->LayerPriority = (int)RenderLayerType::ParticleEditor;

	m_RenderLayerList.push_back(Layer);

	Layer = new RenderLayer;
	Layer->Name = "Collider";
	Layer->LayerPriority = (int)RenderLayerType::Collider;

	m_RenderLayerList.push_back(Layer);

	m_DepthDisable = m_RenderStateManager->FindRenderState("DepthDisable");
	m_AlphaBlend = m_RenderStateManager->FindRenderState("AlphaBlend");
	m_LightAccBlend = m_RenderStateManager->FindRenderState("LightAcc");

	// ���۵� �������� Ÿ�� ����
	Resolution RS = CDevice::GetInst()->GetResolution();

	if (!CResourceManager::GetInst()->CreateTarget("Diffuse",
		RS.Width, RS.Height, DXGI_FORMAT_R8G8B8A8_UNORM))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer1",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer2",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer3",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer4",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("GBuffer5",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	CRenderTarget* GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("Diffuse");
	GBufferTarget->SetPos(Vector3(0.f, 0.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer1");
	GBufferTarget->SetPos(Vector3(0.f, 100.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer2");
	GBufferTarget->SetPos(Vector3(0.f, 200.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer3");
	GBufferTarget->SetPos(Vector3(0.f, 300.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer4");
	GBufferTarget->SetPos(Vector3(0.f, 400.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer5");
	GBufferTarget->SetPos(Vector3(0.f, 500.f, 0.f));
	GBufferTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	GBufferTarget->SetDebugRender(true);
	m_vecGBuffer.push_back(GBufferTarget);

	// Decal�� �̹� ������ ����Ÿ�ٿ� �����Ѵ�.
	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("Diffuse");
	m_vecDecal.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer1");
	m_vecDecal.push_back(GBufferTarget);

	GBufferTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("GBuffer3");
	m_vecDecal.push_back(GBufferTarget);

	if (!CResourceManager::GetInst()->CreateTarget("LightDif",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("LightSpc",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	if (!CResourceManager::GetInst()->CreateTarget("LightEmv",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	CRenderTarget* LightTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("LightDif");
	LightTarget->SetPos(Vector3(100.f, 0.f, 0.f));
	LightTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	LightTarget->SetDebugRender(true);
	m_vecLightBuffer.push_back(LightTarget);

	LightTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("LightSpc");
	LightTarget->SetPos(Vector3(100.f, 100.f, 0.f));
	LightTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	LightTarget->SetDebugRender(true);
	m_vecLightBuffer.push_back(LightTarget);

	LightTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("LightEmv");
	LightTarget->SetPos(Vector3(100.f, 200.f, 0.f));
	LightTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	LightTarget->SetDebugRender(true);
	m_vecLightBuffer.push_back(LightTarget);

	if (!CResourceManager::GetInst()->CreateTarget("FinalScreen",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT))
		return false;

	m_FinalTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("FinalScreen");
	m_FinalTarget->SetPos(Vector3(200.f, 0.f, 0.f));
	m_FinalTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	m_FinalTarget->SetDebugRender(true);

	// Animation Editor �� Render Target => ������ Depth Buffer �� ����ϰ� �� ���̴�.
	if (!CResourceManager::GetInst()->CreateTarget("AnimationEditorRenderTarget",
		RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, true, DXGI_FORMAT_D24_UNORM_S8_UINT))
		return false;

	m_AnimEditorRenderTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("AnimationEditorRenderTarget");
	m_AnimEditorRenderTarget->SetPos(Vector3(450.f, 100.f, 0.f));
	m_AnimEditorRenderTarget->SetScale(Vector3(150.f, 150.f, 1.f));
	m_AnimEditorRenderTarget->SetDebugRender(false);

	// Particle Editor �� Render Target
	// if (!CResourceManager::GetInst()->CreateTarget("ParticleEffectRenderTarget",
	// 	RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT,  true, DXGI_FORMAT_D24_UNORM_S8_UINT))
	// 	return false;
	if (!CResourceManager::GetInst()->CreateTarget("ParticleEffectRenderTarget",
		600, 600, DXGI_FORMAT_R32G32B32A32_FLOAT,  true, DXGI_FORMAT_D24_UNORM_S8_UINT))
		return false;

	m_ParticleEffectEditorRenderTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("ParticleEffectRenderTarget");
	m_ParticleEffectEditorRenderTarget->SetPos(Vector3(600.f, 100.f, 0.f));
	m_ParticleEffectEditorRenderTarget->SetScale(Vector3(150.f, 150.f, 1.f));
	m_ParticleEffectEditorRenderTarget->SetDebugRender(false);
	m_ParticleEffectEditorRenderTarget->SetClearColor(0.6f, 0.6f, 0.6f, 0.6f);

	// Map ��¿� ������

	// Shadow
	if (!CResourceManager::GetInst()->CreateTarget("ShadowMap", (unsigned int)SHADOWMAP_WIDTH, (unsigned int)SHADOWMAP_HEIGHT,
		DXGI_FORMAT_R32G32B32A32_FLOAT, true, DXGI_FORMAT_D24_UNORM_S8_UINT))
	{
		assert(false);
		return false;
	}

	m_ShadowMapTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("ShadowMap");
	m_ShadowMapTarget->SetPos(Vector3(300.f, 0.f, 0.f));
	m_ShadowMapTarget->SetScale(Vector3(100.f, 100.f, 1.f));
	m_ShadowMapTarget->SetDebugRender(true);

	// Player Target
	CResourceManager::GetInst()->CreateTarget("PlayerTarget", RS.Width, RS.Height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_PlayerTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("PlayerTarget");

	m_LightBlendShader = CResourceManager::GetInst()->FindShader("LightBlendShader");
	m_LightBlendRenderShader = CResourceManager::GetInst()->FindShader("LightBlendRenderShader");
	m_Mesh3DNoLightRenderShader = CResourceManager::GetInst()->FindShader("Mesh3DNoLightShader");
	m_ShadowMapShader = CResourceManager::GetInst()->FindShader("ShadowMapShader");
	m_ShadowMapInstancingShader = CResourceManager::GetInst()->FindShader("ShadowMapInstancingShader");

	m_ShadowCBuffer = new CShadowCBuffer;
	m_ShadowCBuffer->Init();

	// Post FX Renderer
	m_PostFXRenderer = new CPostFXRenderer;
	m_PostFXRenderer->Init();

	// Toon Texture
	m_ToonRampTex = CResourceManager::GetInst()->FindTexture("ToonRampTex");

	// Fade in, out
	m_FadeCBuffer = new CFadeCBuffer;
	m_FadeCBuffer->Init();
	m_FadeInfo.StartColor = Vector3(0.f, 0.f, 0.f);
	m_FadeInfo.EndColor = Vector3(1.f, 1.f, 1.f);
	m_FadeInfo.Time = 1.f;

	return true;
}

void CRenderManager::Render(float DeltaTime)
{
	{
		auto	iter = m_RenderLayerList.begin();
		auto	iterEnd = m_RenderLayerList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->RenderList.clear();
			(*iter)->InstancingIndex = 0;

			auto iter1 = (*iter)->m_vecInstancing.begin();
			auto iter1End = (*iter)->m_vecInstancing.end();

			for (; iter1 != iter1End; ++iter1)
			{
				(*iter1)->RenderList.clear();
			}
		}
	}

	{
		auto	iter = m_ObjectList->begin();
		auto	iterEnd = m_ObjectList->end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->PrevRender();
		}	
	}

	m_PlayerTarget->ClearTarget();
	m_FinalTarget->ClearTarget();

	// �ν��Ͻ� ����Ʈ ������Ʈ
	UpdateInstancingList();

	if (m_RenderSkyBox)
	{
		// ȯ��� ���
		RenderSkyBox();
	}
	
	// Default Layer�� �ν��Ͻ� ������ �����.
	UpdateInstancingInfo((int)RenderLayerType::Default, true);

	// �׸��� ���� �����.
	RenderShadowMap();

	// GBuffer�� ������.
	RenderGBuffer();

	// Decal�� �׷�����.
	RenderDecal();

	// Ʈ������ ������ �Ѱ��־�� �Ѵ�.
	TransformCBuffer	TBuffer = {};

	CScene* Scene = CSceneManager::GetInst()->GetScene();
	CCameraComponent* Camera = Scene->GetCameraManager()->GetCurrentCamera();

	TBuffer.matView = Camera->GetViewMatrix();
	TBuffer.matProj = Camera->GetProjMatrix();
	TBuffer.matInvProj = TBuffer.matProj;
	TBuffer.matInvProj.Inverse();
	TBuffer.matWV = TBuffer.matView;
	TBuffer.matWVP = TBuffer.matView * TBuffer.matProj;
	TBuffer.matVP = TBuffer.matWVP;
	TBuffer.matInvVP = TBuffer.matWVP;
	TBuffer.matInvVP.Inverse();
	TBuffer.matInvWVP = TBuffer.matWVP;
	TBuffer.matInvWVP.Inverse();

	TBuffer.matView.Transpose();
	TBuffer.matProj.Transpose();
	TBuffer.matInvProj.Transpose();
	TBuffer.matWV.Transpose();
	TBuffer.matWVP.Transpose();
	TBuffer.matVP.Transpose();
	TBuffer.matInvVP.Transpose();
	TBuffer.matInvWVP.Transpose();

	CResourceManager::GetInst()->FindConstantBuffer("TransformCBuffer")->UpdateBuffer(&TBuffer);

	// ���� �������۸� ������.
	RenderLightAcc();

	// ���� �������ۿ� GBuffer�� �̿��Ͽ� ����ȭ���� ������.
	RenderLightBlend();

	// ������ ���̾��� �ν��Ͻ� ���� ����
	UpdateInstancingInfo((int)RenderLayerType::Transparency, false);

	// �������� ������Ʈ�� �׸���.
	RenderTransparent();

	// ��ƼŬ ���̾�
	RenderParticle();

	// Fade in out
	UpdateFadeEffectInfo(DeltaTime);

	// Bomb Effect �ð� ���
	if (m_BombEffect)
	{
		m_BombEffectTimer += DeltaTime;
		if (m_BombEffectTimer >= m_BombEffectTime)
		{
			m_BombEffect = false;
			m_BombEffectTimer = 0.f;

			// ������ �ٽ� �Ҵ�
			CLightComponent* GlobalLight = Scene->GetLightManager()->GetGlobalLightComponent();
			GlobalLight->SetColor(m_OriginGLightColor);
		}
	}

	if (m_PostProcessing)
	{
		// HDR, Bloom, Adaptation �� PostEffect ó��
		m_PostFXRenderer->Render(DeltaTime, m_FinalTarget, m_BombEffect);
	}
	else
	{
		// ������ ������Ʈ + ����ó�� + �ܰ��� ó���� ���� ȭ���� ����ۿ� �׷�����.
		RenderFinalScreen(DeltaTime);
	}

	// Post-Particle Layer ���
	RenderPostParticle();

	// Debug Mode �϶��� ������ + Editor ������ Render �ǰ� ����
#ifdef _DEBUG
	// Animation Editor Animation Instance ���ۿ� Render Target
	RenderAnimationEditor();

	// Particle Effect Editor ���ۿ� Render Target
	RenderParticleEffectEditor();
#endif

	// Screen Widget ���
	auto iter = m_RenderLayerList[(int)RenderLayerType::ScreenWidgetComponent]->RenderList.begin();
	auto iterEnd = m_RenderLayerList[(int)RenderLayerType::ScreenWidgetComponent]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}

	if (m_DebugRender)
	{
		// Collider Layer ���
		iter = m_RenderLayerList[(int)RenderLayerType::Collider]->RenderList.begin();
		iterEnd = m_RenderLayerList[(int)RenderLayerType::Collider]->RenderList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Render();
		}
	}

	{
		auto	iter1 = m_RenderLayerList.begin();
		auto	iter1End = m_RenderLayerList.end();

		for (; iter1 != iter1End; ++iter1)
		{
			auto	iter2 = (*iter1)->RenderList.begin();
			auto	iter2End = (*iter1)->RenderList.end();

			for (; iter2 != iter2End; ++iter2)
			{
				(*iter2)->PostRender();
			}
		}
	}


	// Widget ���
	m_DepthDisable->SetState();

	m_AlphaBlend->SetState();

	CSceneManager::GetInst()->GetScene()->GetViewport()->Render();

	if (m_DebugRender)
	{
		// ������ ����Ÿ���� ����Ѵ�.
		CResourceManager::GetInst()->RenderTarget();
	}

	// ���콺 ���
	CWidgetWindow* MouseWidget = CEngine::GetInst()->GetMouseWidget();

	if (MouseWidget)
		MouseWidget->Render();

	m_AlphaBlend->ResetState();

	m_DepthDisable->ResetState();
}

void CRenderManager::RenderSkyBox()
{
	CSharedPtr<CGameObject> SkyObj = CSceneManager::GetInst()->GetScene()->GetSkyObject();

 	m_FinalTarget->SetTarget(nullptr);

	SkyObj->Render();

	m_FinalTarget->ResetTarget();
}

void CRenderManager::RenderShadowMap()
{
	// ����Ʈ Shadowmap Textre�� ��ġ�ϵ���
	D3D11_VIEWPORT VP = {};

	VP.Width = SHADOWMAP_WIDTH;
	VP.Height = SHADOWMAP_HEIGHT;
	VP.MaxDepth = 1.f;

	CDevice::GetInst()->GetContext()->RSSetViewports(1, &VP);

	m_ShadowMapTarget->ClearTarget();
	m_ShadowMapTarget->SetTarget();
	
	m_ShadowMapShader->SetShader();

	for (size_t i = 0; i <= (int)RenderLayerType::Default; ++i)
	{
		auto iter = m_RenderLayerList[i]->RenderList.begin();
		auto iterEnd = m_RenderLayerList[i]->RenderList.end();

		for (; iter != iterEnd; ++iter)
		{
			if (!(*iter)->IsDrawShadow())
			{
				continue;
			}

			(*iter)->RenderShadowMap();
		}
	}

	RenderDefaultInstancingShadow();

	m_ShadowMapTarget->ResetTarget();

	D3D11_VIEWPORT PrevVP = {};

	PrevVP.Width = (float)CDevice::GetInst()->GetResolution().Width;
	PrevVP.Height = (float)CDevice::GetInst()->GetResolution().Height;
	PrevVP.MaxDepth = 1.f;

	CDevice::GetInst()->GetContext()->RSSetViewports(1, &PrevVP);
}

void CRenderManager::RenderGBuffer()
{
	// ���� ����۷� ����Ÿ���� �����Ǿ� �ִ�.
	// �̸� GBuffer ����Ÿ������ ��ü�ؾ� �Ѵ�.
	std::vector<ID3D11RenderTargetView*>	vecTarget;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget;
	ID3D11DepthStencilView* PrevDepthTarget = nullptr;

	size_t	GBufferSize = m_vecGBuffer.size();

	vecPrevTarget.resize(GBufferSize);

	for (size_t i = 0; i < GBufferSize; ++i)
	{
		m_vecGBuffer[i]->ClearTarget();
		vecTarget.push_back(m_vecGBuffer[i]->GetTargetView());
	}

	// ���� �����Ǿ� �ִ� ����Ÿ�ٰ� ����Ÿ���� ���´�.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget[0], &PrevDepthTarget);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecTarget[0], PrevDepthTarget);

	for (size_t i = 0; i <= (int)RenderLayerType::PostDefault; ++i)
	{
		auto	iter = m_RenderLayerList[i]->RenderList.begin();
		auto	iterEnd = m_RenderLayerList[i]->RenderList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Render();

		}
	}

	// Default Layer Instancing
	RenderInstancing((int)RenderLayerType::Default, false);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget[0], PrevDepthTarget);

	SAFE_RELEASE(PrevDepthTarget);
	for (size_t i = 0; i < GBufferSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget[i]);
	}
}

void CRenderManager::RenderDecal()
{
	std::vector<ID3D11RenderTargetView*>	vecTarget;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget;
	ID3D11DepthStencilView* PrevDepthTarget = nullptr;

	size_t	DecalSize = m_vecDecal.size();

	vecPrevTarget.resize(DecalSize);

	for (size_t i = 0; i < DecalSize; ++i)
	{
		vecTarget.push_back(m_vecDecal[i]->GetTargetView());
	}

	// ���� �����Ǿ� �ִ� ����Ÿ�ٰ� ����Ÿ���� ���´�.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)DecalSize,
		&vecPrevTarget[0], &PrevDepthTarget);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)DecalSize,
		&vecTarget[0], PrevDepthTarget);

	m_vecGBuffer[2]->SetTargetShader(10);
	m_vecGBuffer[4]->SetTargetShader(11);
	m_vecGBuffer[5]->SetTargetShader(12);

	auto	iter = m_RenderLayerList[(int)RenderLayerType::Decal]->RenderList.begin();
	auto	iterEnd = m_RenderLayerList[(int)RenderLayerType::Decal]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}

	m_vecGBuffer[2]->ResetTargetShader(10);
	m_vecGBuffer[4]->ResetTargetShader(11);
	m_vecGBuffer[5]->ResetTargetShader(12);


	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)DecalSize,
		&vecPrevTarget[0], PrevDepthTarget);

	SAFE_RELEASE(PrevDepthTarget);
	for (size_t i = 0; i < DecalSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget[i]);
	}

	// ����� ���
#ifdef _DEBUG

	std::vector<ID3D11RenderTargetView*>	vecTarget1;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget1;
	ID3D11DepthStencilView* PrevDepthTarget1 = nullptr;

	size_t	GBufferSize = m_vecGBuffer.size();

	vecPrevTarget1.resize(GBufferSize);

	for (size_t i = 0; i < GBufferSize; ++i)
	{
		vecTarget1.push_back(m_vecGBuffer[i]->GetTargetView());
	}

	// ���� �����Ǿ� �ִ� ����Ÿ�ٰ� ����Ÿ���� ���´�.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget1[0], &PrevDepthTarget1);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecTarget1[0], PrevDepthTarget1);

	iter = m_RenderLayerList[(int)RenderLayerType::Decal]->RenderList.begin();
	iterEnd = m_RenderLayerList[(int)RenderLayerType::Decal]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->RenderDebug();
	}

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget1[0], PrevDepthTarget1);

	SAFE_RELEASE(PrevDepthTarget1);
	for (size_t i = 0; i < GBufferSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget1[i]);
	}

#endif // _DEBUG
}

void CRenderManager::RenderLightAcc()
{
	std::vector<ID3D11RenderTargetView*>	vecTarget;
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget;
	ID3D11DepthStencilView* PrevDepthTarget = nullptr;

	size_t	LightBufferSize = m_vecLightBuffer.size();

	vecPrevTarget.resize(LightBufferSize);

	for (size_t i = 0; i < LightBufferSize; ++i)
	{
		m_vecLightBuffer[i]->ClearTarget();
		vecTarget.push_back(m_vecLightBuffer[i]->GetTargetView());
	}

	// ���� �����Ǿ� �ִ� ����Ÿ�ٰ� ����Ÿ���� ���´�.
	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)LightBufferSize,
		&vecPrevTarget[0], &PrevDepthTarget);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)LightBufferSize,
		&vecTarget[0], PrevDepthTarget);


	m_LightAccBlend->SetState();

	m_DepthDisable->SetState();

	// Toon Ramp Texture Bind
	m_ToonRampTex->SetShader(4, (int)Buffer_Shader_Type::Pixel, 0);

	// SkyBox Texture
	CSkyObject* SkyObj = (CSkyObject*)CSceneManager::GetInst()->GetScene()->GetSkyObject();
	CTexture* SkyTex = SkyObj->GetSkyTexture();
	SkyTex->SetShader(23, (int)Buffer_Shader_Type::Pixel, 0);

	m_vecGBuffer[0]->SetTargetShader(14);
	m_vecGBuffer[1]->SetTargetShader(15);
	m_vecGBuffer[2]->SetTargetShader(16);
	m_vecGBuffer[3]->SetTargetShader(17);
	m_vecGBuffer[4]->SetTargetShader(18);
	m_vecGBuffer[5]->SetTargetShader(19);

	CSceneManager::GetInst()->GetScene()->GetLightManager()->Render();

	m_vecGBuffer[0]->ResetTargetShader(14);
	m_vecGBuffer[1]->ResetTargetShader(15);
	m_vecGBuffer[2]->ResetTargetShader(16);
	m_vecGBuffer[3]->ResetTargetShader(17);
	m_vecGBuffer[4]->ResetTargetShader(18);
	m_vecGBuffer[5]->ResetTargetShader(19);

	m_ToonRampTex->ResetShader(4, (int)Buffer_Shader_Type::Pixel, 0);

	m_DepthDisable->ResetState();

	m_LightAccBlend->ResetState();
	SkyTex->ResetShader(23, (int)Buffer_Shader_Type::Pixel, 0);


	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)LightBufferSize,
		&vecPrevTarget[0], PrevDepthTarget);

	SAFE_RELEASE(PrevDepthTarget);
	for (size_t i = 0; i < LightBufferSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget[i]);
	}
}

void CRenderManager::RenderLightBlend()
{
	m_FinalTarget->SetTarget(nullptr);

	m_vecGBuffer[0]->SetTargetShader(14);
	m_vecGBuffer[2]->SetTargetShader(16);
	m_vecLightBuffer[0]->SetTargetShader(18);
	m_vecLightBuffer[1]->SetTargetShader(19);
	m_vecLightBuffer[2]->SetTargetShader(20);
	m_ShadowMapTarget->SetTargetShader(22);

	m_LightBlendShader->SetShader();

	m_DepthDisable->SetState();

	CScene* Scene = CSceneManager::GetInst()->GetScene();
	Matrix matView = Scene->GetCameraManager()->GetCurrentCamera()->GetShadowViewMatrix();
	Matrix matProj = Scene->GetCameraManager()->GetCurrentCamera()->GetShadowProjMatrix();

	m_ShadowCBuffer->SetShadowVP(matView * matProj);
	m_ShadowCBuffer->UpdateCBuffer();

	// �Ʒ��ڵ带 ����� ������, Null Buffer �� ����ϱ� �����̴�.
	UINT Offset = 0;
	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	CDevice::GetInst()->GetContext()->Draw(4, 0);

	m_DepthDisable->ResetState();

	m_vecGBuffer[0]->ResetTargetShader(14);
	m_vecGBuffer[2]->ResetTargetShader(16);
	m_vecLightBuffer[0]->ResetTargetShader(18);
	m_vecLightBuffer[1]->ResetTargetShader(19);
	m_vecLightBuffer[2]->ResetTargetShader(20);
	m_ShadowMapTarget->ResetTargetShader(22);

	m_FinalTarget->ResetTarget();
}

void CRenderManager::RenderTransparent()
{
	// z����
	if (m_RenderLayerList[(int)RenderLayerType::Transparency]->RenderList.size() >= 2)
	{
		m_RenderLayerList[(int)RenderLayerType::Transparency]->RenderList.sort(CRenderManager::SortZ);
	}

	m_FinalTarget->SetTarget();

	// ��� Light���� ����ȭ ���ۿ� ��Ƽ� �ѱ��.
	CSceneManager::GetInst()->GetScene()->GetLightManager()->SetForwardRenderShader();

	// Forward Rendering
	auto iter = m_RenderLayerList[(int)RenderLayerType::Transparency]->RenderList.begin();
	auto iterEnd = m_RenderLayerList[(int)RenderLayerType::Transparency]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}

	// Transparent Layer Instancing
	RenderInstancing((int)RenderLayerType::Transparency, true);

	CSceneManager::GetInst()->GetScene()->GetLightManager()->ResetForwardRenderShader();

	m_FinalTarget->ResetTarget();
}

void CRenderManager::RenderFinalScreen(float DeltaTime)
{

	m_FinalTarget->SetTargetShader(21);
	m_vecGBuffer[2]->SetTargetShader(22);
	m_PlayerTarget->SetTargetShader(23);

	m_LightBlendRenderShader->SetShader();

	m_DepthDisable->SetState();

	// ������ �׸��� ���� Null Buffer �� ����Ͽ� �׷�����.
	UINT Offset = 0;
	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	CDevice::GetInst()->GetContext()->Draw(4, 0);

	m_DepthDisable->ResetState();

	m_FinalTarget->ResetTargetShader(21);
	m_vecGBuffer[2]->ResetTargetShader(22);
	m_PlayerTarget->ResetTargetShader(23);
}

void CRenderManager::RenderAnimationEditor()
{
	// Editor Mode �϶��� Render �ϱ� 
	if (CEngine::GetInst()->GetEditMode() == false)
		return;

	int AnimationEditorLayerIdx = GetRenderLayerIndex("AnimationEditorLayer");

	// ��~�࿡ �ش� Layer �� Idx �� ������ ���� �ʴٸ�
	if (AnimationEditorLayerIdx == -1)
		return;

	// Animation Edtior �󿡼� Animation Editor ���� ������ �ʴٸ�
	if (m_RenderLayerList[AnimationEditorLayerIdx]->RenderList.size() <= 0)
		return;

	// Render Target ��ü
	m_AnimEditorRenderTarget->ClearTarget();

	// Animation Editor Render Target ������ Depth Buffer �� ����ϰ� �� �Ѵ�.
	// �׷��� ������, ���������� �׷����� Scene ������ Depth Buffer �� ����ϰ� �ǹ�����.
	// m_AnimEditorRenderTarget->SetTarget(nullptr);
	m_AnimEditorRenderTarget->SetTarget();

	m_Mesh3DNoLightRenderShader->SetShader();

	auto iter = m_RenderLayerList[AnimationEditorLayerIdx]->RenderList.begin();
	auto iterEnd = m_RenderLayerList[AnimationEditorLayerIdx]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->RenderAnimationEditor();
	}

	m_AnimEditorRenderTarget->ResetTarget();
 }

void CRenderManager::RenderParticleEffectEditor()
{
	// Editor Mode �϶��� Render �ϱ� 
	if (CEngine::GetInst()->GetEditMode() == false)
		return;

	// ����Ʈ Shadowmap Textre�� ��ġ�ϵ���
	D3D11_VIEWPORT VP = {};

	VP.Width  = (float)m_ParticleEffectEditorRenderTarget->GetWidth();
	VP.Height = (float)m_ParticleEffectEditorRenderTarget->GetHeight();
	VP.MaxDepth = 1.f;

	// ���� View Port ũ�� 
	D3D11_VIEWPORT PrevVP = {};

	PrevVP.Width = (float)CDevice::GetInst()->GetResolution().Width;
	PrevVP.Height = (float)CDevice::GetInst()->GetResolution().Height;
	PrevVP.MaxDepth = 1.f;

	CDevice::GetInst()->GetContext()->RSSetViewports(1, &VP);

	int ParticleEffectEditorLayerIdx = GetRenderLayerIndex("ParticleEditorLayer");

	// ��~�࿡ �ش� Layer �� Idx �� ������ ���� �ʴٸ�
	if (ParticleEffectEditorLayerIdx == -1)
	{
		CDevice::GetInst()->GetContext()->RSSetViewports(1, &PrevVP);
		return;
	}

	// Animation Edtior �󿡼� Animation Editor ���� ������ �ʴٸ�
	if (m_RenderLayerList[ParticleEffectEditorLayerIdx]->RenderList.size() <= 0)
	{
		CDevice::GetInst()->GetContext()->RSSetViewports(1, &PrevVP);
		return;
	}

	// Render Target ��ü
	m_ParticleEffectEditorRenderTarget->ClearTarget();

	// ������ Depth Target ����ϱ� 
	m_ParticleEffectEditorRenderTarget->SetTarget();

	auto iter = m_RenderLayerList[ParticleEffectEditorLayerIdx]->RenderList.begin();
	auto iterEnd = m_RenderLayerList[ParticleEffectEditorLayerIdx]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable())
			continue;
		(*iter)->RenderParticleEffectEditor();
	}

	m_ParticleEffectEditorRenderTarget->ResetTarget();

	CDevice::GetInst()->GetContext()->RSSetViewports(1, &PrevVP);
}

void CRenderManager::RenderPostParticle()
{
	// Post - ��ƼŬ ���̾� ���
	auto	iter = m_RenderLayerList[(int)RenderLayerType::PostParticle]->RenderList.begin();
	auto	iterEnd = m_RenderLayerList[(int)RenderLayerType::PostParticle]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderManager::RenderParticle()
{
	// Final Target �� �׸��� 
	m_FinalTarget->SetTarget();

	m_vecGBuffer[2]->SetShader(10, (int)Buffer_Shader_Type::Pixel, 0);

	m_AlphaBlend->SetState();

	// ��ƼŬ ���̾� ���
	auto	iter = m_RenderLayerList[(int)RenderLayerType::Particle]->RenderList.begin();
	auto	iterEnd = m_RenderLayerList[(int)RenderLayerType::Particle]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->IsEnable() == false)
			continue;

		(*iter)->Render();
	}

	m_AlphaBlend->ResetState();

	m_vecGBuffer[2]->ResetShader(10, (int)Buffer_Shader_Type::Pixel, 0);

	m_FinalTarget->ResetTarget();
}

void CRenderManager::RenderPlayer(CMesh* PlayerMesh)
{
	// GBuffer Ÿ���� ��� ��ü
	std::vector<ID3D11RenderTargetView*>	vecPrevTarget;
	ID3D11RenderTargetView* PlayerTarget = m_PlayerTarget->GetTargetView();
	ID3D11DepthStencilView* PrevDepthTarget = nullptr;

	size_t	GBufferSize = m_vecGBuffer.size();
	vecPrevTarget.resize(GBufferSize);

	CDevice::GetInst()->GetContext()->OMGetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget[0], &PrevDepthTarget);

	CDevice::GetInst()->GetContext()->OMSetRenderTargets(1,
		&PlayerTarget, PrevDepthTarget);

	m_DepthDisable->SetState();
	m_ShadowMapShader->SetShader();

	PlayerMesh->Render();

	m_DepthDisable->ResetState();

	CDevice::GetInst()->GetContext()->OMSetRenderTargets((unsigned int)GBufferSize,
		&vecPrevTarget[0], PrevDepthTarget);

	SAFE_RELEASE(PrevDepthTarget);
	for (size_t i = 0; i < GBufferSize; ++i)
	{
		SAFE_RELEASE(vecPrevTarget[i]);
	}
}

void CRenderManager::SetBlendFactor(const std::string& Name, float r, float g,
	float b, float a)
{
	m_RenderStateManager->SetBlendFactor(Name, r, g, b, a);
}

void CRenderManager::AddBlendInfo(const std::string& Name, bool BlendEnable,
	D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp, 
	D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, 
	D3D11_BLEND_OP BlendOpAlpha, UINT8 RenderTargetWriteMask)
{
	m_RenderStateManager->AddBlendInfo(Name, BlendEnable, SrcBlend, DestBlend,
		BlendOp, SrcBlendAlpha, DestBlendAlpha, BlendOpAlpha, RenderTargetWriteMask);
}

bool CRenderManager::CreateBlendState(const std::string& Name, 
	bool AlphaToCoverageEnable, bool IndependentBlendEnable)
{
	return m_RenderStateManager->CreateBlendState(Name, AlphaToCoverageEnable, IndependentBlendEnable);
}

int CRenderManager::GetRenderLayerIndex(const std::string& TargetName)
{
	for (size_t j = 0; j < m_RenderLayerList.size(); ++j)
	{
		if (m_RenderLayerList[j]->Name == TargetName)
			return (int)j;
	}

	return -1;
}

CRenderState* CRenderManager::FindRenderState(const std::string& Name)
{
	return m_RenderStateManager->FindRenderState(Name);
}

void CRenderManager::RenderInstancing(int LayerIndex, bool AlphaBlend)
{
	int InstancingIndex = m_RenderLayerList[LayerIndex]->InstancingIndex;

	// ���� Default Layer �� �ִ� Instancing����. ���� ��ȸ�� ���̴�.
	for (int i = 0; i < InstancingIndex; ++i)
	{
		RenderInstancingList* InstancingList = m_RenderLayerList[LayerIndex]->m_vecInstancing[i];
		// Material Slot ����ŭ �ݺ��Ѵ�.
		int	SlotCount = 0;

		if (InstancingList->RenderList.empty())
		{
			continue;
		}

		if (InstancingList->Mesh->GetMeshType() == Mesh_Type::Static)
		{
			SlotCount = ((CStaticMeshComponent*)InstancingList->RenderList.back())->GetMaterialSlotCount();
		}

		else if (m_RenderLayerList[LayerIndex]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Animation)
		{
			SlotCount = ((CAnimationMeshComponent*)InstancingList->RenderList.back())->GetMaterialSlotCount();
		}

		for (int j = 0; j < SlotCount; ++j)
		{
			CMaterial* Material = nullptr;

			if (m_RenderLayerList[LayerIndex]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Static)
			{
				Material = ((CStaticMeshComponent*)InstancingList->RenderList.back())->GetMaterial(j);
			}

			else if (m_RenderLayerList[LayerIndex]->m_vecInstancing[i]->Mesh->GetMeshType() == Mesh_Type::Animation)
			{
				Material = ((CAnimationMeshComponent*)InstancingList->RenderList.back())->GetMaterial(j);
			}

			if (Material)
				Material->RenderTexture();

			if (m_RenderLayerList[LayerIndex]->m_vecInstancing[i]->Animation)
			{
				((CAnimationMesh*)InstancingList->Mesh)->SetBoneShader();
			}

			if (AlphaBlend)
			{
				m_AlphaBlend->SetState();
			}
			
			// �ش� Material�� Instancing Shader�� ����
			// Transparent�� ��� ��� Material�� Transparent Instancing Shader�� ������ �ִ�. (Mesh Component �ܿ��� ó��)
			CGraphicShader* InstancingShader = (InstancingList->pvecInstancingShader->at(j));
			InstancingShader->SetShader();

			// Shader Parameter ������Ʈ
			ShaderParams ShaderParams = (InstancingList->pvecShaderParams->at(j));
			InstancingShader->SetShaderParams(ShaderParams);

			InstancingList->CBuffer->SetMaterialIndex(j);

			InstancingList->CBuffer->UpdateCBuffer();

			InstancingList->Buffer->SetShader();

			InstancingList->Mesh->RenderInstancing((int)InstancingList->RenderList.size(), j);

			InstancingList->Buffer->ResetShader();

			if (AlphaBlend)
			{
				m_AlphaBlend->ResetState();
			}

			if (InstancingList->Animation)
			{
				((CAnimationMesh*)InstancingList->Mesh)->ResetBoneShader();
			}

			if (Material)
				Material->Reset();
		}
	}
}

void CRenderManager::UpdateInstancingList()
{
	const std::list<InstancingCheckCount*>* InstancingList = CSceneComponent::GetInstancingCheckList();

	auto	iter = InstancingList->begin();
	auto	iterEnd = InstancingList->end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->InstancingList.size() >= 10)
		{
			RenderLayer* Layer = nullptr;

			size_t	Size = m_RenderLayerList.size();

			for (size_t i = 0; i < Size; ++i)
			{
				if (m_RenderLayerList[i]->Name == (*iter)->LayerName)
				{
					Layer = m_RenderLayerList[i];
					break;
				}
			}

			if (Layer)
			{
				if (Layer->m_vecInstancing.size() == Layer->InstancingIndex)
				{
					Layer->m_vecInstancing.resize(Layer->InstancingIndex * 2);

					for (int i = 0; i < Layer->InstancingIndex; ++i)
					{
						Layer->m_vecInstancing[Layer->InstancingIndex + i] = new RenderInstancingList;
					}
				}

				const std::list<class CSceneComponent*>& InstancingCompList = (*iter)->InstancingList;
				int RecommendSBufferSize = (int)InstancingCompList.size();

				if ((*iter)->Mesh->GetMeshType() == Mesh_Type::Static)
				{
					RecommendSBufferSize *= ((CStaticMeshComponent*)InstancingCompList.back())->GetMaterialSlotCount();
				}
				else if ((*iter)->Mesh->GetMeshType() == Mesh_Type::Animation)
				{
					RecommendSBufferSize *= ((CAnimationMeshComponent*)InstancingCompList.back())->GetMaterialSlotCount();
				}

				if (RecommendSBufferSize > Layer->m_vecInstancing[Layer->InstancingIndex]->BufferCount)
				{
					int	Count = (int)(Layer->m_vecInstancing[Layer->InstancingIndex]->BufferCount * 1.5f);

					if ((*iter)->InstancingList.size() > Count)
					{
						Count = RecommendSBufferSize;
					}

					SAFE_DELETE(Layer->m_vecInstancing[Layer->InstancingIndex]->Buffer);
					SAFE_DELETE(Layer->m_vecInstancing[Layer->InstancingIndex]->ShadowBuffer);

					Layer->m_vecInstancing[Layer->InstancingIndex]->Buffer = new CStructuredBuffer;
					Layer->m_vecInstancing[Layer->InstancingIndex]->Buffer->Init("InstancingBuffer", sizeof(Instancing3DInfo),
						Count, 40, true,
						(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);

					Layer->m_vecInstancing[Layer->InstancingIndex]->ShadowBuffer = new CStructuredBuffer;
					Layer->m_vecInstancing[Layer->InstancingIndex]->Buffer->Init("InstancingShadowBuffer", sizeof(Instancing3DInfo),
						Count, 40, true,
						(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);

					Layer->m_vecInstancing[Layer->InstancingIndex]->BufferCount = Count;
				}

				CScene* CurScene = CSceneManager::GetInst()->GetScene();

				auto	iter1 = (*iter)->InstancingList.begin();
				auto	iter1End = (*iter)->InstancingList.end();

				int ObjCount = 0;
				for (; iter1 != iter1End; ++iter1)
				{
					if (CurScene != (*iter1)->GetScene())
					{
						continue;
					}

					Layer->m_vecInstancing[Layer->InstancingIndex]->RenderList.push_back(*iter1);
					++ObjCount;
				}

				Layer->m_vecInstancing[Layer->InstancingIndex]->Mesh = (*iter)->Mesh;
				Layer->m_vecInstancing[Layer->InstancingIndex]->pvecInstancingShader = &(*iter)->vecInstancingShader;
				Layer->m_vecInstancing[Layer->InstancingIndex]->pvecShaderParams = &(*iter)->vecShaderParams;

				if ((*iter)->Mesh->CheckType<CAnimationMesh>())
				{
					Layer->m_vecInstancing[Layer->InstancingIndex]->Animation = true;
					Layer->m_vecInstancing[Layer->InstancingIndex]->CBuffer->SetBoneCount(((CAnimationMesh*)(*iter)->Mesh)->GetBoneCount());
				}

				Layer->m_vecInstancing[Layer->InstancingIndex]->CBuffer->SetObjectCount(ObjCount);

				++Layer->InstancingIndex;
			}
		}
	}
}

void CRenderManager::UpdateInstancingInfo(int LayerIndex, bool UpdateShadow)
{
	int InstancingIndex = m_RenderLayerList[LayerIndex]->InstancingIndex;

	for (int i = 0; i < InstancingIndex; ++i)
	{
		RenderInstancingList* InstancingList = m_RenderLayerList[LayerIndex]->m_vecInstancing[i];

		if (InstancingList->RenderList.empty())
		{
			continue;
		}

		CSceneComponent* RenderComponent = InstancingList->RenderList.back();

		// Material Slot ����ŭ �ݺ��Ѵ�.
		int	SlotCount = 0;
		if (InstancingList->Mesh->GetMeshType() == Mesh_Type::Static)
		{
			SlotCount = ((CStaticMeshComponent*)RenderComponent)->GetMaterialSlotCount();
		}

		else if (InstancingList->Mesh->GetMeshType() == Mesh_Type::Animation)
		{
			SlotCount = ((CAnimationMeshComponent*)RenderComponent)->GetMaterialSlotCount();
		}

		// �����ؾ� �� ������Ʈ �� * ������Ʈ �� Material ����ŭ ����ȭ ���� �Ҵ�
		int SBufferSize = (int)(InstancingList->RenderList.size()) * SlotCount;

		std::vector<Instancing3DInfo>	vecInfo;
		std::vector<Instancing3DInfo>	vecShadowInfo;

		vecInfo.reserve(SBufferSize);

		// �׸��� ���� ���� 
		bool DrawShadow = RenderComponent->IsDrawShadow();

		if (DrawShadow)
		{
			// ShadowBuffer
			vecShadowInfo.reserve(SBufferSize);
		}

		for (int j = 0; j < SlotCount; ++j)
		{
			auto	iter = InstancingList->RenderList.begin();
			auto	iterEnd = InstancingList->RenderList.end();

			CMaterial* Material = nullptr;

			for (; iter != iterEnd; ++iter)
			{
				if (InstancingList->Mesh->GetMeshType() == Mesh_Type::Static)
				{
					Material = ((CStaticMeshComponent*)(*iter))->GetMaterial(j);
				}

				else if (InstancingList->Mesh->GetMeshType() == Mesh_Type::Animation)
				{
					Material = ((CAnimationMeshComponent*)(*iter))->GetMaterial(j);
				}

				Instancing3DInfo	Info = {};

				(*iter)->SetInstancingInfo(&Info);
				Material->GetCBuffer()->SetInstancingInfo(&Info);

				// �����۹� ����
				if (Info.MtrlPaperBurnEnable == 1)
				{
					CPaperBurnComponent* PaperBurn = (*iter)->GetGameObject()->FindComponentFromType<CPaperBurnComponent>();
					if (PaperBurn)
					{
						PaperBurn->SetInstancingInfo(&Info);
					}
				}

				vecInfo.push_back(Info);

				Instancing3DInfo	ShadowInfo = {};

				(*iter)->SetInstancingShadowInfo(&ShadowInfo);
				Material->GetCBuffer()->SetInstancingInfo(&ShadowInfo);

				if (DrawShadow)
				{
					vecShadowInfo.push_back(ShadowInfo);
				}
			}
		}

		InstancingList->Buffer->UpdateBuffer(&vecInfo[0],
			(int)SBufferSize);

		if (DrawShadow)
		{
			InstancingList->ShadowBuffer->UpdateBuffer(&vecShadowInfo[0],
				(int)SBufferSize);
		}
	}
}

void CRenderManager::UpdateFadeEffectInfo(float DeltaTime)
{
	// ���� ���̵� ����Ʈ�� ��ϵ� ��ŸƮ �ݹ� ȣ��
	if (m_FadeEffectStart)
	{
		if (m_FadeEffectTimer == 0.f && m_FadeInfo.StartCallBack)
		{
			m_FadeInfo.StartCallBack();
			m_FadeInfo.StartCallBack = nullptr;
		}

		m_FadeEffectTimer += DeltaTime;

		if (DeltaTime >= (int)m_FadeInfo.Time)
		{
			m_FadeEffectTimer = 0.f;
		}

		float Ratio = 0.f;

		// ���̵� ���� ��� End Color -> Start Color��
		if (m_CurFadeEffectType == FadeEffecType::FADE_IN)
		{
			Ratio = m_FadeEffectTimer / m_FadeInfo.Time;
		}
		// ���̵� �ƿ��� ��� Start Color -> End Color��
		else if (m_CurFadeEffectType == FadeEffecType::FADE_OUT)
		{
			Ratio = 1.f - (m_FadeEffectTimer / m_FadeInfo.Time);
		}

		m_FadeCBuffer->SetFadeRatio(Ratio);

		// ���̵� ����Ʈ ���� �ݹ� ȣ�� �� ���̵� ����
		if (m_FadeEffectTimer >= m_FadeInfo.Time)
		{
			if (m_FadeInfo.EndCallBack)
			{
				m_FadeInfo.EndCallBack();
				m_FadeInfo.EndCallBack = nullptr;
			}

			// Stay ���¶��, Effect�� �������� �ʰ�, ���� �÷��� �����Ѵ�.
			if (m_FadeInfo.Stay)
			{
				m_FadeEffectTimer = 1.f;
			}
			else
			{
				m_FadeEffectTimer = 0.f;
				m_FadeEffectStart = false;
				m_FadeCBuffer->SetFadeStart(false);
			}
		}

		m_FadeCBuffer->UpdateCBuffer();
	}
}

void CRenderManager::RenderDefaultInstancingShadow()
{
	int InstancingIndex = m_RenderLayerList[(int)RenderLayerType::Default]->InstancingIndex;

	for (int i = 0; i < InstancingIndex; ++i)
	{
		RenderInstancingList* InstancingList = m_RenderLayerList[(int)RenderLayerType::Default]->m_vecInstancing[i];

		if (InstancingList->RenderList.empty())
		{
			continue;
		}

		bool DrawShadow = InstancingList->RenderList.back()->IsDrawShadow();

		if (!DrawShadow)
		{
			continue;
		}

		// Material Slot ����ŭ �ݺ��Ѵ�.
		int	SlotCount = 0;
		if (InstancingList->Mesh->GetMeshType() == Mesh_Type::Static)
		{
			SlotCount = ((CStaticMeshComponent*)InstancingList->RenderList.back())->GetMaterialSlotCount();
		}

		else if (InstancingList->Mesh->GetMeshType() == Mesh_Type::Animation)
		{
			SlotCount = ((CAnimationMeshComponent*)InstancingList->RenderList.back())->GetMaterialSlotCount();
		}

		for (int j = 0; j < SlotCount; ++j)
		{
			if (InstancingList->Animation)
			{
				((CAnimationMesh*)InstancingList->Mesh)->SetBoneShader();
			}

			m_ShadowMapInstancingShader->SetShader();

			InstancingList->CBuffer->UpdateCBuffer();

			InstancingList->ShadowBuffer->SetShader();

			InstancingList->Mesh->RenderInstancing((int)InstancingList->RenderList.size(), j);

			InstancingList->ShadowBuffer->ResetShader();

			if (InstancingList->Animation)
			{
				((CAnimationMesh*)InstancingList->Mesh)->ResetBoneShader();
			}
		}
	}
}

bool CRenderManager::Sortlayer(RenderLayer* Src, RenderLayer* Dest)
{
	return Src->LayerPriority < Dest->LayerPriority;
}

bool CRenderManager::SortZ(CSceneComponent* Src, CSceneComponent* Dest)
{
	return Src->GetViewZ() > Dest->GetViewZ();
}

 //void CRenderManager::RenderGray()
 //{
 //	CRenderTarget* FinalScreenTarget = (CRenderTarget*)CResourceManager::GetInst()->FindTexture("FinalScreen");
 //	FinalScreenTarget->SetTargetShader(21);
 //
 //	m_DepthDisable->SetState();
 //	m_GrayShader->SetShader();
 //
 //	UINT Offset = 0;
 //	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
 //	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
 //	CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
 //
 //	CDevice::GetInst()->GetContext()->Draw(4, 0);
 //
 //	m_DepthDisable->ResetState();
 //	FinalScreenTarget->ResetTargetShader(21);
 //}

/*
void CRenderManager::RenderParticleEffectEditor()
{
	int ParticleEffectEditorLayerIdx = GetRenderLayerIndex("ParticleEditorLayer");

	// ��~�࿡ �ش� Layer �� Idx �� ������ ���� �ʴٸ�
	if (ParticleEffectEditorLayerIdx == -1)
		return;

	// Animation Edtior �󿡼� Animation Editor ���� ������ �ʴٸ�
	if (m_RenderLayerList[ParticleEffectEditorLayerIdx]->RenderList.size() <= 0)
		return;

	// Render Target ��ü
	m_ParticleEffectEditorRenderTarget->ClearTarget();

	m_ParticleEffectEditorRenderTarget->SetTarget(nullptr);

	auto iter = m_RenderLayerList[ParticleEffectEditorLayerIdx]->RenderList.begin();
	auto iterEnd = m_RenderLayerList[ParticleEffectEditorLayerIdx]->RenderList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->RenderParticleEffectEditor();
	}

	CSharedPtr<CGameObject> SkyObj = CSceneManager::GetInst()->GetScene()->GetSkyObject();
	SkyObj->GetRootComponent()->RenderParticleEffectEditor();

	// Sample Sky �׸���

	m_ParticleEffectEditorRenderTarget->ResetTarget();
}
*/