
#include "EyeLaserComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/ColliderHalfLine.h"
#include "Animation/AnimationSequenceInstance.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "ObjectDataComponent.h"

CEyeLaserComponent::CEyeLaserComponent()	:
	m_TriggerHitCount(0),
	m_Player(nullptr),
	m_RayCollider(nullptr),
	m_FaceCameraOnce(false),
	m_LaserGenerator(nullptr),
	m_LaserGenTime(1.f),
	m_AccTime(0.f),
	m_LaserShootSoundPlay(false)
{
	SetTypeID<CEyeLaserComponent>();
	m_ComponentType = Component_Type::SceneComponent;

	m_CurrentLaserLeftRightDir = Vector3(0.f, 0.f, 1.f);
	//m_CurrentLaserUpDownDir = Vector3(0.f, 0.f, 1.f);
	m_NormalDir = Vector3(0.f, 1.f, 0.f);

	m_LayerName = "Transparency";
	m_Render = true;

}

CEyeLaserComponent::CEyeLaserComponent(const CEyeLaserComponent& com)	:
	CSceneComponent(com)
{
}

CEyeLaserComponent::~CEyeLaserComponent()
{
}

void CEyeLaserComponent::SetWakeEnd()
{

}

void CEyeLaserComponent::Start()
{
	CSceneComponent::Start();

	m_Player = m_Object->GetScene()->GetPlayerObject();

	// Notify(ex. Wake 애니메이션 끝나면 TrackPlayer 함수 호출) 세팅


	SetWorldScale(0.3f, 1.f, 500.f);

	m_LaserPlaneMesh = CResourceManager::GetInst()->FindMesh("PlaneMesh");

	m_Material->SetShader("LaserShader");
	m_Material->AddTexture(0, (int)Buffer_Shader_Type::Pixel,
		"LaserDif", TEXT("Laser.png"));
	m_Material->SetOpacity(1.f);
	m_Material->SetTransparency(true);

	m_RayCollider = m_Object->FindComponentFromType<CColliderHalfLine>();
	if (m_RayCollider)
	{
		m_RayCollider->AddCollisionCallback<CEyeLaserComponent>(Collision_State::Begin, this, &CEyeLaserComponent::OnRaserCollisionBegin);
		m_RayCollider->AddCollisionCallback<CEyeLaserComponent>(Collision_State::Stay, this, &CEyeLaserComponent::OnRaserCollisionStay);
		m_RayCollider->AddCollisionCallback<CEyeLaserComponent>(Collision_State::End, this, &CEyeLaserComponent::OnRaserCollisionEnd);
		m_RayCollider->Enable(false);
	}

	m_LaserGenerator = m_Scene->FindObject("LaserGenerator");
}

bool CEyeLaserComponent::Init()
{
	if (!CSceneComponent::Init())
		return false;

	return true;
}

void CEyeLaserComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
	
	if (m_TriggerHitCount == 0)
		m_RayCollider->Enable(false);

	if (m_TriggerHitCount >= 0 && m_TriggerHitCount < 4)
	{
		FaceCamera();
	}

	if (m_TriggerHitCount > 0 && m_TriggerHitCount < 4)
	{
		TrackPlayer(DeltaTime);

		if (!m_LaserShootSoundPlay)
		{
			m_Object->GetScene()->GetResource()->SoundPlay("LaserLoop");
			m_LaserShootSoundPlay = true;
		}
	}

	else if (m_TriggerHitCount >= 4)
	{
		m_Object->GetScene()->GetResource()->SoundStop("LaserLoop");
		m_RayCollider->Enable(false);
	}
}

void CEyeLaserComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CEyeLaserComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CEyeLaserComponent::Render()
{

	if (m_TriggerHitCount > 0 && m_TriggerHitCount < 4)
	{
		CSceneComponent::Render();

		m_Material->Render();

		m_LaserPlaneMesh->Render();

		m_Material->Reset();
	}
}

void CEyeLaserComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CEyeLaserComponent* CEyeLaserComponent::Clone()
{
	return new CEyeLaserComponent(*this);
}

bool CEyeLaserComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);
	
	m_Material->Save(File);
	m_LaserPlaneMesh->Save(File);


	return true;
}

bool CEyeLaserComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);

	m_Material = new CMaterial;

	m_Material->Load(File);

	m_LaserPlaneMesh = CResourceManager::GetInst()->FindMesh("PlaneMesh");

	m_LaserPlaneMesh->Load(File);

	return true;
}

