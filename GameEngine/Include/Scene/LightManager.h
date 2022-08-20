#pragma once

#include "../GameInfo.h"
#include "../Component/LightComponent.h"

class CLightManager
{
	friend class CScene;

private:
	CLightManager();
	~CLightManager();

private:
	class CScene* m_Scene;
	CSharedPtr<class CGameObject>	m_GlobalLight;
	CSharedPtr<CLightComponent>	m_GlobalLightComponent;

	// Ambient 강도 등 전역 조명 값 상수버퍼
	class CGlobalLightCBuffer* m_GlobalLightCBuffer;

private:
	std::list<CSharedPtr<CLightComponent>>	m_LightList;

	// 투명 오브젝트들을 위한 Forward Rendering용 멤버
private:
	class CLightForwardConstantBuffer* m_CBuffer;
	class CStructuredBuffer* m_LightListBuffer;
	std::vector<LightCBuffer> m_LightListBufferData;

public:
	class CGameObject* GetGlobalLight() const
	{
		return m_GlobalLight;
	}

	class CLightComponent* GetGlobalLightComponent() const
	{
		return m_GlobalLightComponent;
	}

	void SetGlogbalLightAmbientIntensity(float Intensity);
	float GetGlobalLightAmbiendIntensity() const;

public:
	void AddLight(CLightComponent* Light);
	void DeleteLight(CLightComponent* Light);
	void DeleteLight(const std::string& Name);

public:
	void Start();
	void Init();
	void Update(float DeltaTime);
	void PostUpdate(float DeltaTime);
	void SetShader();
	void Destroy();
	void Render();
	void SetForwardRenderShader();
	void ResetForwardRenderShader();

private:
	void CreateLightListBuffer();
};

