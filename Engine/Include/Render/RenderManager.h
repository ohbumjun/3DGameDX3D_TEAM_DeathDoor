#pragma once

#include "../GameInfo.h"
#include "../Resource/Texture/RenderTarget.h"
#include "../Resource/Shader/InstancingCBuffer.h"
#include "../Resource/Shader/StructuredBuffer.h"
#include "../Resource/Shader/OutlineConstantBuffer.h"

enum class RenderLayerType
{
	Back,
	Default,
	PostDefault,
	Transparency,
	Decal,
	Particle,
	PostParticle,
	ScreenWidgetComponent,
	AnimationEditor,
	ParticleEditor,
	Collider
};

enum class FadeEffecType
{
	FADE_IN,
	FADE_OUT,
	NONE,
};

struct FadeEffectInfo
{
	bool Stay;								// Fade 끝 상태에서 기다릴 것인지
	float Time;
	Vector3 StartColor;
	Vector3 EndColor;
	std::function<void()> StartCallBack;
	std::function<void()> EndCallBack;
};

struct RenderInstancingList
{
	std::list<class CSceneComponent*> RenderList;
	class CMesh* Mesh;
	std::vector<class CGraphicShader*>* pvecInstancingShader;
	std::vector<ShaderParams>* pvecShaderParams;
	CStructuredBuffer* Buffer;
	CStructuredBuffer* ShadowBuffer;
	int BufferCount; // 인스턴싱할 Scene Component의 개수
	CInstancingCBuffer* CBuffer;
	bool Animation;

	RenderInstancingList()
	{
		Mesh = nullptr;
		Animation = false;
		pvecInstancingShader = nullptr;
		pvecShaderParams = nullptr;

		Buffer = new CStructuredBuffer;

		Buffer->Init("InstancingBuffer", sizeof(Instancing3DInfo), 100, 40, true, 
			(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);

		BufferCount = 100;

		ShadowBuffer = new CStructuredBuffer;

		ShadowBuffer->Init("InstancingShadowBuffer", sizeof(Instancing3DInfo), 100, 40, true,
			(int)Buffer_Shader_Type::Vertex | (int)Buffer_Shader_Type::Pixel);

		CBuffer = new CInstancingCBuffer;
		CBuffer->Init();
	}

	~RenderInstancingList()
	{
		SAFE_DELETE(CBuffer);
		SAFE_DELETE(ShadowBuffer);
		SAFE_DELETE(Buffer);
	}
};

struct RenderLayer
{
	std::string		Name;
	int				LayerPriority;
	std::list<class CSceneComponent*> RenderList;
	std::vector<RenderInstancingList*> m_vecInstancing;
	int				InstancingIndex;
	int				RenderCount;

	RenderLayer()
	{
		LayerPriority = 0;
		RenderCount = 0;
		InstancingIndex = 0;
		m_vecInstancing.resize(30);

		for (int i = 0; i < 30; ++i)
		{
			m_vecInstancing[i] = new RenderInstancingList;
		}
	}

	~RenderLayer()
	{
		size_t Size = m_vecInstancing.size();

		for (size_t i = 0; i < Size; ++i)
		{
			SAFE_DELETE(m_vecInstancing[i]);
		}
	}
};

class CRenderManager
{
private:
	class CRenderStateManager* m_RenderStateManager;

private:
	const std::list<CSharedPtr<class CGameObject>>* m_ObjectList;
	std::vector<RenderLayer*>	m_RenderLayerList;
	class CStandard2DConstantBuffer* m_Standard2DCBuffer;

	class CRenderState* m_DepthDisable;
	class CRenderState* m_AlphaBlend;
	class CRenderState* m_LightAccBlend;

	CSharedPtr<class CShader> m_LightBlendShader;
	CSharedPtr<class CShader> m_LightBlendRenderShader;

	// Final Target
	CSharedPtr<CRenderTarget> m_FinalTarget;
	
	// GBuffer
	std::vector<CSharedPtr<CRenderTarget>>	m_vecGBuffer;
	std::vector<CSharedPtr<CRenderTarget>>	m_vecDecal;
	std::vector<CSharedPtr<CRenderTarget>>	m_vecLightBuffer;

	// Shadow
	bool m_Shadow;
	CSharedPtr<CRenderTarget> m_ShadowMapTarget;
	CSharedPtr<class CShader> m_ShadowMapShader;
	CSharedPtr<class CShader> m_ShadowMapInstancingShader;
	float m_ShadowLightDistance;
	class CShadowCBuffer* m_ShadowCBuffer;

	// Player
	CSharedPtr<CRenderTarget> m_PlayerTarget;

	// Debug Render
	bool m_DebugRender;

	// SkyBox
	bool m_RenderSkyBox;

	// Post Processing (HDR)
	bool m_PostProcessing;

	// Animation Editor 
	CSharedPtr<class CShader> m_Mesh3DNoLightRenderShader; // m_AnimEditorRenderTarget 에 그려내기 위한 Shader 
	CSharedPtr<CRenderTarget>	m_AnimEditorRenderTarget; // Skinning 처리 이후, 해당 출력을, 별도의 RenderTarget 에 그려낸다.

	// Particle Editor
	CSharedPtr<class CShader> m_ParticleShader; // m_AnimEditorRenderTarget 에 그려내기 위한 Shader 
	CSharedPtr<CRenderTarget>	m_ParticleEffectEditorRenderTarget; // Skinning 처리 이후, 해당 출력을, 별도의 RenderTarget 에 그려낸다.

	// Toon Ramp Texture
	class CTexture* m_ToonRampTex;

	// Post Processing Renderer
	class CPostFXRenderer* m_PostFXRenderer;