bool CEyeLaserComponent::SaveOnly(FILE* File)
{
	return false;
}

bool CEyeLaserComponent::LoadOnly(FILE* File)
{
	return false;
}

void CEyeLaserComponent::TrackPlayer(float DeltaTime)
{
	if (!m_RayCollider->IsEnable())
	{
		m_RayCollider->Enable(true);
	}
	// 원래 레이저가 바라보는 방향은 플레이어가 입장하는 문을 바라보는 방향. 
	// 이때 플레이어가 레이저를 바라볼 때 오른쪽이 -x, 왼쪽이 +x이다
	//m_CurrentLaserRot = Vector3(0.f, 0.f, 1.f);
	m_CurrentLaserLeftRightDir = m_CurrentLaserLeftRightDir.TransformCoord(m_LaserLeftRightRotMatrix);

	Vector3 PlayerPos = m_Player->GetWorldPos();
	//Vector3 LaserPos = m_Object->GetWorldPos();
	Vector3 LaserPos = GetWorldPos();

	Vector3 Dir = PlayerPos - LaserPos;
	Dir.y = 0.f;
	Dir.Normalize();

	float Rad = Dir.Dot(m_CurrentLaserLeftRightDir);
	float Degree = RadianToDegree(acosf(Rad));

	Vector3 CrossVector = Dir.Cross(m_CurrentLaserLeftRightDir);
	float Angle = DeltaTime * 10.f;

	// Player가 레이저를 바라봤을때, Player의 위치가 레이저 쏘는 위치보다 오른쪽에 존재
	// 로컬축 기준으로 회전하니까 Z방향으로 회전해야함
	if (CrossVector.y > 0.f)
	{
		// 회전은 로컬축 기준이라 Z축 회전을 하지만 World기준은 Y방향이므로 Matrix는 Y축 회전 Matrix를 만든다
		//m_AnimComp->AddWorldRotationZ(-DeltaTime * 7.f);
		AddWorldRotationY(-Angle);
		if (m_RayCollider)
			m_RayCollider->AddWorldRotationY(-Angle);
		m_LaserLeftRightRotMatrix.RotationY(-Angle);
	}

	// Player가 레이저를 바라봤을때, Player의 위치가 레이저 쏘는 위치보다 왼쪽에 존재
	else if(CrossVector.y < 0.f)
	{
		// 회전은 로컬축 기준이라 Z축 회전을 하지만 World기준은 Y방향이므로 Matrix는 Y축 회전 Matrix를 만든다
		//m_AnimComp->AddWorldRotationZ(DeltaTime * 7.f);
		AddWorldRotationY(Angle);
		if(m_RayCollider)
			m_RayCollider->AddWorldRotationY(Angle);
		m_LaserLeftRightRotMatrix.RotationY(Angle);
	}

	Vector3 AxisZ = m_LaserGenerator->GetWorldAxis(AXIS_Z);
	AxisZ = Vector3(-1.f * AxisZ.x, -1.f * AxisZ.y, -1.f * AxisZ.z);

	float DotProduct = m_CurrentLaserLeftRightDir.Dot(AxisZ);
	float LaserDegree = 0.f;

	if (DotProduct >= -0.99999999f && DotProduct <= 0.99999999f)
	{
		LaserDegree = RadianToDegree(acosf(DotProduct));

		Vector3 CrossVec = m_CurrentLaserLeftRightDir.Cross(AxisZ);

		if (CrossVec.y > 0.f)
			Angle *= -1.f;

		if(abs(LaserDegree) > 1.f)
			m_LaserGenerator->AddWorldRotationY(Angle);
	}
}

void CEyeLaserComponent::ChangeToWakeAnimation()
{
}

void CEyeLaserComponent::ChangeToDieAnimation()
{
}

void CEyeLaserComponent::ChangeToIdleAnimation()
{
}

