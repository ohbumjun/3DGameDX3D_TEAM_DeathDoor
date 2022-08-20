
#include "ParticleComponent.h"
#include "../Device.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Scene/SceneManager.h"
#include "../Scene/CameraManager.h"
#include "../PathManager.h"
#include "../Component/CameraComponent.h"
#include "../Resource/Shader/StructuredBuffer.h"
#include "../EngineUtil.h"

CParticleComponent::CParticleComponent()	:
	m_SpawnTime(0.f),
	m_SpawnTimeMax(0.01f),
	m_Info{},
	m_BillBoardEffect(false),
	// m_UpdateInitBillBoardDir(false),
	m_UpdateInitBillBoardDir(true),
	// m_BazierMoveEffect(false),
	// m_ParticleMoveSpeed(20.f),
	m_TempCreateAccTimeMax(5.f),
	m_InfoShared{},
	m_CBuffer(nullptr)
{
	SetTypeID<CParticleComponent>();
	m_Render = true;

	m_LayerName = "Particle";

	// ó������ Particle Component �� Dir �� (0, 0, -1) �� ����
	// �ֳ��ϸ�, Particle Component ��ü��, z �� ���� -1 �������� ���ϸ鼭
	// ����� ���� �ٶ󺸰� �ֱ� �����̴�.
	// m_ParticleMoveDir = Vector3(0, 0, -1);
}

CParticleComponent::CParticleComponent(const CParticleComponent& com) :
	CSceneComponent(com)
{
	m_SpawnTime = 0.f;
	m_SpawnTimeMax = com.m_SpawnTimeMax;

	m_Particle = com.m_Particle;

	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		SAFE_DELETE(m_vecStructuredBuffer[i]);
	}

	m_vecStructuredBuffer.clear();

	if (m_Particle)
	{
		m_Particle->CloneStructuredBuffer(m_vecStructuredBuffer);

		m_UpdateShader = m_Particle->GetUpdateShader();

		SAFE_DELETE(m_CBuffer);

		m_CBuffer = m_Particle->CloneConstantBuffer();
	}
}

CParticleComponent::~CParticleComponent()
{
	// SAFE_DELETE(m_NormalDistributionBuffer);

	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		SAFE_DELETE(m_vecStructuredBuffer[i]);
	}

	SAFE_DELETE(m_CBuffer);
	SAFE_DELETE(m_TempVCBuffer);
}

void CParticleComponent::StartParticle(const Vector3& StartPos)
{
	CRef::Enable(true);

	// �ش� �ڵ��� ��ġ�� ���ؼ� ������ �߻��� �� �ִ�.
	DestroyExistingParticles();

	SetWorldPos(StartPos);

	RecreateOnlyOnceCreatedParticle();

	m_UpdateInitBillBoardDir = true;
	m_Render = true;
}

void CParticleComponent::SetParticle(const std::string& Name)
{
	SetParticle(m_Scene->GetResource()->FindParticle(Name));
}

void CParticleComponent::SetParticle(CParticle* Particle)
{
	m_Particle = Particle;

	m_Material = m_Particle->CloneMaterial();
	
	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		SAFE_DELETE(m_vecStructuredBuffer[i]);
	}

	// SAFE_DELETE(m_NormalDistributionBuffer);

	m_vecStructuredBuffer.clear();

	m_Particle->CloneStructuredBuffer(m_vecStructuredBuffer);

	// ���� ���� ������ ���, �� �ѹ��� Update ���ش�. (�켱 ��� X)
	// m_Particle->CloneNormalDistStructuredBuffer(m_NormalDistributionBuffer);
	// std::vector<float> VecNormalDistVal = m_Particle->GetVecNormalDistVal();
	// m_NormalDistributionBuffer->UpdateBuffer(&VecNormalDistVal[0], (int)VecNormalDistVal.size());

	// m_UpdateShader = m_Particle->GetUpdateShader();
	m_UpdateShader = m_Particle->CloneUpdateShader();

	SAFE_DELETE(m_CBuffer);

	m_CBuffer = m_Particle->CloneConstantBuffer();

	m_SpawnTimeMax = m_Particle->GetSpawnTimeMax();

	m_ParticleName = m_Particle->GetName();

	SAFE_DELETE(m_TempVCBuffer);

	m_TempVCBuffer = new CParticleTempValConstantBuffer;

	if (!m_TempVCBuffer->Init())
		assert(false);
}