	// Fade Effect
	class CFadeCBuffer* m_FadeCBuffer;
	FadeEffecType m_CurFadeEffectType;
	bool m_FadeEffectStart;
	float m_FadeEffectTimer;
	FadeEffectInfo m_FadeInfo;

	// Bomb Effect
	Vector4 m_OriginGLightColor;
	bool m_BombEffect;
	float m_BombEffectTime;
	float m_BombEffectTimer;

public :
	CRenderStateManager* GetRenderStateManager() const
	{
		return m_RenderStateManager;
	}

	void StartFadeEffect(FadeEffecType Type, bool Stay = false);

public:
	float GetShadowLightDistance() const
	{
		return m_ShadowLightDistance;
	}

	class CStandard2DConstantBuffer* GetStandard2DCBuffer()	const
	{
		return m_Standard2DCBuffer;
	}

	CRenderTarget* GetAnimationRenderTarget() const
	{
		return m_AnimEditorRenderTarget;
	}

	CRenderTarget* GetParticleEffectRenderTarget() const
	{
		return m_ParticleEffectEditorRenderTarget;
	}

	void SetDebugRender(bool DebugRender)
	{
		m_DebugRender = DebugRender;
	}

	bool IsPostProcessingEnable()
	{
		return m_PostProcessing;
	}

	void EnablePostProcessing(bool Enable)
	{
		m_PostProcessing = Enable;
	}

	void SetRenderSkyBox(bool Enable)
	{
		m_RenderSkyBox = Enable;
	}

	class CShadowCBuffer* GetShadowCBuffer() const
	{
		return m_ShadowCBuffer;
	}

	void EnableBombEffect(float Time);

	float GetMiddleGray() const;
	float GetLumWhite() const;
	float GetBloomThreshold() const;
	float GetBloomScale() const;
	float GetDOFMin() const;
	float GetDOFMax() const;
	const Vector3& GetFogColor() const;
	Fog_Type GetFogType() const;
	float GetFogStart() const;
	float GetFogEnd() const;
	float GetFogDensity() const;
	float GetShadowBias() const;
	bool GetDebugRender() const;

	void SetMiddleGray(float Gray);
	void SetLumWhite(float White);
	void SetBloomThreshold(float Threshold);
	void SetBloomScale(float Scale);
	void SetDOFMin(float Min);
	void SetDOFMax(float Max);
	void SetFogColor(const Vector3& Color);
	void SetFogType(Fog_Type Type);
	void SetFogStart(float Start);
	void SetFogEnd(float End);
	void SetFogDensity(float Density);
	void SetShadowBias(float Bias);

	void SetAdaptationTime(float Time);
	float GetAdaptationTime() const;

	bool IsDebugRender()
	{
		return m_DebugRender;
	}

	bool IsRenderSkyBox() const
	{
		return m_RenderSkyBox;
	}

	void SetFadeTime(float Time)
	{
		m_FadeInfo.Time = Time;
	}

	void SetFadeStartColor(const Vector3& Color)
	{
		m_FadeInfo.StartColor = Color;
	}

	void SetFadeEndColor(const Vector3& Color)
	{
		m_FadeInfo.EndColor = Color;
	}

	void SetFadeStay(bool Stay)
	{
		m_FadeInfo.Stay = Stay;
	}

public:
	void SetObjectList(const std::list<CSharedPtr<class CGameObject>>* List)
	{
		m_ObjectList = List;
	}

	void AddRenderList(class CSceneComponent* Component);
	void CreateLayer(const std::string& Name, int Priority);
	void SetLayerPriority(const std::string& Name, int Priority);

public:
	bool Init();
	void Render(float DeltaTime);
	void RenderPlayer(class CMesh* PlayerMesh);

private:
	void RenderSkyBox();
	void RenderShadowMap();
	void RenderGBuffer();
	void RenderDecal();
	void RenderLightAcc();
	void RenderLightBlend();
	void RenderTransparent();
	void RenderFinalScreen(float DeltaTime);
	void RenderAnimationEditor();
	void RenderParticleEffectEditor();
	void RenderPostParticle(); // 물 쉐이더 등
	void RenderParticle();

	// Render State
public:
	void SetBlendFactor(const std::string& Name, float r, float g, float b, float a);
	void AddBlendInfo(const std::string& Name, bool BlendEnable = true, D3D11_BLEND SrcBlend = D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND DestBlend = D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP BlendOp = D3D11_BLEND_OP_ADD,
		D3D11_BLEND SrcBlendAlpha = D3D11_BLEND_ONE, D3D11_BLEND DestBlendAlpha = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP BlendOpAlpha = D3D11_BLEND_OP_ADD,
		UINT8 RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);
	bool CreateBlendState(const std::string& Name, bool AlphaToCoverageEnable, bool IndependentBlendEnable);

public:
	class CRenderState* FindRenderState(const std::string& Name);

public:
	template <typename T>
	void SetFadeStartCallBack(T* Obj, void(T::* Func)())
	{
		m_FadeInfo.StartCallBack = std::bind(Func, Obj);
	}

	template <typename T>
	void SetFadeEndCallBack(T* Obj, void(T::* Func)())
	{
		m_FadeInfo.EndCallBack = std::bind(Func, Obj);
	}

private:
	void RenderDefaultInstancingShadow();
	void RenderInstancing(int LayerIndex, bool AlphaBlend);
	void UpdateInstancingList();
	void UpdateInstancingInfo(int LayerIndex, bool UpdateShadow);
	void UpdateFadeEffectInfo(float DeltaTime);

private :
	int GetRenderLayerIndex(const std::string& Name);

private:
	static bool Sortlayer(RenderLayer* Src, RenderLayer* Dest);
	static bool SortZ(CSceneComponent* Src, CSceneComponent* Dest);

	DECLARE_SINGLE(CRenderManager)
};

