
#include "ColliderHalfLine.h"
#include "ColliderBox3D.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Shader/ColliderConstantBuffer.h"
#include "../Collision/Collision.h"
#include "../Render/RenderManager.h"

CColliderHalfLine::CColliderHalfLine()
{
	SetTypeID<CColliderHalfLine>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_ColliderType = Collider_Type::HalfLine;
}

CColliderHalfLine::CColliderHalfLine(const CColliderHalfLine& com)	:
	CColliderComponent(com)
{
}

CColliderHalfLine::~CColliderHalfLine()
{
}

void CColliderHalfLine::Start()
{
	CColliderComponent::Start();
}

bool CColliderHalfLine::Init()
{
	if (!CColliderComponent::Init())
		return false;

	m_Info.StartPos = GetWorldPos();
	m_Info.EndPos = Vector3(1.f, 0.f, 0.f);

	m_Mesh = m_Scene->GetResource()->FindMesh("HalfLinePos");

	return true;
}

void CColliderHalfLine::Update(float DeltaTime)
{
	CColliderComponent::Update(DeltaTime);
}

void CColliderHalfLine::PostUpdate(float DeltaTime)
{
	CColliderComponent::PostUpdate(DeltaTime);

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix	matWorld, matView, matProj, matWVP;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	Matrix	matScale, matRot, matTrans;

	Vector3 Scale = GetWorldScale();
	matScale.Scaling(Scale);
	matRot.Rotation(GetWorldRot());
	matTrans.Translation(GetWorldPos());
	//matTrans.Translation(m_Info.StartPos);

	matWorld = matScale * matRot * matTrans;

	m_Info.StartPos = GetWorldPos();
	m_Info.EndPos = Vector3(1.f, 0.f, 0.f);

	//m_Info.StartPos = m_Info.StartPos.TransformCoord(matWorld);
	m_Info.EndPos = m_Info.EndPos.TransformCoord(matWorld);

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

void CColliderHalfLine::PrevRender()
{
	CColliderComponent::PrevRender();
}

void CColliderHalfLine::Render()
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

void CColliderHalfLine::PostRender()
{
	CColliderComponent::PostRender();
}

CColliderHalfLine* CColliderHalfLine::Clone()
{
	return new CColliderHalfLine(*this);
}

bool CColliderHalfLine::Save(FILE* File)
{
	return CColliderComponent::Save(File);
}

bool CColliderHalfLine::Load(FILE* File)
{
	return CColliderComponent::Load(File);
}

bool CColliderHalfLine::Collision(CColliderComponent* Dest)
{
	switch (Dest->GetColliderType())
	{
	case Collider_Type::Box3D:
		return CCollision::CollisionBox3DToHalfLine((CColliderBox3D*)Dest, this);
	}

	return false;
}

bool CColliderHalfLine::CollisionMouse(const Vector2& MousePos)
{
	return false;
}

void CColliderHalfLine::RefreshInfo()
{
	Matrix	matWorld;


	Matrix	matScale, matRot, matTrans;

	matScale.Scaling(GetWorldScale());
	matRot.Rotation(GetWorldRot());
	matTrans.Translation(GetWorldPos());

	matWorld = matScale * matRot * matTrans;

	m_Info.StartPos = GetWorldPos();
	m_Info.EndPos = Vector3(1.f, 0.f, 0.f);

	m_Info.EndPos = m_Info.EndPos.TransformCoord(matWorld);
}
