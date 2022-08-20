
#include "LightComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/LightManager.h"
#include "../Scene/CameraManager.h"
#include "CameraComponent.h"

CLightComponent::CLightComponent()	:
	m_Add(false)
{
	SetTypeID<CLightComponent>();
	m_Render = false;

	m_CBuffer = nullptr;
}

CLightComponent::CLightComponent(const CLightComponent& com) :
	CSceneComponent(com)
{
	m_CBuffer = com.m_CBuffer->Clone();
	m_Add = false;
}

CLightComponent::~CLightComponent()
{
	if (m_Object->IsInPool() == false && m_Scene)
	{
		CLightManager* LightManager = m_Scene->GetLightManager();

		if (LightManager)
		{
			LightManager->DeleteLight(this);
		}
	}

	SAFE_DELETE(m_CBuffer);
}

void CLightComponent::Start()
{
	CSceneComponent::Start();

	if (!m_Add)
	{
		m_Scene->GetLightManager()->AddLight(this);
	}

	m_Add = true;
}

bool CLightComponent::Init()
{
	m_CBuffer = new CLightConstantBuffer;
	
	m_CBuffer->Init();

	return true;
}

void CLightComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
}

void CLightComponent::Reset()
{
	CSceneComponent::Reset();

	m_Add = false;

	CLightManager* LightManager = m_Scene->GetLightManager();

	if (LightManager)
	{
		LightManager->DeleteLight(this);
	}
}

void CLightComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	if (m_CBuffer->GetLightType() != Light_Type::Dir)
	{
		Vector3	Pos = GetWorldPos();

		// 뷰 공간으로 변환한다.
		Pos = Pos.TransformCoord(Camera->GetViewMatrix());

		m_CBuffer->SetPos(Pos);

		if (m_CBuffer->GetLightType() == Light_Type::Point)
			SetRelativeScale(m_CBuffer->GetLightDistance(), m_CBuffer->GetLightDistance(), m_CBuffer->GetLightDistance());
	}

	if(m_CBuffer->GetLightType() != Light_Type::Point)
	{
		Vector3 Dir = GetWorldAxis(AXIS_Z);

		// 뷰공간으로 변환한다.
		Dir = Dir.TransformNormal(Camera->GetViewMatrix());
		Dir.Normalize();

		m_CBuffer->SetDir(Dir);
	}
}

void CLightComponent::CheckCollision()
{
	Light_Type Type = m_CBuffer->GetLightType();
	switch (Type)
	{
	case Light_Type::Dir:
		m_Culling = false;
		break;
	case Light_Type::Point:
	{
		m_SphereInfo.Center = GetWorldPos();
		m_SphereInfo.Radius = m_CBuffer->GetLightDistance();
		m_SphereInfo.Radius /= 2.f;

		CCameraComponent* Cam = m_Scene->GetCameraManager()->GetCurrentCamera();

		m_Culling = Cam->FrustumInSphere(m_SphereInfo);
		break;
	}
	case Light_Type::Spot:
		m_Culling = false;
		break;
	}

	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->CheckCollision();
	}
}

void CLightComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CLightComponent::Render()
{
	CSceneComponent::Render();
}

void CLightComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CLightComponent* CLightComponent::Clone()
{
	return new CLightComponent(*this);
}

bool CLightComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);

	Vector4 Color = m_CBuffer->GetLightColor();
	Light_Type Type = (Light_Type)m_CBuffer->GetLightType();
	float Distance = m_CBuffer->GetLightDistance();
	float AngleIn = m_CBuffer->GetLightAngleIn();
	float AngleOut = m_CBuffer->GetLightAngleOut();
	float Att1 = m_CBuffer->GetLightAtt1();
	float Att2 = m_CBuffer->GetLightAtt2();
	float Att3 = m_CBuffer->GetLightAtt3();

	fwrite(&Color, sizeof(Vector4), 1, File);
	fwrite(&Type, sizeof(Light_Type), 1, File);
	fwrite(&Distance, sizeof(float), 1, File);
	fwrite(&AngleIn, sizeof(float), 1, File);
	fwrite(&AngleOut, sizeof(float), 1, File);
	fwrite(&Att1, sizeof(float), 1, File);
	fwrite(&Att2, sizeof(float), 1, File);
	fwrite(&Att3, sizeof(float), 1, File);

	return true;
}

