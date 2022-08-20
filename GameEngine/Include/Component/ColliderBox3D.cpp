
#include "ColliderBox3D.h"
#include "ColliderBox2D.h"
#include "ColliderCircle.h"
#include "ColliderPixel.h"
#include "ColliderSphere.h"
#include "ColliderHalfLine.h"
#include "ColliderRay.h"
#include "../Collision/Collision.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneResource.h"
#include "../Resource/Shader/ColliderConstantBuffer.h"
#include "../Render/RenderManager.h"

CColliderBox3D::CColliderBox3D()	:
	m_UpdateMinMax(false)
{
	SetTypeID<CColliderBox3D>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_ColliderType = Collider_Type::Box3D;

	m_Transform->AddChangePosCallBack<CColliderBox3D>(this, &CColliderBox3D::UpdateTransform);
	m_Transform->AddChangeRotCallBack<CColliderBox3D>(this, &CColliderBox3D::UpdateTransform);
	m_Transform->AddChangeScaleCallBack<CColliderBox3D>(this, &CColliderBox3D::UpdateTransform);

}

CColliderBox3D::CColliderBox3D(const CColliderBox3D& com) :
	CColliderComponent(com)
{
	m_Info = com.m_Info;
}

CColliderBox3D::~CColliderBox3D()
{
}

void CColliderBox3D::UpdateTransform(const Vector3& World, const Vector3& Relative)
{
	UpdateMinMax();
}

void CColliderBox3D::ForceUpdateCBuffer()
{
	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix	matWorld, matView, matProj, matWVP;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	Matrix	matScale, matRot, matTrans;

	matScale.Scaling(m_Info.AxisLen[0] * 2.f, m_Info.AxisLen[1] * 2.f, m_Info.AxisLen[2] * 2.f);
	matRot.Rotation(GetWorldRot());
	matTrans.Translation(m_Info.Center);

	matWorld = matScale * matRot * matTrans;

	matWVP = matWorld * matView * matProj;

	matWVP.Transpose();

	m_CBuffer->SetWVP(matWVP);

	if (m_PrevCollisionList.empty())
		m_CBuffer->SetColliderColor(Vector4(0.f, 1.f, 0.f, 1.f));

	else
		m_CBuffer->SetColliderColor(Vector4(1.f, 0.f, 0.f, 1.f));
}


void CColliderBox3D::Start()
{
	CColliderComponent::Start();

	UpdateMinMax();

}

bool CColliderBox3D::Init()
{
	if (!CColliderComponent::Init())
		return false;

	m_Info.Axis[0] = Vector3(1.f, 0.f, 0.f);
	m_Info.Axis[1] = Vector3(0.f, 1.f, 0.f);
	m_Info.Axis[2] = Vector3(0.f, 0.f, 1.f);

	m_Info.AxisLen[0] = 0.5f;
	m_Info.AxisLen[1] = 0.5f;
	m_Info.AxisLen[2] = 0.5f;

	m_Info.Min.x = m_Info.Center.x - m_Info.Axis[0].x * m_Info.AxisLen[0];
	m_Info.Min.y = m_Info.Center.y - m_Info.Axis[1].y * m_Info.AxisLen[1];
	m_Info.Min.z = m_Info.Center.z - m_Info.Axis[2].z * m_Info.AxisLen[2];

	m_Info.Max.x = m_Info.Center.x + m_Info.Axis[0].x * m_Info.AxisLen[0];
	m_Info.Max.y = m_Info.Center.y + m_Info.Axis[1].y * m_Info.AxisLen[1];
	m_Info.Max.z = m_Info.Center.z + m_Info.Axis[2].z * m_Info.AxisLen[2];

	//SetInheritRotZ(true);

	SetWorldScale(m_Info.AxisLen[0] * 2.f, m_Info.AxisLen[1] * 2.f, m_Info.AxisLen[2] * 2.f);

	m_Mesh = m_Scene->GetResource()->FindMesh("CubeLinePos");

	return true;
}

