
#include "LightManager.h"
#include "Scene.h"
#include "../GameObject/GameObject.h"
#include "SceneResource.h"
#include "../Resource/Shader/Shader.h"
#include "../Device.h"
#include "../Component/Transform.h"
#include "../Resource/Shader/StructuredBuffer.h"
#include "../Resource/Shader/LightForwardConstantBuffer.h"
#include "../Resource/Shader/GlobalLightCBuffer.h"

CLightManager::CLightManager()	:
	m_CBuffer(nullptr),
	m_LightListBuffer(nullptr)
{
}

CLightManager::~CLightManager()
{
	SAFE_DELETE(m_GlobalLightCBuffer);
	SAFE_DELETE(m_CBuffer);
	SAFE_DELETE(m_LightListBuffer);
}

void CLightManager::SetGlogbalLightAmbientIntensity(float Intensity)
{
	m_GlobalLightCBuffer->SetAmbientIntensity(Intensity);
}

float CLightManager::GetGlobalLightAmbiendIntensity() const
{
	return m_GlobalLightCBuffer->GetAmbiendIntensity();
}

void CLightManager::AddLight(CLightComponent* Light)
{
	m_LightList.push_back(Light);

	CreateLightListBuffer();
}

void CLightManager::DeleteLight(CLightComponent* Light)
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == Light)
		{
			m_LightList.erase(iter);
			break;
		}
	}

	if (!m_LightList.empty())
	{
		CreateLightListBuffer();
	}

}

void CLightManager::DeleteLight(const std::string& Name)
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetName() == Name)
		{
			m_LightList.erase(iter);
			break;
		}
	}

	if (!m_LightList.empty())
	{
		CreateLightListBuffer();
	}
}

void CLightManager::Start()
{
}

void CLightManager::Init()
{
	m_GlobalLight = m_Scene->FindObject("GlobalLight");
	m_GlobalLight = m_Scene->CreateGameObject<CGameObject>("GlobalLight");
	m_GlobalLight->ExcludeFromSceneSave();
	// m_GlobalLight->SetNoDestroyOnSceneChange(true);

	m_GlobalLightComponent = m_GlobalLight->CreateComponent<CLightComponent>("Light");
	m_GlobalLightComponent->SetLightType(Light_Type::Dir);

	m_GlobalLightComponent->SetRelativeRotation(45.f, 90.f, 0.f);

	m_CBuffer = new CLightForwardConstantBuffer;
	m_CBuffer->Init();

	m_GlobalLightCBuffer = new CGlobalLightCBuffer;
	m_GlobalLightCBuffer->Init();
}

void CLightManager::Update(float DeltaTime)
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsActive())
		{
			iter = m_LightList.erase(iter);
			iterEnd = m_LightList.end();

			continue;
		}

		++iter;
	}
}

void CLightManager::PostUpdate(float DeltaTime)
{
}

void CLightManager::SetShader()
{
	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable())
			continue;

		(*iter)->SetShader();
	}
}

void CLightManager::Destroy()
{
	std::list<CSharedPtr<CLightComponent>>	List = m_LightList;
	m_LightList.clear();
	List.clear();
	m_GlobalLightComponent = nullptr;
	m_GlobalLight = nullptr;
}

void CLightManager::Render()
{
	CShader* Shader = m_Scene->GetResource()->FindShader("LightAccShader");

	Shader->SetShader();

	// 전역 조명 상수버퍼 바인드
	m_GlobalLightCBuffer->UpdateCBuffer();

	auto	iter = m_LightList.begin();
	auto	iterEnd = m_LightList.end();

	bool	SendTransform = false;

	int CullCount = 0;

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable() || (*iter)->GetCulling())
		{
			++CullCount;
			continue;
		}

		if (!SendTransform)
		{
			(*iter)->GetTransform()->SetTransform();
			SendTransform = true;
		}

		(*iter)->SetShader();

		UINT Offset = 0;
		CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
		CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		CDevice::GetInst()->GetContext()->Draw(4, 0);
	}
}

void CLightManager::SetForwardRenderShader()
{
	size_t Size = m_LightList.size();

	m_CBuffer->SetLightCount((int)Size);
	m_CBuffer->UpdateCBuffer();

	// 전역 조명 상수버퍼 바인드
	m_GlobalLightCBuffer->UpdateCBuffer();

	m_LightListBufferData.clear();
	m_LightListBufferData.resize(Size);

	int Count = 0;

	auto iter = m_LightList.begin();
	auto iterEnd = m_LightList.end();

	for (; iter != iterEnd; ++iter)
	{
		m_LightListBufferData[Count].Pos = (*iter)->GetLightPos();
		m_LightListBufferData[Count].Color = (*iter)->GetLightColor();
		m_LightListBufferData[Count].LightType = (int)(*iter)->GetLightType();
		m_LightListBufferData[Count].Dir = (*iter)->GetLightDir();
		m_LightListBufferData[Count].Distance = (*iter)->GetLightDistance();
		m_LightListBufferData[Count].AngleIn = (*iter)->GetLightAngleIn();
		m_LightListBufferData[Count].AngleOut = (*iter)->GetLightAngleOut();
		m_LightListBufferData[Count].Att1 = (*iter)->GetLightAtt1();
		m_LightListBufferData[Count].Att2 = (*iter)->GetLightAtt2();
		m_LightListBufferData[Count].Att3 = (*iter)->GetLightAtt3();

		++Count;
	}

	m_LightListBuffer->UpdateBuffer(&m_LightListBufferData[0], (int)m_LightList.size());
	m_LightListBuffer->SetShader();
}

void CLightManager::ResetForwardRenderShader()
{
	m_LightListBuffer->ResetShader();
}

void CLightManager::CreateLightListBuffer()
{
	SAFE_DELETE(m_LightListBuffer);

	m_LightListBuffer = new CStructuredBuffer;
	m_LightListBuffer->Init("LightListBuffer", sizeof(LightCBuffer), (unsigned int)m_LightList.size(),
		13, true, (int)Buffer_Shader_Type::Pixel);
}
