
#include "Transform.h"
#include "../Resource/Shader/TransformConstantBuffer.h"
#include "../Scene/Scene.h"
#include "../Scene/CameraManager.h"
#include "../Scene/Navigation3DManager.h"
#include "CameraComponent.h"
#include "../Engine.h"
#include "../Resource/Animation/SkeletonSokcet.h"

static float NormalizeAngle(float angle)
{
	while (angle > 360)
		angle -= 360;
	while (angle < 0)
		angle += 360;
	return angle;
}

static Vector3 NormalizeAngles(Vector3 angles)
{
	angles.x = NormalizeAngle(angles.x);
	angles.y = NormalizeAngle(angles.y);
	angles.z = NormalizeAngle(angles.z);
	return angles;
}

CTransform::CTransform() :
	m_Parent(nullptr),
	m_Scene(nullptr),
	m_Object(nullptr),
	m_Owner(nullptr),
	m_InheritScale(false),
	m_InheritRotX(false),
	m_InheritRotY(false),
	m_InheritRotZ(false),
	m_InheritParentRotationPosX(true),
	m_InheritParentRotationPosY(true),
	m_InheritParentRotationPosZ(true),
	m_UpdateScale(true),
	m_UpdateRot(true),
	m_UpdateRotAxis(false),
	m_UpdatePos(true),
	m_CBuffer(nullptr),
	m_RelativeScale(1.f, 1.f, 1.f),
	m_WorldScale(1.f, 1.f, 1.f),
	m_State(Transform_State::None),
	m_Socket(nullptr),
	m_UpdateByMat(false),
	m_FixTransform(false)
{
	for (int i = 0; i < AXIS_MAX; ++i)
	{
		m_RelativeAxis[i] = Vector3::Axis[i];
		m_WorldAxis[i] = Vector3::Axis[i];
	}
}

CTransform::CTransform(const CTransform& transform)
{
	*this = transform;

	m_CBuffer = transform.m_CBuffer->Clone();
}

CTransform::~CTransform()
{
	SAFE_DELETE(m_CBuffer);

	OnDestroy();
}

CTransformConstantBuffer* CTransform::GetTransformCBuffer() const
{
	return m_CBuffer;
}

void CTransform::SetInstancingInfo(Instancing3DInfo* Info)
{
	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();
	Matrix	matWV = m_matWorld * Camera->GetViewMatrix();

	Info->matWV = matWV;

	Matrix	matWVP = matWV * Camera->GetProjMatrix();

	Info->matWVP = matWVP;

	Info->matWV.Transpose();
	Info->matWVP.Transpose();
}

void CTransform::SetInstancingShadowInfo(Instancing3DInfo* Info)
{
	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	Matrix matWV = m_matWorld * Camera->GetShadowViewMatrix();

	Info->matWV = matWV;

	Matrix matWVP = matWV * Camera->GetShadowProjMatrix();

	Info->matWVP = matWVP;

	Info->matWV.Transpose();
	Info->matWVP.Transpose();
}

void CTransform::InheritScale(bool Current)
{
	if (m_FixTransform)
		return;

	if (m_Parent && m_InheritScale)
		m_WorldScale = m_RelativeScale * m_Parent->GetWorldScale();

	m_UpdateScale = true;

	CallChangeScaleCallBack();

	// 자식이 있을 경우 모두 갱신해준다.
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->InheritScale(false);
	}
}

void CTransform::InheritRotation(bool Current)
{
	if (m_FixTransform)
		return;

	if (m_Parent)
	{
		if (m_InheritRotX)
			m_WorldRot.x = m_RelativeRot.x + m_Parent->GetWorldRot().x;

		if (m_InheritRotY)
			m_WorldRot.y = m_RelativeRot.y + m_Parent->GetWorldRot().y;

		if (m_InheritRotZ)
			m_WorldRot.z = m_RelativeRot.z + m_Parent->GetWorldRot().z;

		if ((m_InheritRotX || m_InheritRotY || m_InheritRotZ) && !Current)
			InheritParentRotationPos(false);
	}

	Vector3	ConvertRot = m_RelativeRot.ConvertAngle();

	XMVECTOR Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);

	Matrix	matRot;
	matRot.RotationQuaternion(Qut);

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		m_RelativeAxis[i] = Vector3::Axis[i].TransformNormal(matRot);
		m_RelativeAxis[i].Normalize();
	}

	ConvertRot = m_WorldRot.ConvertAngle();

	Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);

	matRot.RotationQuaternion(Qut);

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		m_WorldAxis[i] = Vector3::Axis[i].TransformNormal(matRot);
		m_WorldAxis[i].Normalize();
	}

	m_UpdateRot = true;

	CallChangeRotCallBack();

	// 자식이 있을 경우 모두 갱신해준다.
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->InheritRotation(false);
	}
}