void CParticleComponent::SetParticleWithOutCloneShader(CParticle* Particle)
{
	m_Particle = Particle;

	m_Material = m_Particle->CloneMaterial();

	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		SAFE_DELETE(m_vecStructuredBuffer[i]);
	}

	// SAFE_DELETE(m_NormalDistributionBuffer);

	m_vecStructuredBuffer.clear();

	m_Particle->CloneStructuredBuffer(m_vecStructuredBuffer);

	// ���� ���� ������ ���, �� �ѹ��� Update ���ش�. (�켱 ��� X)
	// m_Particle->CloneNormalDistStructuredBuffer(m_NormalDistributionBuffer);
	// std::vector<float> VecNormalDistVal = m_Particle->GetVecNormalDistVal();
	// m_NormalDistributionBuffer->UpdateBuffer(&VecNormalDistVal[0], (int)VecNormalDistVal.size());

	m_UpdateShader = m_Particle->GetUpdateShader();

	SAFE_DELETE(m_CBuffer);

	m_CBuffer = m_Particle->CloneConstantBuffer();

	m_SpawnTimeMax = m_Particle->GetSpawnTimeMax();

	m_ParticleName = m_Particle->GetName();

	SAFE_DELETE(m_TempVCBuffer);

	m_TempVCBuffer = new CParticleTempValConstantBuffer;
	
	if (!m_TempVCBuffer->Init())
		assert(false);
}

void CParticleComponent::SetSpawnTime(float Time)
{
	m_SpawnTimeMax = Time;

	m_Particle->SetSpawnTimeMax(m_SpawnTimeMax);
}

void CParticleComponent::ExecuteComputeShader()
{
	int	GroupCount = m_Particle->GetSpawnCountMax() / 64 + 1;

	m_UpdateShader->Excute(GroupCount, 1, 1);
}

void CParticleComponent::ApplyBillBoardEffect()
{
	// Vector3 CameraPos = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera()->GetWorldPos();
	// 
	// Vector3 View = CameraPos - GetWorldPos();
	// 
	// View.Normalize();
	// 
	// // float 
	// Vector3 OriginDir = Vector3(0.f, 0.f, -1.f);
	// 
	// m_Transform->SetRotationAxis(OriginDir, View);
}
void CParticleComponent::Start()
{
	CSceneComponent::Start();

}

bool CParticleComponent::Init()
{
	m_Mesh = m_Scene->GetResource()->FindMesh("ParticlePointMesh");

	return true;
}

void CParticleComponent::Reset()
{
	CSceneComponent::Reset();

	DestroyExistingParticles();
		
	m_InitBillBoardZLookDir = Vector3(0.f, 0.f, 0.f);
	m_InitBillBoardXLookDir = Vector3(0.f, 0.f, 0.f);
}

