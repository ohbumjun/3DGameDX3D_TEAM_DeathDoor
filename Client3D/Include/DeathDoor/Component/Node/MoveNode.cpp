
#include "MoveNode.h"
#include "Component/AnimationMeshComponent.h"
#include "../PlayerNormalAttackCheckCollider.h"
#include "Animation/AnimationSequenceInstance.h"
#include "../GameBehaviorTree.h"
#include "../GameStateComponent.h"
#include "../PlayerDataComponent.h"
#include "Input.h"
#include "Scene/SceneManager.h"

CMoveNode::CMoveNode()	:
	m_NavAgent(nullptr)
{
	SetTypeID(typeid(CMoveNode).hash_code());
}

CMoveNode::CMoveNode(const CMoveNode& Node)	:
	CActionNode(Node)
{
}

CMoveNode::~CMoveNode()
{
}

NodeResult CMoveNode::OnStart(float DeltaTime)
{
	m_NavAgent = m_Object->FindObjectComponentFromType<CNavAgent>();
	CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	 m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	std::string ObjectName = m_Object->GetName();

	std::string SequenceName = ObjectName + "Run";
	//std::string SequenceName = "Run";

	if (m_AnimationMeshComp)
	{
		m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation(SequenceName);
	}
	
	Data->GetSword()->GetAnimationInstance()->ChangeAnimation(SequenceName);

	//m_CallStart = true;

	return NodeResult::Node_True;
}

NodeResult CMoveNode::OnUpdate(float DeltaTime)
{
	CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (!Data)
		return NodeResult::Node_False;

	if (!m_NavAgent)
		return NodeResult::Node_False;

	const keyState WState = CInput::GetInst()->FindKeyState('W');
	const keyState AState = CInput::GetInst()->FindKeyState('A');
	const keyState SState = CInput::GetInst()->FindKeyState('S');
	const keyState DState = CInput::GetInst()->FindKeyState('D');

	float Speed = Data->GetMoveSpeed();

	CCameraComponent* CurrentCam = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	Vector3 ZAxis = m_Object->GetWorldAxis(AXIS::AXIS_Z);

	Vector3 FrontVector = Vector3(-ZAxis.x, -ZAxis.y, -ZAxis.z);

	Vector3 MoveDir;
	Matrix matRot = CurrentCam->GetTransform()->GetRotationMatrix();

	if (WState.State[0] || WState.State[1])
	{
		MoveDir += Vector3(0.f, 0.f, 1.f).TransformCoord(matRot);
	}

	if (AState.State[0] || AState.State[1])
	{
		MoveDir += Vector3(-1.f, 0.f, 0.f).TransformCoord(matRot);
	}

	if (SState.State[1])
	{
		MoveDir += Vector3(0.f, 0.f, -1.f).TransformCoord(matRot);
	}

	if (DState.State[1])
	{
		MoveDir += Vector3(1.f, 0.f, 0.f).TransformCoord(matRot);
	}

	MoveDir.y = 0.f;
	MoveDir.Normalize();

	m_Object->SetMoveDir(MoveDir);
	
	Vector3 CrossVector = Vector3(FrontVector.x, FrontVector.y, -FrontVector.z).Cross(Vector3(MoveDir.x, MoveDir.y, -MoveDir.z));

	bool Over180 = false;

	if (CrossVector.y < 0.f)
		Over180 = true;

	float Degree = FrontVector.Dot(MoveDir);
	Degree = RadianToDegree(acosf(Degree));

	if (abs(Degree) > 1.5f)
	{
		// 180도 넘으면 반시계로 회전
		if (Over180)
		{
			if (Degree > 1.5f)
			{
				m_Object->AddWorldRotationY(720.f * DeltaTime);
			}
		}

		else
		{
			if (Degree > 1.5f)
			{
				m_Object->AddWorldRotationY(-720.f * DeltaTime);
			}
		}
	}

	//m_Object->AddWorldPos(MoveDir.x * Speed * DeltaTime, 0.f, MoveDir.z * Speed * DeltaTime);
	m_NavAgent->MoveOnNavMesh(Vector3(MoveDir.x * Speed * DeltaTime, 0.f, MoveDir.z * Speed * DeltaTime));

	return NodeResult::Node_True;
}

NodeResult CMoveNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

void CMoveNode::SetNavAgent(CNavAgent* Agent)
{
	m_NavAgent = Agent;
}

NodeResult CMoveNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}

