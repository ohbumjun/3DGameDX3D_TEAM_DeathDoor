
#include "ColliderRay.h"
#include "ColliderBox3D.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Shader/ColliderConstantBuffer.h"
#include "../Collision/Collision.h"
#include "../Render/RenderManager.h"

CColliderRay::CColliderRay()
{
	SetTypeID<CColliderRay>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_ColliderType = Collider_Type::Ray;
}

CColliderRay::CColliderRay(const CColliderRay& com)	:
	CColliderComponent(com)
{
}

CColliderRay::~CColliderRay()
{
}

void CColliderRay::Start()
{
	CColliderComponent::Start();
}

bool CColliderRay::Init()
{
	if (!CColliderComponent::Init())
		return false;

	m_RayInfo.Pos = GetWorldPos();
	m_RayInfo.Dir = Vector3(1.f, 0.f, 0.f);
	
	m_Transform->SetWorldScale(1000.f, 1.f, 1.f);

	m_Mesh = m_Scene->GetResource()->FindMesh("HalfLinePos");

	return true;
}

void CColliderRay::Update(float DeltaTime)
{
	CColliderComponent::Update(DeltaTime);

	Matrix MatWorld = m_Transform->GetWorldMatrix();
	m_EndPos = Vector3(1.f, 0.f, 0.f).TransformCoord(MatWorld);
	m_StartPos = m_RayInfo.Pos;

}

void CColliderRay::PostUpdate(float DeltaTime)
{
	CColliderComponent::PostUpdate(DeltaTime);

	CColliderComponent::Render();

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix	matWorld, matView, matProj, matWVP;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	Matrix	matScale, matRot, matTrans;

	matScale.Scaling(GetWorldScale());
	matRot.Rotation(GetWorldRot());
	matTrans.Translation(GetWorldPos());

	matWorld = matScale * matRot * matTrans;

	m_RayInfo.Pos = GetWorldPos();
	m_RayInfo.Dir = Vector3(1.f, 0.f, 0.f).TransformCoord(matRot);


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

void CColliderRay::PrevRender()
{
	CColliderComponent::PrevRender();
}

void CColliderRay::Render()
{
	bool DebugRender = CRenderManager::GetInst()->IsDebugRender();

	if (!DebugRender)
	{
		return;
	}

	m_CBuffer->UpdateCBuffer();

	m_Shader->SetShader();

	m_Mesh->Render();
}

void CColliderRay::PostRender()
{
	CColliderRay::Render();
}

CColliderRay* CColliderRay::Clone()
{
	return new CColliderRay(*this);
}

bool CColliderRay::Save(FILE* File)
{
	CColliderComponent::Save(File);

	fwrite(&m_RayInfo, sizeof(Ray), 1, File);

	return true;
}

bool CColliderRay::Load(FILE* File)
{
	CColliderComponent::Load(File);

	fread(&m_RayInfo, sizeof(Ray), 1, File);

	return true;
}

bool CColliderRay::Collision(CColliderComponent* Dest)
{
	switch (Dest->GetColliderType())
	{
	case Collider_Type::Box3D:
		return CCollision::CollisionRayToBox3D(this, (CColliderBox3D*)Dest);
	}

	return false;
}

bool CColliderRay::CollisionMouse(const Vector2& MousePos)
{
	return false;
}