void CParticleComponent::Update(float DeltaTime)
{
	if (!m_CBuffer)
		return;

	if (m_InitActiveDelayTime > 0.f)
	{
		m_InitActiveDelayTime -= DeltaTime;
		return;
	}

	CSceneComponent::Update(DeltaTime);

	// �Ͻ������� �����Ǵ� Paritlcle ����, ()
	// �ٽ� Activate �ϰ� �� ����, �ٽ� Enable false ó��
	if (m_TempCreateAccTime > 0.f && m_CBuffer->IsDisableNewAlive())
	{
		m_TempCreateAccTime -= DeltaTime;

		if (m_TempCreateAccTime <= 0.f)
		{
			m_TempCreateAccTime = 0.f;

			// ������ �ѹ��� �׷��� Particle �̹Ƿ� Enable False ó�����ش�.
			// Enable(false);
			// ��, �ڽ� Component ���� ��� Enable false ó�����ִ� ���� �ƴ϶�
			// - �ش� Component���� Enable False ó������� �Ѵ�.
			CRef::Enable(false);

			// ������ �ʰ� �� ���̴�.
			// m_Render = false;

			// �ش� �ڵ��� ��ġ�� ���� ������ �߻��� �� �ִ�.
			// Reset();

			return;
		}
	}

	m_SpawnTime += DeltaTime;

	// CBuffer �� DisableNewAlive �� true ���, �ѹ��� �� �����ع����� �ϴ� ��
	// �� ��� �� �ϳ��� m_SpawnTimeMax �� 0���� ���������� ��
	if (m_CBuffer->IsDisableNewAlive() == 1)
	{
		// if (m_CBuffer->IsApplySpawnTimeDuringDisableNewAlive() == false)
		// {
		// 	m_SpawnTimeMax = 0.f;
		// }
	}

	// Spawn Time ����
	if (m_SpawnTime >= m_SpawnTimeMax)
	{
		m_SpawnTime -= m_SpawnTimeMax;
		m_CBuffer->SetSpawnEnable(1);
		m_SpawnTime = 0.f;

		// SpawnTime �� 0���� �����.
		// if (m_CBuffer->IsDisableNewAlive() == 1)
		// {
		// 	m_SpawnTime = 0.f;
		// }
	}
	else
	{
		m_CBuffer->SetSpawnEnable(0);
	}

	// �߰� : Particle �� BillBoard �� �����ϱ����� OBJ �� �߰�
	// if (m_BillBoardEffect)
	// {
	// 	ApplyBillBoardEffect();
	// }
}