void CEyeLaserComponent::FaceCamera()
{
	// 카메라를 계속 바라보게 만든다.
	Vector3 CameraPos = m_Scene->GetCameraManager()->GetCurrentCamera()->GetWorldPos();
	Vector3 MyPos = GetWorldPos();

	Vector3 NormalDir = Vector3(0.f, 1.f, 0.f);

	//m_NormalDir.z = 0.f;
	//CameraPos.x = MyPos.x;
	Vector3	View = CameraPos - MyPos;
	View.z = 0.f;
	View.Normalize();

	float Angle = 0.f;
	float DotProduct = View.Dot(NormalDir);

	if (DotProduct >= -0.9999999f && DotProduct <= 0.9999999f)
	{
		Vector3 ViewXZPlaneVec = Vector3(View.x, 0.f, View.z);
		Vector3 LaserRot = GetWorldRot();
		Vector3 LaserXZPlaneVec = Vector3(LaserRot.x, 0.f, LaserRot.z);

		float DotProduct2 = ViewXZPlaneVec.Dot(LaserXZPlaneVec);
		if (DotProduct2 >= -0.9999999f && DotProduct2 <= 0.9999999f)
		{
			float Angle2 = RadianToDegree(acosf(DotProduct2));

			if (Angle2 != 0.f)
			{
				Angle = RadianToDegree(acosf(DotProduct));

				Vector3 RotDir = View.Cross(NormalDir);
				RotDir.Normalize();

				if (RotDir.z > 0.f)
					Angle *= -1.f;

				SetWorldRotationZ(Angle);
			}
		}
	}
}

void CEyeLaserComponent::OnRaserCollisionBegin(const CollisionResult& Result)
{
	CGameObject* Object = Result.Dest->GetGameObject();

	CObjectDataComponent* Comp = (CObjectDataComponent*)(Object->FindObjectComponent("ObjectData"));

	if (Comp)
	{
		Comp->DecreaseHP(1);
	}

	Vector3 RayScale = m_RayCollider->GetWorldScale();
	SetWorldScale(0.3f, 1.f, RayScale.x);

}

void CEyeLaserComponent::OnRaserCollisionStay(const CollisionResult& Result)
{
	Vector3 RayScale = m_RayCollider->GetWorldScale();
	SetWorldScale(0.3f, 1.f, RayScale.x);
}

void CEyeLaserComponent::OnRaserCollisionEnd(const CollisionResult& Result)
{
	int a = 3;

	//if (!Result.Src || !Result.Dest)
	//	return;

	m_RayCollider->SetWorldScale(500.f, 1.f, 1.f);
	SetWorldScale(0.3f, 1.f, 500.f);

}

void CEyeLaserComponent::SetBaseColor(const Vector4& Color, int Index)
{
	m_Material->SetBaseColor(Color);
}

void CEyeLaserComponent::SetBaseColor(float r, float g, float b, float a, int Index)
{
	m_Material->SetBaseColor(r, g, b, a);
}

void CEyeLaserComponent::SetEmissiveColor(const Vector4& Color, int Index)
{
	m_Material->SetEmissiveColor(Color);
}

void CEyeLaserComponent::SetEmissiveColor(float r, float g, float b, float a, int Index)
{
	m_Material->SetEmissiveColor(r, g, b, a);
}

void CEyeLaserComponent::SetTransparencyMaterial(bool Enable)
{
	if (Enable)
	{
		bool AlreadyTransparent = m_LayerName == "Transparency";

		if (!AlreadyTransparent)
		{
			m_LayerName = "Transparency";
		}

		m_Material->SetTransparency(true);

		// 모든 Material의 Shader 교체
		SetMaterialShader("Transparent3DShader");

		// 인스턴싱 레이어를 바꾼다.
		if (!AlreadyTransparent)
		{
			ChangeInstancingLayer();
		}
	}

	else
	{
		bool AlreadyOpaque = m_LayerName == "Default";

		if (!AlreadyOpaque)
		{
			m_LayerName = "Default";
		}

		// 이전 쉐이더로 되돌림
		m_Material->SetTransparency(false);
		m_Material->RevertShader();

		// 인스턴싱 레이어를 바꾼다.
		if (!AlreadyOpaque)
		{
			ChangeInstancingLayer();
		}
	}
}

void CEyeLaserComponent::SetMaterialShader(const std::string& Name)
{
	CGraphicShader* Shader = dynamic_cast<CGraphicShader*>(CResourceManager::GetInst()->FindShader(Name));

	if (!Shader)
		return;

	m_Material->SetShader(Shader);
	OnChangeMaterialShader(Shader);

}