void CTransform::InheritParentRotationPos(bool Current)
{
	if (m_FixTransform)
		return;

	if (m_Parent)
	{
		Matrix	matRot;

		Vector3	ParentRot;

		if (m_InheritRotX)
			ParentRot.x = m_Parent->GetWorldRot().x;

		if (m_InheritRotY)
			ParentRot.y = m_Parent->GetWorldRot().y;

		if (m_InheritRotZ)
			ParentRot.z = m_Parent->GetWorldRot().z;

		if (m_InheritRotX || m_InheritRotY || m_InheritRotZ)
		{
			Vector3	ConvertRot = ParentRot.ConvertAngle();

			XMVECTOR Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);

			Matrix	matRot;
			matRot.RotationQuaternion(Qut);

			Vector3	ParentPos = m_Parent->GetWorldPos();

			memcpy(&matRot._41, &ParentPos, sizeof(Vector3));

			m_WorldPos = m_RelativePos.TransformCoord(matRot);
		}
		else
		{
			m_WorldPos = m_RelativePos + m_Parent->GetWorldPos();
		}
	}

	m_UpdatePos = true;

	CallChangePosCallBack();

	// 자식이 있을 경우 모두 갱신해준다.
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->InheritParentRotationPos(false);
	}
}

void CTransform::InheritWorldScale(bool Current)
{
	if (m_FixTransform)
		return;

	if (m_Parent && m_InheritScale)
		m_RelativeScale = m_WorldScale / m_Parent->GetWorldScale();

	m_UpdateScale = true;

	CallChangeScaleCallBack();

	// 자식이 있을 경우 모두 갱신해준다.
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->InheritWorldScale(false);
	}
}

void CTransform::InheritWorldRotation(bool Current)
{
	if (m_FixTransform)
		return;

	if (m_Parent)
	{
		if (m_InheritRotX)
			m_WorldRot.x = m_RelativeRot.x + m_Parent->GetWorldRot().x;

		if (m_InheritRotY)
			m_WorldRot.y = m_RelativeRot.y + m_Parent->GetWorldRot().y;

		if (m_InheritRotZ)
			m_WorldRot.z = m_RelativeRot.z + m_Parent->GetWorldRot().z;

		if ((m_InheritRotX || m_InheritRotY || m_InheritRotZ) && !Current)
			InheritParentRotationPos(false);
	}

	Vector3	ConvertRot = m_RelativeRot.ConvertAngle();

	XMVECTOR Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);

	Matrix	matRot;
	matRot.RotationQuaternion(Qut);

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		m_RelativeAxis[i] = Vector3::Axis[i].TransformNormal(matRot);
		m_RelativeAxis[i].Normalize();
	}

	ConvertRot = m_WorldRot.ConvertAngle();
	Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);
	matRot.RotationQuaternion(Qut);

	for (int i = 0; i < AXIS_MAX; ++i)
	{
		m_WorldAxis[i] = Vector3::Axis[i].TransformNormal(matRot);
		m_WorldAxis[i].Normalize();
	}

	m_UpdateRot = true;

	CallChangeRotCallBack();

	// 자식이 있을 경우 모두 갱신해준다.
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->InheritWorldRotation(false);
	}
}

void CTransform::InheritParentRotationWorldPos(bool Current)
{
	if (m_FixTransform)
		return;

	if (m_Parent)
	{
		Matrix	matRot;

		Vector3	ParentRot;

		if (m_InheritRotX)
			ParentRot.x = m_Parent->GetWorldRot().x;

		if (m_InheritRotY)
			ParentRot.y = m_Parent->GetWorldRot().y;

		if (m_InheritRotZ)
			ParentRot.z = m_Parent->GetWorldRot().z;

		if (m_InheritRotX || m_InheritRotY || m_InheritRotZ)
		{
			Vector3	ConvertRot = ParentRot.ConvertAngle();

			XMVECTOR Qut = XMQuaternionRotationRollPitchYaw(ConvertRot.x, ConvertRot.y, ConvertRot.z);

			Matrix	matRot;
			matRot.RotationQuaternion(Qut);

			Vector3	ParentPos = m_Parent->GetWorldPos();

			memcpy(&matRot._41, &ParentPos, sizeof(Vector3));

			matRot.Inverse();

			m_RelativePos = m_WorldPos.TransformCoord(matRot);
		}

		else
		{
			//m_WorldPos = m_RelativePos + m_Parent->GetWorldPos();
			m_RelativePos = m_WorldPos - m_Parent->GetWorldPos();
		}
	}

	m_UpdatePos = true;

	CallChangePosCallBack();

	// 자식이 있을 경우 모두 갱신해준다.
	size_t	Size = m_vecChild.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecChild[i]->InheritParentRotationPos(false);
	}
}