void CParticleComponent::PostUpdate(float DeltaTime)
{
	if (!m_CBuffer)
		return;

	if (m_InitActiveDelayTime > 0.f)
		return;

	CSceneComponent::PostUpdate(DeltaTime);

	CParticleConstantBuffer* CBuffer = m_Particle->GetCBuffer();

	// Update Shader�� ���۽�Ų��.
	// Start Min, Max �� ���, 
	// 1) Particle Component �� World Rot �� �ݿ��Ͽ� ȸ�� ��Ų ����
	// 2) Particle Component �� Scalilng ���� �����Ѵ�. (�켱 RelativeScale �� ����)
	// 3) Translation�� ó������ �ʴ´�. StartMin, Max ��, Local Space �󿡼��� Min, Max �� �ǹ��ϰ� �� ���̴�.
	// ( Rot  -> Translation )
	Vector3	StartMin = CBuffer->GetStartMin() * GetWorldScale();
	StartMin = StartMin.TransformCoord(GetRotationMatrix());

	Vector3	StartMax = CBuffer->GetStartMax() * GetWorldScale();
	StartMax = StartMax.TransformCoord(GetRotationMatrix());
	
	// StartMin, Max ��, World Pos �� �������� �ʰ� �Ѱ��� ���̴�.
	// ���� Particle Component Local Space �󿡼��� Min, Max �� ���� �������� ���̴�.
	// StartMin += GetWorldPos();
	// StartMax += GetWorldPos();

	// (���� �ڵ�)
	// StartMin = GetWorldPos() + CBuffer->GetStartMin();
	// StartMax = GetWorldPos() + CBuffer->GetStartMax();

	m_CBuffer->SetStartMin(StartMin);
	m_CBuffer->SetStartMax(StartMax);

	// BillBoard ȿ���� �ְ� �Ǹ�, ParticleComponent �� Y�ุ ȸ����Ų��. (ī�޶� ��������)
	Vector3 BillBoardAngle;

	if (m_BillBoardEffect)
	{
		// Particle Component ��ü�� Rotation Inherit �� ���� �ʴ� ������ �� �ִ�.
		// Vector3 ZLookDir = m_Object->GetWorldAxis(AXIS::AXIS_Z) * -1.f;
		// Vector3 XLookDir = m_Object->GetWorldAxis(AXIS::AXIS_X) * -1.f;
		Vector3 ZLookDir = GetWorldAxis(AXIS::AXIS_Z) * -1.f;
		Vector3 XLookDir = GetWorldAxis(AXIS::AXIS_X) * -1.f;

		// ó�� ��������� ������ ZLookDir, XLoosdkDir �� �������ش�.
		if (m_UpdateInitBillBoardDir)
		{
			m_InitBillBoardZLookDir = Vector3(ZLookDir.x, 0.f, ZLookDir.z);
			m_InitBillBoardXLookDir = Vector3(XLookDir.x, 0.f, XLookDir.z);

			m_UpdateInitBillBoardDir = false;
		}

		// ī�޶� ��� �ٶ󺸰� �����.
		// ī�޶��� ��ġ�� ���´�.
		Vector3 CameraPos = m_Scene->GetCameraManager()->GetCurrentCamera()->GetWorldPos();

		// x ��
		{
		}

		// y��
		{
			Vector3	View = CameraPos - GetWorldPos();
			View.y = 0.f;

			float AngleBetween = m_InitBillBoardZLookDir.Angle(View);
			// float AngleBetween = ZLookDir.Angle(View);

			// Object ���� ������ �ִٸ�, 180 ���� �Ѿ�� ��
			// ������ acos �� 0 ���� 180 �� ������ ������ �����Ѵ�.
			if (View.Dot(m_InitBillBoardXLookDir) > 0)
			// if (View.Dot(XLookDir) > 0)
			{
				AngleBetween = 180.f + (180.f - AngleBetween);
			}

			BillBoardAngle.y = AngleBetween;
		}

		// z ��
		{
		}
		
	}

	// Rotation Angle ������ �����Ѵ�. Transform �� ������ ���� ���̴�.
	// m_CBuffer->SetRotationAngle(GetWorldRot());
	const Vector3& ParticleWorldRot = GetWorldRot();
	m_CBuffer->SetRotationAngle(ParticleWorldRot + BillBoardAngle);
	
	// (�Ʒ� ������ ���� m_TempVCBuffer �� ���� �������ش�.)
	//m_CBuffer->SetCommonWorldScale(GetWorldScale());
	//m_CBuffer->SetCommonParticleComponentWorldPos(ParticleComponentWorldPos);

	const Vector3& ParticleComponentWorldPos = GetWorldPos();
	const Vector3& ParticleCommonWorldScale = GetWorldScale();

	m_CBuffer->UpdateCBuffer();

	// �ӽ� Val �� ���Ǵ� Particle Value �Ѱ��ֱ�
	m_TempVCBuffer->SetCommonWorldScale(ParticleCommonWorldScale);
	m_TempVCBuffer->SetCommonParticleComponentWorldPos(ParticleComponentWorldPos);

	m_TempVCBuffer->UpdateCBuffer();

	size_t	BufferCount = m_vecStructuredBuffer.size();

	// Normal Dist ����ȭ ���� ������ �Ѱ��ش�.
	// m_NormalDistributionBuffer->SetShader();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->SetShader();
	}

	// UpdateShader�� Thread�� 64, 1, 1�� ����ϰ� �ִ�.
	// �����Ǿ��� ��ƼŬ�� ��ü ������ 64���� ������. ���� 64���� �ִ���ƼŬ ���� �������־��ٸ�
	// �ʿ��� �׷��� ���� 1���̴�. ������ 64�� �̸��̶�� 64�� ���� ��� 0�� �����Ƿ� ���⿡ 1�� �����־�� �Ѵ�.
	// 100���� ��� �׷��� 2���� �����ȴ�. �̶� ������� 128���� �ǹǷ� 100���� ������ ������ 28���� ó���� �ȵǰ�
	// �����ָ� �Ǵ°��̴�.
	int	GroupCount = m_Particle->GetSpawnCountMax() / 64 + 1;

	m_UpdateShader->Excute(GroupCount, 1, 1);

	// m_NormalDistributionBuffer->ResetShader();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->ResetShader();
	}

	// ���⿡�� Data�� CopyResource�� �����ؼ� �׽�Ʈ �غ���.
}