void CColliderBox3D::Update(float DeltaTime)
{
	CColliderComponent::Update(DeltaTime);

	m_Info.Center.x = GetWorldPos().x + m_Offset.x;
	m_Info.Center.y = GetWorldPos().y + m_Offset.y;
	m_Info.Center.z = GetWorldPos().z + m_Offset.z;

	m_Info.Axis[0].x = GetWorldAxis(AXIS_X).x;
	m_Info.Axis[0].y = GetWorldAxis(AXIS_X).y;
	m_Info.Axis[0].z = GetWorldAxis(AXIS_X).z;

	m_Info.Axis[1].x = GetWorldAxis(AXIS_Y).x;
	m_Info.Axis[1].y = GetWorldAxis(AXIS_Y).y;
	m_Info.Axis[1].z = GetWorldAxis(AXIS_Y).z;

	m_Info.Axis[2].x = GetWorldAxis(AXIS_Z).x;
	m_Info.Axis[2].y = GetWorldAxis(AXIS_Z).y;
	m_Info.Axis[2].z = GetWorldAxis(AXIS_Z).z;

	m_Info.AxisLen[0] = GetWorldScale().x / 2.f;
	m_Info.AxisLen[1] = GetWorldScale().y / 2.f;
	m_Info.AxisLen[2] = GetWorldScale().z / 2.f;

	// Scale이 변할 수 도 있으니 매 프레임마다 해준다. 
	// Start에서 한번만 MeshSize를 설정해주면 Pool에서 꺼내 오는 경우 GetXXX함수를 호출하고 난 뒤 얻은 Object로 Transform을 수정하는데
	// 그렇게 되면 MeshSize를 (x, y, z) = (FLT_MAX, FLT_MAX, FLT_MAX)인 상태로 MeshSize를 설정하고 난 뒤에 Transform을 수정하게 되어 버리기 때문에 Update에서 해줌
	SetMeshSize(m_Info.Max - m_Info.Min);
}