void CTransform::SetRelativeScale(const Vector3& Scale)
{
	m_RelativeScale = Scale;

	m_WorldScale = Scale;

	InheritScale(true);
}

void CTransform::SetRelativeScale(float x, float y, float z)
{
	SetRelativeScale(Vector3(x, y, z));
}

void CTransform::SetRelativeRotation(const Vector3& Rot)
{
	m_RelativeRot = Rot;

	m_WorldRot = Rot;

	InheritRotation(true);
}

void CTransform::SetRelativeRotation(float x, float y, float z)
{
	SetRelativeRotation(Vector3(x, y, z));
}

void CTransform::SetRelativeRotationX(float x)
{
	Vector3	Rot(x, m_RelativeRot.y, m_RelativeRot.z);

	SetRelativeRotation(Rot);
}

void CTransform::SetRelativeRotationY(float y)
{
	Vector3	Rot(m_RelativeRot.x, y, m_RelativeRot.z);

	SetRelativeRotation(Rot);
}

void CTransform::SetRelativeRotationZ(float z)
{
	Vector3	Rot(m_RelativeRot.x, m_RelativeRot.y, z);

	SetRelativeRotation(Rot);
}

void CTransform::SetRelativePos(const Vector3& Pos)
{
	m_RelativePos = Pos;

	m_WorldPos = Pos;

	InheritParentRotationPos(true);
}

void CTransform::SetRelativePos(float x, float y, float z)
{
	Vector3	Pos(x, y, z);

	SetRelativePos(Pos);
}

void CTransform::AddRelativeScale(const Vector3& Scale)
{
	m_RelativeScale += Scale;

	m_WorldScale = m_RelativeScale;

	InheritScale(true);
}

void CTransform::AddRelativeScale(float x, float y, float z)
{
	Vector3	Scale(x, y, z);

	AddRelativeScale(Scale);
}

void CTransform::AddRelativeRotation(const Vector3& Rot)
{
	m_RelativeRot += Rot;

	m_WorldRot = m_RelativeRot;

	InheritRotation(true);
}

void CTransform::AddRelativeRotation(float x, float y, float z)
{
	Vector3	Rot(x, y, z);

	AddRelativeRotation(Rot);
}

void CTransform::AddRelativeRotationX(float x)
{
	Vector3	Rot(x, 0.f, 0.f);

	AddRelativeRotation(Rot);
}

void CTransform::AddRelativeRotationY(float y)
{
	Vector3	Rot(0.f, y, 0.f);

	AddRelativeRotation(Rot);
}

void CTransform::AddRelativeRotationZ(float z)
{
	Vector3	Rot(0.f, 0.f, z);

	AddRelativeRotation(Rot);
}

void CTransform::AddRelativePos(const Vector3& Pos)
{
	m_RelativePos += Pos;

	m_WorldPos = m_RelativePos;

	InheritParentRotationPos(true);
}

void CTransform::AddRelativePos(float x, float y, float z)
{
	Vector3	Pos(x, y, z);

	AddRelativePos(Pos);
}

void CTransform::SetTransformByWorldMatrix(const Matrix& matTRS)
{
	m_UpdateByMat = true;
	
	m_matWorld = matTRS;
}

void CTransform::SetWorldScale(const Vector3& Scale)
{
	m_WorldScale = Scale;

	m_RelativeScale = m_WorldScale;

	InheritWorldScale(true);
}

void CTransform::SetWorldScale(float x, float y, float z)
{
	Vector3	Scale(x, y, z);

	SetWorldScale(Scale);
}