void CParticleComponent::PrevRender()
{
	if (m_InitActiveDelayTime > 0.f)
		return;

	CSceneComponent::PrevRender();
}

void CParticleComponent::RenderParticleEffectEditor()
{
	if (!m_CBuffer)
		return;

	size_t	BufferCount = m_vecStructuredBuffer.size();

	CSceneComponent::RenderParticleEffectEditor();

	// ��� ���۸� �ٽ� �ѹ� Setting ���ش�.
	// ��� ���̴� �ܿ���, Render ���������� ��� ���� ������ ����� �� �ְ� �ϴ� ���̴�.
	m_CBuffer->UpdateCBuffer();
	m_TempVCBuffer->UpdateCBuffer();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->SetShader(30 + (int)i, (int)Buffer_Shader_Type::Geometry);
	}

	if (m_Material)
	{
		m_Material->Render();
		m_Material->EnableDecal(m_ReceiveDecal);
	}

	// �ν��Ͻ��� �̿��ؼ� �׷��ش�.
	m_Mesh->RenderInstancing(m_CBuffer->GetSpawnCount());

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->ResetShader(30 + (int)i, (int)Buffer_Shader_Type::Geometry);
	}

	if (m_Material)
		m_Material->Reset();
}

void CParticleComponent::Render()
{
	if (!m_CBuffer)
		return;

	if (m_InitActiveDelayTime > 0.f)
		return;

	// ��� ���̴� �ܿ���, Render ���������� ��� ���� ������ ����� �� �ְ� �ϴ� ���̴�.
	m_CBuffer->UpdateCBuffer();
	m_TempVCBuffer->UpdateCBuffer();

	CSceneComponent::Render();

	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->SetShader(30 + (int)i, (int)Buffer_Shader_Type::Geometry);
	}

	if (m_Material)
	{
		m_Material->Render();
	}

	// �ν��Ͻ��� �̿��ؼ� �׷��ش�.
	m_Mesh->RenderInstancing(m_CBuffer->GetSpawnCount());

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->ResetShader(30 + (int)i, (int)Buffer_Shader_Type::Geometry);
	}

	if (m_Material)
		m_Material->Reset();
}

void CParticleComponent::PostRender()
{
	CSceneComponent::PostRender();

	// Restart ��ư�� ���� �Ѵ�.
	// �ϴ� �ش� ��� ���� ������ ���� GPU ������ �ѹ� �Ѿ�� �ϴϱ�
	// ������ Render Ȥ�� PostUpdate ���� �Ѱ��ְ� ���⼭ ����
	if (m_CBuffer)
	{
		// �Ʒ� �ڵ带 �������־�� �� �ѹ��� �ٽ� �����ǰ� �� ���̴�.
		if (m_CBuffer->IsDisableNewAlive())
		{
			// m_CBuffer->SetResetParticleSharedInfoSumSpawnCnt(false);
			m_TempVCBuffer->SetResetParticleSharedInfoSumSpawnCnt(false);
		}

		// �ش� ���� Render ���������� true �� ���õǾ� �ִٰ� �ϴ��� 
		// ���� �����Ǵ� �༮���� Destroy �Ѵ�.
		m_CBuffer->SetDestroyExstingAllLivingParticles(false);
	}
}

CParticleComponent* CParticleComponent::Clone()
{
	return new CParticleComponent(*this);
}

bool CParticleComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);

	bool Result = SaveOnly(File);

	return Result;
}

bool CParticleComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);

	LoadOnly(File);

	return true;
}

void CParticleComponent::DestroyExistingParticles()
{
	// ���� ����ִ� Particle ���� ��� Alive False �� ������� ���̴�.
	m_CBuffer->SetDestroyExstingAllLivingParticles(true);

	m_TempCreateAccTime = 0.f;

	// ����ȭ ���� ���� Update
	m_CBuffer->UpdateCBuffer();

	// Init Delay Time �� 0���� �ش�.
	m_InitActiveDelayTime = 0.f;

	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->SetShader();
	}

	int	GroupCount = m_Particle->GetSpawnCountMax() / 64 + 1;

	m_UpdateShader->Excute(GroupCount, 1, 1);

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->ResetShader();
	}
}