void CColliderBox3D::PostUpdate(float DeltaTime)
{
	CColliderComponent::PostUpdate(DeltaTime);

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix	matWorld, matView, matProj, matWVP;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	Matrix	matScale, matRot, matTrans;

	matScale.Scaling(m_Info.AxisLen[0] * 2.f, m_Info.AxisLen[1] * 2.f, m_Info.AxisLen[2] * 2.f);
	matRot.Rotation(GetWorldRot());
	matTrans.Translation(m_Info.Center);

	matWorld = matScale * matRot * matTrans;

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

void CColliderBox3D::PrevRender()
{
	CColliderComponent::PrevRender();
}

void CColliderBox3D::Render()
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

void CColliderBox3D::PostRender()
{
	CColliderComponent::PostRender();
}

CColliderBox3D* CColliderBox3D::Clone()
{
	return new CColliderBox3D(*this);
}

bool CColliderBox3D::Save(FILE* File)
{
	CColliderComponent::Save(File);

	fwrite(&m_Info.Center, sizeof(Vector3), 1, File);
	fwrite(&m_Info.Axis[0], sizeof(Vector3), 1, File);
	fwrite(&m_Info.Axis[1], sizeof(Vector3), 1, File);
	fwrite(&m_Info.Axis[2], sizeof(Vector3), 1, File);
	fwrite(&m_Info.AxisLen[0], sizeof(float), 1, File);
	fwrite(&m_Info.AxisLen[1], sizeof(float), 1, File);
	fwrite(&m_Info.AxisLen[2], sizeof(float), 1, File);

	return true;
}

bool CColliderBox3D::Load(FILE* File)
{
	CColliderComponent::Load(File);

	fread(&m_Info.Center, sizeof(Vector3), 1, File);
	fread(&m_Info.Axis[0], sizeof(Vector3), 1, File);
	fread(&m_Info.Axis[1], sizeof(Vector3), 1, File);
	fread(&m_Info.Axis[2], sizeof(Vector3), 1, File);
	fread(&m_Info.AxisLen[0], sizeof(float), 1, File);
	fread(&m_Info.AxisLen[1], sizeof(float), 1, File);
	fread(&m_Info.AxisLen[2], sizeof(float), 1, File);

	m_Info.Min = m_Min;
	m_Info.Max = m_Max;

	return true;
}

bool CColliderBox3D::Collision(CColliderComponent* Dest)
{
	switch (Dest->GetColliderType())
	{
		//case Collider_Type::Box2D:
		//	return CCollision::CollisionBox2DToBox2D(this, (CColliderBox2D*)Dest);
		//case Collider_Type::Circle:
		//	return CCollision::CollisionBox2DToCircle(this, (CColliderCircle*)Dest);
		//case Collider_Type::Pixel:
		//	return CCollision::CollisionBox2DToPixel(this, (CColliderPixel*)Dest);
	case Collider_Type::Box3D:
		return CCollision::CollisionBox3DToBox3D(this, (CColliderBox3D*)Dest);
	case Collider_Type::Sphere:
		return CCollision::CollisionBox3DToSphere(this, (CColliderSphere*)Dest);
	case Collider_Type::Ray:
		return CCollision::CollisionRayToBox3D((CColliderRay*)Dest, this);
	case Collider_Type::HalfLine:
		return CCollision::CollisionBox3DToHalfLine(this, (CColliderHalfLine*)Dest);
	}

	// OBJ 추가 (Warning : 모든 경로 리턴값 반환)
	return false;
}

bool CColliderBox3D::CollisionMouse(const Vector2& MousePos)
{
	return false;
}

void CColliderBox3D::UpdateMinMax()
{
	Matrix	matWorld;
	Matrix	matScale, matRot, matTrans;

	matScale.Scaling(m_Info.AxisLen[0] * 2.f, m_Info.AxisLen[1] * 2.f, m_Info.AxisLen[2] * 2.f);
	matRot.Rotation(GetWorldRot());
	matTrans.Translation(GetWorldPos());

	matWorld = matScale * matRot * matTrans;

	Vector3 P1 = Vector3(-0.5f, 0.5f, -0.5f);
	Vector3 P2 = Vector3(0.5f, 0.5f, -0.5f);
	Vector3 P3 = Vector3(-0.5f, -0.5f, -0.5f);
	Vector3 P4 = Vector3(0.5f, -0.5f, -0.5f);
	Vector3 P5 = Vector3(-0.5f, 0.5f, 0.5f);
	Vector3 P6 = Vector3(0.5f, 0.5f, 0.5f);
	Vector3 P7 = Vector3(-0.5f, -0.5f, 0.5f);
	Vector3 P8 = Vector3(0.5f, -0.5f, 0.5f);

	P1 = P1.TransformCoord(matWorld);
	P2 = P2.TransformCoord(matWorld);
	P3 = P3.TransformCoord(matWorld);
	P4 = P4.TransformCoord(matWorld);
	P5 = P5.TransformCoord(matWorld);
	P6 = P6.TransformCoord(matWorld);
	P7 = P7.TransformCoord(matWorld);
	P8 = P8.TransformCoord(matWorld);

	std::vector<Vector3> PointList;
	PointList.push_back(P1);
	PointList.push_back(P2);
	PointList.push_back(P3);
	PointList.push_back(P4);
	PointList.push_back(P5);
	PointList.push_back(P6);
	PointList.push_back(P6);
	PointList.push_back(P7);
	PointList.push_back(P8);

	std::sort(PointList.begin(), PointList.end(), SortX);
	m_Info.Min.x = PointList[0].x;
	m_Info.Max.x = PointList[7].x;

	std::sort(PointList.begin(), PointList.end(), SortY);
	m_Info.Min.y = PointList[0].y;
	m_Info.Max.y = PointList[7].y;

	std::sort(PointList.begin(), PointList.end(), SortZ);
	m_Info.Min.z = PointList[0].z;
	m_Info.Max.z = PointList[7].z;

	m_Min = m_Info.Min;
	m_Max = m_Info.Max;

	m_UpdateMinMax = true;
}

bool CColliderBox3D::SortX(const Vector3& Src, const Vector3& Dest)
{
	return Src.x < Dest.x;
}

bool CColliderBox3D::SortY(const Vector3& Src, const Vector3& Dest)
{
	return Src.y < Dest.y;
}

bool CColliderBox3D::SortZ(const Vector3& Src, const Vector3& Dest)
{
	return Src.z < Dest.z;
}