void CTransform::SetWorldRotation(const Vector3& Rot)
{
	m_WorldRot = Rot;

	m_RelativeRot = m_WorldRot;

	InheritWorldRotation(true);
}

void CTransform::SetWorldRotation(float x, float y, float z)
{
	Vector3	Rot(x, y, z);

	SetWorldRotation(Rot);
}

void CTransform::SetWorldRotationX(float x)
{
	Vector3	Rot(x, m_WorldRot.y, m_WorldRot.z);

	SetWorldRotation(Rot);
}

void CTransform::SetWorldRotationY(float y)
{
	Vector3	Rot(m_WorldRot.x, y, m_WorldRot.z);

	SetWorldRotation(Rot);
}

void CTransform::SetWorldRotationZ(float z)
{
	Vector3	Rot(m_WorldRot.x, m_WorldRot.y, z);

	SetWorldRotation(Rot);
}

void CTransform::SetWorldPos(const Vector3& Pos)
{
	m_WorldPos = Pos;
	m_RelativePos = Pos;

	InheritParentRotationWorldPos(true);
}

void CTransform::SetWorldPos(float x, float y, float z)
{
	Vector3	Pos(x, y, z);

	SetWorldPos(Pos);
}

void CTransform::SetRotationAxis(const Vector3& OriginDir, const Vector3& View)
{
	m_UpdateRotAxis = true;

	Vector3 Axis = OriginDir.Cross(View);
	Axis.Normalize();

	float Angle = OriginDir.Angle(View);

	m_matRot = XMMatrixRotationAxis(Axis.Convert(), DegreeToRadian(Angle));
}

void CTransform::AddWorldScale(const Vector3& Scale)
{
	m_WorldScale += Scale;

	m_RelativeScale = m_WorldScale;

	InheritWorldScale(true);
}

void CTransform::AddWorldScale(float x, float y, float z)
{
	Vector3	Scale(x, y, z);

	AddWorldScale(Scale);
}

void CTransform::AddWorldRotation(const Vector3& Rot)
{
	m_WorldRot += Rot;

	m_RelativeRot = m_WorldRot;

	InheritWorldRotation(true);
}

void CTransform::AddWorldRotation(float x, float y, float z)
{
	Vector3	Rot(x, y, z);

	AddWorldRotation(Rot);
}

void CTransform::AddWorldRotationX(float x)
{
	Vector3	Rot(x, 0.f, 0.f);

	AddWorldRotation(Rot);
}

void CTransform::AddWorldRotationY(float y)
{
	Vector3	Rot(0.f, y, 0.f);

	AddWorldRotation(Rot);
}

void CTransform::AddWorldRotationZ(float z)
{
	Vector3	Rot(0.f, 0.f, z);

	AddWorldRotation(Rot);
}

void CTransform::AddWorldPos(const Vector3& Pos)
{
	m_WorldPos += Pos;
	m_RelativePos = m_WorldPos;

	InheritParentRotationWorldPos(true);
}

void CTransform::AddWorldPos(float x, float y, float z)
{
	Vector3	Pos(x, y, z);

	AddWorldPos(Pos);
}

void CTransform::AddWorldPosByLocalAxis(AXIS Axis, float Amount)
{
	Vector3 Move =  m_WorldAxis[Axis] * Amount;
	AddWorldPos(Move);
}

void CTransform::AddWorldPosByLocalAxis(const Vector3& Pos)
{
	Vector3 Move = m_WorldAxis[AXIS::AXIS_X] * Pos.x + m_WorldAxis[AXIS::AXIS_Y] * Pos.y + m_WorldAxis[AXIS::AXIS_Z] * Pos.z;
	AddWorldPos(Pos);
}

void CTransform::Start()
{
	InheritScale(true);
	InheritRotation(true);
	InheritParentRotationPos(true);
}

void CTransform::Init()
{
	m_CBuffer = new CTransformConstantBuffer;

	m_CBuffer->Init();
}

void CTransform::Update(float DeltaTime)
{
}