// Shader ���� �Ѱ��� ����ȭ ���� ������ �ʱ�ȭ ���ִ� �ڵ��̴�.
void CParticleComponent::RecreateOnlyOnceCreatedParticle()
{
	if (!m_Particle)
		return;

	if (m_CBuffer->IsDisableNewAlive() == 0)
		return;

	RecreateOnlyOnceCreatedParticleWithOutLifeTimeSetting();

	// Object Pool ���� ������ Particle�� ��� Destroy ���Ѽ� �ٽ� Pool �� �ǵ��� ���̴�.
	// -  m_TempCreateAccTimeMax ��ŭ LifeTime �� �������� ���̴�.
	if (m_Object->IsInPool())
	{
		// m_Object->SetLifeSpan(m_TempCreateAccTimeMax);
		float MaxLifeSpan = m_Object->GetLifeSpan();
		float ParticleLifeSpan = m_CBuffer->GetLifeTimeMax() + 0.1f;
	
		m_Object->SetLifeSpan(MaxLifeSpan > ParticleLifeSpan ? MaxLifeSpan : ParticleLifeSpan);
	}
}

void CParticleComponent::RecreateOnlyOnceCreatedParticleWithOutLifeTimeSetting()
{
	if (!m_Particle)
		return;

	if (m_CBuffer->IsDisableNewAlive() == 0)
		return;

	CRef::Enable(true);

	// Render �� �ǵ��� �����Ѵ�.
	m_Render = true;

	size_t	BufferCount = m_vecStructuredBuffer.size();

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->SetShader();
	}

	int	GroupCount = m_Particle->GetSpawnCountMax() / 64 + 1;

	// DiableNewAlive �� true �� Particle �� ���ؼ��� �����Ѵ�.
	// DiableNewAlive �� ���, �ѹ��� �����ϰ�, ���̻� �߰� �������� �ʴ� Particle
	// m_CBuffer->SetResetParticleSharedInfoSumSpawnCnt(true);
	m_TempVCBuffer->SetResetParticleSharedInfoSumSpawnCnt(true);

	// �ش� AccTime ���� Enable False �� �ȴ�.
	// m_TempCreateAccTime = m_TempCreateAccTimeMax;
	// m_TempCreateAccTimeMax = m_CBuffer->GetLifeTimeMax() + 0.1f;
	m_TempCreateAccTime = m_CBuffer->GetLifeTimeMax() + 0.01f;

	// ��� ���� ������ Update
	m_TempVCBuffer->SetCommonParticleComponentWorldPos(GetWorldPos());

	m_TempVCBuffer->UpdateCBuffer();
	m_CBuffer->UpdateCBuffer();

	// ��� ���̴� �ѹ� �� ȣ��
	// m_UpdateShader->Excute(GroupCount, 1, 1);
	m_UpdateShader->Excute(GroupCount, 1, 1);

	for (size_t i = 0; i < BufferCount; ++i)
	{
		m_vecStructuredBuffer[i]->ResetShader();
	}
}