bool CLightComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);

	Vector4 Color;
	Light_Type Type;
	float Distance;
	float AngleIn;
	float AngleOut;
	float Att1;
	float Att2;
	float Att3;

	fread(&Color, sizeof(Vector4), 1, File);
	fread(&Type, sizeof(Light_Type), 1, File);
	fread(&Distance, sizeof(float), 1, File);
	fread(&AngleIn, sizeof(float), 1, File);
	fread(&AngleOut, sizeof(float), 1, File);
	fread(&Att1, sizeof(float), 1, File);
	fread(&Att2, sizeof(float), 1, File);
	fread(&Att3, sizeof(float), 1, File);

	m_CBuffer = new CLightConstantBuffer;
	m_CBuffer->Init();

	m_CBuffer->SetColor(Color);
	m_CBuffer->SetLightType(Type);
	m_CBuffer->SetDistance(Distance);
	m_CBuffer->SetAngleIn(AngleIn);
	m_CBuffer->SetAngleOut(AngleOut);
	m_CBuffer->SetAtt1(Att1);
	m_CBuffer->SetAtt2(Att2);
	m_CBuffer->SetAtt3(Att3);

	return true;
}

bool CLightComponent::SaveOnly(FILE* File)
{
	CSceneComponent::SaveOnly(File);

	Vector4 Color = m_CBuffer->GetLightColor();
	Light_Type Type = (Light_Type)m_CBuffer->GetLightType();
	float Distance = m_CBuffer->GetLightDistance();
	float AngleIn = m_CBuffer->GetLightAngleIn();
	float AngleOut = m_CBuffer->GetLightAngleOut();
	float Att1 = m_CBuffer->GetLightAtt1();
	float Att2 = m_CBuffer->GetLightAtt2();
	float Att3 = m_CBuffer->GetLightAtt3();

	fwrite(&Color, sizeof(Vector4), 1, File);
	fwrite(&Type, sizeof(Light_Type), 1, File);
	fwrite(&Distance, sizeof(float), 1, File);
	fwrite(&AngleIn, sizeof(float), 1, File);
	fwrite(&AngleOut, sizeof(float), 1, File);
	fwrite(&Att1, sizeof(float), 1, File);
	fwrite(&Att2, sizeof(float), 1, File);
	fwrite(&Att3, sizeof(float), 1, File);

	return true;
}

bool CLightComponent::LoadOnly(FILE* File)
{
	CSceneComponent::LoadOnly(File);

	Vector4 Color = m_CBuffer->GetLightColor();
	Light_Type Type = (Light_Type)m_CBuffer->GetLightType();
	float Distance = m_CBuffer->GetLightDistance();
	float AngleIn = m_CBuffer->GetLightAngleIn();
	float AngleOut = m_CBuffer->GetLightAngleOut();
	float Att1 = m_CBuffer->GetLightAtt1();
	float Att2 = m_CBuffer->GetLightAtt2();
	float Att3 = m_CBuffer->GetLightAtt3();

	fread(&Color, sizeof(Vector4), 1, File);
	fread(&Type, sizeof(Light_Type), 1, File);
	fread(&Distance, sizeof(float), 1, File);
	fread(&AngleIn, sizeof(float), 1, File);
	fread(&AngleOut, sizeof(float), 1, File);
	fread(&Att1, sizeof(float), 1, File);
	fread(&Att2, sizeof(float), 1, File);
	fread(&Att3, sizeof(float), 1, File);

	m_CBuffer = new CLightConstantBuffer;
	m_CBuffer->Init();

	m_CBuffer->SetColor(Color);
	m_CBuffer->SetLightType(Type);
	m_CBuffer->SetDistance(Distance);
	m_CBuffer->SetAngleIn(AngleIn);
	m_CBuffer->SetAngleOut(AngleOut);
	m_CBuffer->SetAtt1(Att1);
	m_CBuffer->SetAtt1(Att2);
	m_CBuffer->SetAtt1(Att3);

	return true;
}

void CLightComponent::Enable(bool Enable)
{
	CSceneComponent::Enable(Enable);

	CLightManager* LightManager = m_Scene->GetLightManager();

	if (Enable)
	{
		if (!m_Add)
		{
			LightManager->AddLight(this);
		}
		m_Add = true;
	}
	else
	{
		LightManager->DeleteLight(this);
		m_Add = false;
	}
}

void CLightComponent::SetShader()
{
	m_CBuffer->UpdateCBuffer();
}