void CTransform::PostUpdate(float DeltaTime)
{
	if (m_UpdateByMat)
	{
		return;
	}

	if (m_State == Transform_State::Ground)
	{
		float Height = m_Scene->GetNavigation3DManager()->GetY(m_WorldPos);

		if (Height != m_WorldPos.y)
		{
			m_WorldPos.y = Height;
			m_RelativePos = m_WorldPos;

			InheritParentRotationWorldPos(true);
		}
	}

	// 소켓이 있을 경우 부모로 소켓을 곱해준다.
	if (m_Socket)
	{
		if (m_UpdateScale)
		{
			m_matScale.Scaling(m_RelativeScale);
		}
		
		// 축 기준 회전을 적용받는 경우, Rotaton Matrix가 이미 계산되어 있다.
		if (m_UpdateRot && !m_UpdateRotAxis)
		{
			m_matRot.Rotation(m_RelativeRot);
		}

		if (m_UpdatePos)
		{
			m_matPos.Translation(m_RelativePos);
		}

		if (m_UpdateScale || m_UpdateRot || m_UpdateRotAxis || m_UpdatePos )
		{
			m_matWorld = m_matScale * m_matRot * m_matPos;
		}

		// Local World * Socket ( Socket Local Transform * Bone * Parent World ) = Final World
		m_matWorld *= m_Socket->GetSocketMatrix();
	}
	else
	{
		Vector3 WorldPos = m_WorldPos;

		if (CEngine::GetInst()->GetEngineSpace() == Engine_Space::Space2D)
		{
			WorldPos.z = WorldPos.y / 30000.f * 1000.f;
		}

		if (m_UpdateScale)
		{
			m_matScale.Scaling(m_WorldScale);
		}
		
		if (m_UpdateRot && !m_UpdateRotAxis)
		{
			m_matRot.Rotation(m_WorldRot);
		}

		if (m_UpdatePos)
		{
			m_matPos.Translation(m_WorldPos);
		}

		if (m_UpdateScale || m_UpdateRot || m_UpdateRotAxis || m_UpdatePos)
		{
			m_matWorld = m_matScale * m_matRot * m_matPos;
		}
	}
}

void CTransform::SetTransform()
{
	m_CBuffer->SetWorldMatrix(m_matWorld);

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	m_CBuffer->SetViewMatrix(Camera->GetViewMatrix());
	m_CBuffer->SetProjMatrix(Camera->GetProjMatrix());

	m_CBuffer->SetPivot(m_Pivot);
	m_CBuffer->SetMeshSize(m_MeshSize);

	m_CBuffer->UpdateCBuffer();
}

void CTransform::SetTransformShadow()
{
	m_CBuffer->SetWorldMatrix(m_matWorld);

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	m_CBuffer->SetViewMatrix(Camera->GetShadowViewMatrix());
	m_CBuffer->SetProjMatrix(Camera->GetShadowProjMatrix());

	m_CBuffer->SetPivot(m_Pivot);
	m_CBuffer->SetMeshSize(m_MeshSize);

	m_CBuffer->UpdateCBuffer();
}

void CTransform::ComputeWorld()
{
	m_matWorld = m_matScale * m_matRot * m_matPos;

	if (m_Socket)
	{
		m_matWorld *= m_Socket->GetSocketMatrix();
	}
}

void CTransform::DecomposeWorld()
{
	XMVECTOR vScale;
	XMVECTOR vRotQ;
	XMVECTOR vPos;
	XMMatrixDecompose(&vScale, &vRotQ, &vPos, m_matWorld.m);

	m_WorldScale = vScale;
	m_WorldPos = vPos;

	// Quaternion to Euler Angle
	// 출처 http://www.littlecandle.co.kr/bbs/board.php?bo_table=codingnote&wr_id=174&page=2
	float w, x, y, z;
	w = vRotQ.m128_f32[3];
	x = vRotQ.m128_f32[0];
	y = vRotQ.m128_f32[1];
	z = vRotQ.m128_f32[2];

	float sqW = w * w;
	float sqX = x * x;
	float sqY = y * y;
	float sqZ = z * z;
	float unit = sqX + sqY + sqZ + sqW;
	float test = x * w - y * z;
	Vector3 v;

	if (test > 0.4955f * unit)
	{
		v.y = 2.f * atan2f(y, x);
		v.x = PI / 2.f;
		v.z = 0;
		v = v * (180.f / PI);
	}
	else if (test < -0.4995f * unit)
	{
		v.x = -2.f * atan2f(y, x);
		v.x = -PI / 2.f;
		v.z = 0;
		v = v * (180.f / PI);
	}
	else
	{
		Vector4 Quat(w, z, x, y);
		v.y = (float)atan2f(2.f * Quat.x * Quat.w + 2.f * Quat.y * Quat.z, 1.f - 2.f * (Quat.z * Quat.z + Quat.w + Quat.w)); // Yaw
		v.x = (float)asinf(2.f * (Quat.x * Quat.z - Quat.w * Quat.y)); // Pitch
		v.z = (float)atan2(2.f * Quat.x * Quat.y + 2.f * Quat.z * Quat.w, 1.f - 2.f * (Quat.y * Quat.y + Quat.z * Quat.z)); // Roll
		v = v * (180.f / PI);
	}
	NormalizeAngles(v);
	m_WorldRot = v;

	m_RelativePos = m_WorldPos;
	m_RelativeScale = m_WorldScale;
	m_RelativeRot = m_WorldRot;

	if (m_Parent)
	{
		m_RelativePos = m_Parent->GetWorldPos() - m_RelativePos;
		m_RelativeScale = m_RelativeScale / m_Parent->GetWorldScale();
		m_RelativeRot = m_Parent->GetWorldRot() - m_RelativeRot;
	}

	if (m_vecChild.size() > 0)
	{
		InheritParentRotationPos(true);
		InheritRotation(true);
		InheritScale(true);
	}

	CallChangePosCallBack();
	CallChangeRotCallBack();
	CallChangeScaleCallBack();
}