bool CParticleComponent::SaveOnly(FILE* File)
{
	// Mesh
	std::string	MeshName = m_Mesh->GetName();
	int	Length = (int)MeshName.length();
	fwrite(&Length, sizeof(int), 1, File);
	fwrite(MeshName.c_str(), sizeof(char), Length, File);

	if (m_Particle->GetName() == "")
	{
		// Particle �� ���, Particle Editor �� ���ؼ� ������ ��, �ݵ�� Particle �̸��� �����ϰ� �Ǿ� �ִ�.
		// Particle �� �̸���, Particle File�� ����� ���� �̸��� �����ϴ�.
		assert(false); 
	}

	// Ȥ�� �𸣴� �ѹ� �� �������ش�.
	m_ParticleName = m_Particle->GetName();
	int	 ParticleNameLength = (int)m_ParticleName.length();
	fwrite(&ParticleNameLength, sizeof(int), 1, File);
	fwrite(m_ParticleName.c_str(), sizeof(char), ParticleNameLength, File);

	// Particle
	// m_Particle->Save(File);
	std::string SaveParticleFileFullName = m_Particle->GetName();
	SaveParticleFileFullName.reserve(100);

	// .ptrc �� �ش� Name�� �ִ��� Ȯ�� ��, ������ Add
	std::string StrExt = ".prtc";

	auto iterFind = std::search(SaveParticleFileFullName.begin(), SaveParticleFileFullName.end(),
		std::boyer_moore_searcher(StrExt.begin(), StrExt.end()));

	if (iterFind == SaveParticleFileFullName.end())
		SaveParticleFileFullName.append(".prtc");

	auto LoadedFullPath = CEngineUtil::CheckAndExtractFullPathOfTargetFile(PARTICLE_PATH, SaveParticleFileFullName);

	if (!LoadedFullPath.has_value())
		assert(false);

	m_Particle->SaveFile(LoadedFullPath.value().c_str());

	fwrite(&m_BillBoardEffect, sizeof(bool), 1, File);
	fwrite(&m_SpawnTimeMax, sizeof(float), 1, File);

	return true;
}

bool CParticleComponent::LoadOnly(FILE* File)
{
	// Mesh
	char	MeshName[256] = {};
	int	Length = 0;
	fread(&Length, sizeof(int), 1, File);
	fread(MeshName, sizeof(char), Length, File);

	// m_Mesh = (CSpriteMesh*)m_Scene->GetResource()->FindMesh(MeshName);
	m_Mesh = (CSpriteMesh*)CSceneManager::GetInst()->GetScene()->GetResource()->FindMesh(MeshName);

	// Particle ����
	m_Particle = CSceneManager::GetInst()->GetScene()->GetResource()->CreateParticleEmpty<CParticle>();
	// m_Particle->Load(File);

	char LoadedParticleName[MAX_PATH] = {};
	int	 ParticleNameLength = (int)m_ParticleName.length();
	fread(&ParticleNameLength, sizeof(int), 1, File);
	fread(LoadedParticleName, sizeof(char), ParticleNameLength, File);

	// Particle
	// m_Particle->Save(File);
	std::string LoadParticleFileFullFileName = LoadedParticleName;

	// .ptrc �� �ش� Name�� �ִ��� Ȯ�� ��, ������ Add
	std::string StrExt = ".prtc";

	auto iterFind = std::search(LoadParticleFileFullFileName.begin(), LoadParticleFileFullFileName.end(),
		std::boyer_moore_searcher(StrExt.begin(), StrExt.end()));

	if (iterFind == LoadParticleFileFullFileName.end())
		LoadParticleFileFullFileName.append(".prtc");

	auto LoadedFullPath = CEngineUtil::CheckAndExtractFullPathOfTargetFile(PARTICLE_PATH, LoadParticleFileFullFileName);

	// �׷��� Load �� �ȵȴٸ�, �ƿ� Load �� prtc ���� ��ü�� �������� �ʴ´ٴ� �ǹ�
	if (!LoadedFullPath.has_value())
		assert(false);

	bool Result = m_Particle->LoadFile(LoadedFullPath.value().c_str());

	if (!Result)
		assert(false);

	m_ParticleName = m_Particle->GetName();

	// SetParticle(m_Particle);
	SetParticleWithOutCloneShader(m_Particle);

	// Load �� Particle �� Particle Manager �� �߰����ش�.
	CResourceManager::GetInst()->GetParticleManager()->AddParticle(m_Particle);

	fread(&m_BillBoardEffect, sizeof(bool), 1, File);
	fread(&m_SpawnTimeMax, sizeof(float), 1, File);

	// Update Bill Board Dir �� �� �� �ֵ��� üũ�Ѵ�.
	m_UpdateInitBillBoardDir = true;

	return true;
}

void CParticleComponent::SetBillBoardEffect(bool Enable)
{
	m_BillBoardEffect = Enable;

	m_UpdateInitBillBoardDir = true;
}