void CEyeLaserComponent::ChangeInstancingLayer()
{
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	bool CheckCountExist = false;

	bool AddOnNewLayer = false;
	bool DeleteOnOldLayer = false;

	for (; iter != iterEnd;)
	{
		if ((*iter)->Mesh == m_LaserPlaneMesh)
		{
			// 같은 레이어일 경우 추가한다.
			if ((*iter)->LayerName == m_LayerName)
			{
				CheckCountExist = true;

				(*iter)->InstancingList.push_back(this);

				auto iterInst = (*iter)->InstancingList.begin();
				auto iterInstEnd = (*iter)->InstancingList.end();

				// 이 컴포넌트를 추가하면서 인스턴싱으로 출력하게 되는 경우
				if ((*iter)->InstancingList.size() == 10)
				{
					for (; iterInst != iterInstEnd; ++iterInst)
					{
						(*iterInst)->SetInstancing(true);
					}
				}
				// 이미 인스턴싱 출력중인 경우
				else if ((*iter)->InstancingList.size() > 10)
				{
					SetInstancing(true);
				}
				// 인스턴싱 하지 않는 경우
				else
				{
					SetInstancing(false);
				}

				AddOnNewLayer = true;
			}

			// Layer가 다를 경우 해당 레이어에서 뺀다
			else
			{
				// 이 컴포넌트가 빠지면서 인스턴싱을 하지 않게 처리해야 하는 경우
				bool InstancingOff = (*iter)->InstancingList.size() == 10;

				auto iterInst = (*iter)->InstancingList.begin();
				auto iterInstEnd = (*iter)->InstancingList.end();

				if (InstancingOff)
				{
					for (; iterInst != iterInstEnd;)
					{
						(*iterInst)->SetInstancing(false);

						if ((*iterInst) == this)
						{
							iterInst = (*iter)->InstancingList.erase(iterInst);
							continue;
						}

						++iterInst;
					}
				}
				else
				{
					for (; iterInst != iterInstEnd; ++iterInst)
					{
						if ((*iterInst) == this)
						{
							// 현재 레이어의 인스턴싱 리스트에서 제거
							(*iter)->InstancingList.erase(iterInst);
							break;
						}
					}
				}

				DeleteOnOldLayer = true;

				if ((*iter)->InstancingList.empty())
				{
					SAFE_DELETE(*iter);
					iter = m_InstancingCheckList.erase(iter);
					continue;
				}
			}
		}

		// 새 레이어에 넣고, 이전 레이어에서 빼는 작업 완료한 경우 루프 종료
		if (AddOnNewLayer && DeleteOnOldLayer)
		{
			break;
		}

		++iter;
	}

	// 이 컴포넌트가 속한 레이어에 처음 추가되는 경우
	if (!CheckCountExist)
	{
		OnCreateNewInstancingCheckCount();
	}
}

void CEyeLaserComponent::OnCreateNewInstancingCheckCount()
{
	InstancingCheckCount* CheckCount = new InstancingCheckCount;

	m_InstancingCheckList.push_back(CheckCount);

	CheckCount->InstancingList.push_back(this);
	CheckCount->Mesh = m_LaserPlaneMesh;
	CheckCount->LayerName = m_LayerName;

	// Material별로 어떤 Instanicng Shader로 렌더해야 하는지 받아온다.
	size_t SlotSize = 1;

	CGraphicShader* NoInstancingShader = nullptr;
	CGraphicShader* InstancingShader = nullptr;
	CMaterial* Mat = nullptr;
	ShaderParams MatShaderParams = {};

	CheckCount->vecInstancingShader.resize(SlotSize);
	CheckCount->vecShaderParams.resize(SlotSize);

	for (size_t i = 0; i < SlotSize; ++i)
	{
		// Material별 Instancing Shader, Shader Paramerter 정보 저장
		Mat = m_Material;
		NoInstancingShader = Mat->GetShader();
		InstancingShader = CResourceManager::GetInst()->FindInstancingShader(NoInstancingShader);
		MatShaderParams = Mat->GetShaderParams();

		CheckCount->vecInstancingShader[i] = InstancingShader;
		CheckCount->vecShaderParams[i] = MatShaderParams;
	}

	SetInstancing(false);
}

void CEyeLaserComponent::SetOpacity(float Opacity)
{
	m_Material->SetOpacity(Opacity);
}

void CEyeLaserComponent::AddOpacity(float Opacity)
{
	m_Material->AddOpacity(Opacity);
}

void CEyeLaserComponent::OnChangeMaterialShader(CGraphicShader* NewShader)
{
	auto iter = m_InstancingCheckList.begin();
	auto iterEnd = m_InstancingCheckList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->Mesh == m_LaserPlaneMesh)
		{
			if ((*iter)->LayerName == m_LayerName)
			{
				// Instancing Shader 교체
				CGraphicShader* NewInstancingShader = CResourceManager::GetInst()->FindInstancingShader(NewShader);
				(*iter)->vecInstancingShader[0] = NewInstancingShader;
				break;
			}
		}
	}
}