CTransform* CTransform::Clone()
{
	return new CTransform(*this);
}

void CTransform::Save(FILE* File)
{
	fwrite(&m_InheritScale, sizeof(bool), 1, File);
	fwrite(&m_InheritRotX, sizeof(bool), 1, File);
	fwrite(&m_InheritRotY, sizeof(bool), 1, File);
	fwrite(&m_InheritRotZ, sizeof(bool), 1, File);

	fwrite(&m_InheritParentRotationPosX, sizeof(bool), 1, File);
	fwrite(&m_InheritParentRotationPosY, sizeof(bool), 1, File);
	fwrite(&m_InheritParentRotationPosZ, sizeof(bool), 1, File);

	fwrite(&m_RelativeScale, sizeof(Vector3), 1, File);
	fwrite(&m_RelativeRot, sizeof(Vector3), 1, File);
	fwrite(&m_RelativePos, sizeof(Vector3), 1, File);
	fwrite(&m_RelativeAxis, sizeof(Vector3), AXIS_MAX, File);

	fwrite(&m_WorldScale, sizeof(Vector3), 1, File);
	fwrite(&m_WorldRot, sizeof(Vector3), 1, File);
	fwrite(&m_WorldPos, sizeof(Vector3), 1, File);
	fwrite(&m_WorldAxis, sizeof(Vector3), AXIS_MAX, File);
	fwrite(&m_Pivot, sizeof(Vector3), 1, File);
	fwrite(&m_MeshSize, sizeof(Vector3), 1, File);
}

void CTransform::Load(FILE* File)
{
	fread(&m_InheritScale, sizeof(bool), 1, File);
	fread(&m_InheritRotX, sizeof(bool), 1, File);
	fread(&m_InheritRotY, sizeof(bool), 1, File);
	fread(&m_InheritRotZ, sizeof(bool), 1, File);

	fread(&m_InheritParentRotationPosX, sizeof(bool), 1, File);
	fread(&m_InheritParentRotationPosY, sizeof(bool), 1, File);
	fread(&m_InheritParentRotationPosZ, sizeof(bool), 1, File);

	fread(&m_RelativeScale, sizeof(Vector3), 1, File);
	fread(&m_RelativeRot, sizeof(Vector3), 1, File);
	fread(&m_RelativePos, sizeof(Vector3), 1, File);
	fread(&m_RelativeAxis, sizeof(Vector3), AXIS_MAX, File);

	fread(&m_WorldScale, sizeof(Vector3), 1, File);
	fread(&m_WorldRot, sizeof(Vector3), 1, File);
	fread(&m_WorldPos, sizeof(Vector3), 1, File);
	fread(&m_WorldAxis, sizeof(Vector3), AXIS_MAX, File);
	fread(&m_Pivot, sizeof(Vector3), 1, File);
	fread(&m_MeshSize, sizeof(Vector3), 1, File);
}

void CTransform::SetAnimationTransform()
{
	m_CBuffer->SetWorldMatrix(m_matWorld);

	CCameraComponent * Camera = m_Scene->GetCameraManager()->GetAnimationEditorCamera();
	// CCameraComponent * Camera = m_Scene->GetCameraManager()->GetCurrentCamera();

	m_CBuffer->SetViewMatrix(Camera->GetViewMatrix());
	m_CBuffer->SetProjMatrix(Camera->GetProjMatrix());

	m_CBuffer->SetPivot(m_Pivot);
	m_CBuffer->SetMeshSize(m_MeshSize);

	m_CBuffer->UpdateCBuffer();
}

