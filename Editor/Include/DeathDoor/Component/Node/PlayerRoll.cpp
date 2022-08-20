
#include "PlayerRoll.h"
#include "Scene/SceneManager.h"
#include "Component/AnimationMeshComponent.h"
#include "../GameStateComponent.h"
#include "../PlayerDataComponent.h"
#include "Input.h"

CPlayerRoll::CPlayerRoll()	:
	m_SoundPlay(false)
{
	SetTypeID(typeid(CPlayerRoll).hash_code());
}

CPlayerRoll::CPlayerRoll(const CPlayerRoll& Node)	:
	CActionNode(Node)
{
}

CPlayerRoll::~CPlayerRoll()
{
}

NodeResult CPlayerRoll::OnStart(float DeltaTime)
{
	CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation("PlayerRoll");

	Data->GetSword()->GetAnimationInstance()->ChangeAnimation("PlayerRoll");

	m_NavAgent = m_Object->FindObjectComponentFromType<CNavAgent>();



	m_CallStart = true;
	m_Owner->SetCurrentNode(this);

	return NodeResult::Node_True;
}

NodeResult CPlayerRoll::OnUpdate(float DeltaTime)
{
	CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (!Data)
		return NodeResult::Node_False;

	if (!m_NavAgent)
		return NodeResult::Node_False;

	//const keyState WState = CInput::GetInst()->FindKeyState('W');
	//const keyState AState = CInput::GetInst()->FindKeyState('A');
	//const keyState SState = CInput::GetInst()->FindKeyState('S');
	//const keyState DState = CInput::GetInst()->FindKeyState('D');

	int Dir = (int)Data->GetRollDirection();

	float Speed = Data->GetMoveSpeed();

	CCameraComponent* CurrentCam = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	Vector3 ZAxis = m_Object->GetWorldAxis(AXIS::AXIS_Z);

	Vector3 FrontVector = Vector3(-ZAxis.x, -ZAxis.y, -ZAxis.z);

	Vector3 MoveDir;
	Vector3 CamRot = CurrentCam->GetTransform()->GetWorldRot();
	CamRot.x = 0.f;
	CamRot.z = 0.f;
	Matrix matRot;
	matRot.Rotation(CamRot);

	if (Dir & (int)DDPlayerRollDirection::Up)
	{
		MoveDir += Vector3(0.f, 0.f, 1.f);

	}

	if (Dir & (int)DDPlayerRollDirection::Left)
	{
		MoveDir += Vector3(-1.f, 0.f, 0.f);

	}

	if (Dir & (int)DDPlayerRollDirection::Down)
	{
		MoveDir += Vector3(0.f, 0.f, -1.f);

	}

	if (Dir & (int)DDPlayerRollDirection::Right)
	{
		MoveDir += Vector3(1.f, 0.f, 0.f);

	}

	if (!m_SoundPlay)
	{
		m_Object->GetScene()->GetResource()->SoundPlay("PlayerRoll");
		m_SoundPlay = true;
	}


	MoveDir = MoveDir.TransformCoord(matRot);
	MoveDir.Normalize();

	if (m_AnimationMeshComp->GetAnimationInstance()->IsCurrentAnimEnd())
	{
		m_Owner->SetCurrentNode(nullptr);
		m_CallStart = false;
		m_IsEnd = true;
		m_SoundPlay = false;

		return NodeResult::Node_True;
	}

	if (MoveDir.x == 0.f && MoveDir.y == 0.f && MoveDir.z == 0.f)
	{
		m_Owner->SetCurrentNode(nullptr);
		m_CallStart = false;
		m_IsEnd = true;
		return NodeResult::Node_True;
	}

	m_Object->SetMoveDir(MoveDir);

	Vector3 CrossVector = Vector3(FrontVector.x, FrontVector.y, -FrontVector.z).Cross(Vector3(MoveDir.x, MoveDir.y, -MoveDir.z));

	bool Over180 = false;

	if (CrossVector.y < 0.f)
		Over180 = true;

	float Degree = FrontVector.Dot(MoveDir);
	Degree = RadianToDegree(acosf(Degree));

	// 180도 넘으면 반시계로 회전
	if (Over180)
	{
		if (Degree > 1.5f)
		{
			m_Object->AddWorldRotationY(360.f * DeltaTime);
		}
	}

	else
	{
		if (Degree > 1.5f)
		{
			m_Object->AddWorldRotationY(-360.f * DeltaTime);
		}
	}

	//m_Object->AddWorldPos(MoveDir.x * Speed * DeltaTime, 0.f, MoveDir.z * Speed * DeltaTime);
	m_NavAgent->MoveOnNavMesh(Vector3(MoveDir.x * Speed * 3.f * DeltaTime, 0.f, MoveDir.z * Speed * 3.f * DeltaTime));

	const keyState SpaceState = CInput::GetInst()->FindKeyState(VK_SPACE);

	m_CallStart = false;

	return NodeResult::Node_True;
}

NodeResult CPlayerRoll::OnEnd(float DeltaTime)
{
	m_CallStart = false;
	m_Owner->SetCurrentNode(nullptr);
	m_IsEnd = false;

	return NodeResult::Node_True;
}
