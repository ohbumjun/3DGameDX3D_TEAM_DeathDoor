
#include "ColliderSphere.h"
#include "ColliderBox3D.h"
#include "../Collision/Collision.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Shader/ColliderConstantBuffer.h"
#include "../Scene/Scene.h"
#include "../Scene/CameraManager.h"
#include "CameraComponent.h"
#include "../Render/RenderManager.h"

CColliderSphere::CColliderSphere()
{
	SetTypeID<CColliderSphere>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_ColliderType = Collider_Type::Sphere;
}

CColliderSphere::CColliderSphere(const CColliderSphere& com) :
	CColliderComponent(com)
{
	m_Info = com.m_Info;
}

CColliderSphere::~CColliderSphere()
{
}

void CColliderSphere::Start()
{
	CColliderComponent::Start();
}

bool CColliderSphere::Init()
{
	if (!CColliderComponent::Init())
		return false;

	m_Info.Radius = 10.f;

	SetInheritRotZ(true);

	SetWorldScale(m_Info.Radius, m_Info.Radius, m_Info.Radius);

	m_Mesh = m_Scene->GetResource()->FindMesh("SphereLinePos");

	return true;
}

void CColliderSphere::Update(float DeltaTime)
{
	CColliderComponent::Update(DeltaTime);

	// Scale�� ���� �� �� ������ �� �����Ӹ��� ���ش�. 
	// Start���� �ѹ��� MeshSize�� �������ָ� Pool���� ���� ���� ��� GetXXX�Լ��� ȣ���ϰ� �� �� ���� Object�� Transform�� �����ϴµ�
	// �׷��� �Ǹ� MeshSize�� (x, y, z) = (FLT_MAX, FLT_MAX, FLT_MAX)�� ���·� MeshSize�� �����ϰ� �� �ڿ� Transform�� �����ϰ� �Ǿ� ������ ������ Update���� ����
	SetMeshSize(m_Info.Max - m_Info.Min);
}

void CColliderSphere::PostUpdate(float DeltaTime)
{
	CColliderComponent::PostUpdate(DeltaTime);

	m_Info.Center = GetWorldPos() + m_Offset;

	m_Min = m_Info.Center - m_Info.Radius;
	m_Max = m_Info.Center + m_Info.Radius;

	m_Info.Min = m_Min;
	m_Info.Max = m_Max;

	SetMeshSize(m_Max - m_Min);
	m_SphereInfo.Center = m_Info.Center;

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix	matWorld, matView, matProj, matWVP;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	Matrix	matScale, matTrans;

	matScale.Scaling(m_Info.Radius, m_Info.Radius, m_Info.Radius);
	//matRot.Rotation(GetWorldRot());
	matTrans.Translation(m_Info.Center);

	matWorld = matScale * matTrans;

	matWVP = matWorld * matView * matProj;

	matWVP.Transpose();

	m_CBuffer->SetWVP(matWVP);

	if (m_PrevCollisionList.empty())
		m_CBuffer->SetColliderColor(Vector4(0.f, 1.f, 0.f, 1.f));

	else
		m_CBuffer->SetColliderColor(Vector4(1.f, 0.f, 0.f, 1.f));

	if (m_MouseCollision)
		m_CBuffer->SetColliderColor(Vector4(1.f, 0.f, 0.f, 1.f));
}

void CColliderSphere::PrevRender()
{
	CColliderComponent::PrevRender();
}

void CColliderSphere::Render()
{
	bool DebugRender = CRenderManager::GetInst()->IsDebugRender();

	if (!DebugRender)
	{
		return;
	}

	CColliderComponent::Render();

	m_CBuffer->UpdateCBuffer();

	m_Shader->SetShader();

	m_Mesh->Render();
}

void CColliderSphere::PostRender()
{
	CColliderComponent::PostRender();
}

CColliderSphere* CColliderSphere::Clone()
{
	return new CColliderSphere(*this);
}

bool CColliderSphere::Save(FILE* File)
{
	CColliderComponent::Save(File);

	fwrite(&m_Info, sizeof(SphereInfo), 1, File);

	return true;
}

bool CColliderSphere::Load(FILE* File)
{
	CColliderComponent::Load(File);

	fread(&m_Info, sizeof(SphereInfo), 1, File);

	return true;
}

bool CColliderSphere::Collision(CColliderComponent* Dest)
{
	switch (Dest->GetColliderType())
	{
	case Collider_Type::Sphere:
		return CCollision::CollisionSphereToSphere((CColliderSphere*)Dest, this);
	case Collider_Type::Box3D:
		return CCollision::CollisionBox3DToSphere((CColliderBox3D*)Dest, this);
	}

	return false;
}

bool CColliderSphere::CollisionMouse(const Vector2& MousePos)
{
	//CollisionResult	result;

	//m_MouseCollision = CCollision::CollisionCircleToPoint(m_MouseResult, result, m_Info, MousePos);

	return m_MouseCollision;
}

bool CColliderSphere::CollisionRay(const Ray& ray)
{
	return false;
}