void CTransform::SetParticleEffectEditorTransform()
{
	m_CBuffer->SetWorldMatrix(m_matWorld);

	CCameraComponent* Camera = m_Scene->GetCameraManager()->GetParticleEffectEditorCamera();

	if (Camera)
	{
		m_CBuffer->SetViewMatrix(Camera->GetViewMatrix());
		m_CBuffer->SetProjMatrix(Camera->GetProjMatrix());
	}

	m_CBuffer->SetPivot(m_Pivot);
	m_CBuffer->SetMeshSize(m_MeshSize);

	m_CBuffer->UpdateCBuffer();
}

void CTransform::CallChangePosCallBack()
{
	auto iter = m_PosChangeCallBackList.begin();
	auto iterEnd = m_PosChangeCallBackList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter).Func(m_WorldPos, m_RelativePos);
	}
}

void CTransform::CallChangeRotCallBack()
{
	auto iter = m_RotChangeCallBackList.begin();
	auto iterEnd = m_RotChangeCallBackList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter).Func(m_WorldRot, m_RelativeRot);
	}
}

void CTransform::CallChangeScaleCallBack()
{
	auto iter = m_ScaleChangeCallBackList.begin();
	auto iterEnd = m_ScaleChangeCallBackList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter).Func(m_WorldScale, m_RelativeScale);
	}
}

void CTransform::OnDestroy()
{
	auto iter = m_OnDestroyCallBackList.begin();
	auto iterEnd = m_OnDestroyCallBackList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)();
	}
}

void CTransform::DeleteChangePosCallBack(void* Callee)
{
	auto iter = m_PosChangeCallBackList.begin();
	auto iterEnd = m_PosChangeCallBackList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter).Callee == Callee)
		{
			m_PosChangeCallBackList.erase(iter);
			return;
		}
	}
}

void CTransform::DeleteChangeScaleCallBack(void* Callee)
{
	auto iter = m_ScaleChangeCallBackList.begin();
	auto iterEnd = m_ScaleChangeCallBackList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter).Callee == Callee)
		{
			m_ScaleChangeCallBackList.erase(iter);
			return;
		}
	}
}

void CTransform::DeleteChangeRotCallBack(void* Callee)
{
	auto iter = m_RotChangeCallBackList.begin();
	auto iterEnd = m_RotChangeCallBackList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter).Callee == Callee)
		{
			m_RotChangeCallBackList.erase(iter);
			return;
		}
	}
}

void CTransform::ForceUpdateMat()
{
	if (m_Socket)
	{
		if (m_UpdateScale)
		{
			m_matScale.Scaling(m_RelativeScale);
		}

		// 축 기준 회전을 적용받는 경우, Rotaton Matrix가 이미 계산되어 있다.
		if (m_UpdateRot && !m_UpdateRotAxis)
		{
			m_matRot.Rotation(m_RelativeRot);
		}

		if (m_UpdatePos)
		{
			m_matPos.Translation(m_RelativePos);
		}

		if (m_UpdateScale || m_UpdateRot || m_UpdateRotAxis || m_UpdatePos)
		{
			m_matWorld = m_matScale * m_matRot * m_matPos;
		}

		// Local World * Socket ( Socket Local Transform * Bone * Parent World ) = Final World
		m_matWorld *= m_Socket->GetSocketMatrix();
	}
	else
	{
		Vector3 WorldPos = m_WorldPos;

		if (CEngine::GetInst()->GetEngineSpace() == Engine_Space::Space2D)
		{
			WorldPos.z = WorldPos.y / 30000.f * 1000.f;
		}

		if (m_UpdateScale)
		{
			m_matScale.Scaling(m_WorldScale);
		}

		if (m_UpdateRot && !m_UpdateRotAxis)
		{
			m_matRot.Rotation(m_WorldRot);
		}

		if (m_UpdatePos)
		{
			m_matPos.Translation(m_WorldPos);
		}

		if (m_UpdateScale || m_UpdateRot || m_UpdateRotAxis || m_UpdatePos)
		{
			m_matWorld = m_matScale * m_matRot * m_matPos;
		}
	}
}

