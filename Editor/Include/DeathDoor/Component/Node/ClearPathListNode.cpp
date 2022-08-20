
#include "ClearPathListNode.h"
#include "../MonsterDataComponent.h"
#include "../GameStateComponent.h"
#include "../MonsterNavAgent.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"

CClearPathListNode::CClearPathListNode()
{
	SetTypeID(typeid(CClearPathListNode).hash_code());
}

CClearPathListNode::CClearPathListNode(const CClearPathListNode& Node) :
	CActionNode(Node)
{
}

CClearPathListNode::~CClearPathListNode()
{
}

NodeResult CClearPathListNode::OnStart(float DeltaTime)
{
	CMonsterDataComponent* Data = dynamic_cast<CMonsterDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	CMonsterNavAgent* Agent = Data->GetMonsterNavAgent();

	if (Agent)
	{
		Vector3 TargetPos;

		if (Agent->IsEmptyPathList())
		{
			TargetPos = m_Object->GetScene()->GetPlayerObject()->GetWorldPos();
		}
		else
		{
			TargetPos = Agent->GetPathList().front();
		}

		Vector3 CurrentPos = m_Object->GetRootComponent()->GetWorldPos();

		Vector3 CurrentDir = Agent->GetCurrentFaceDir();
		CurrentDir.y = 0.f;
		CurrentDir.Normalize();
		Vector3 TargetDir = TargetPos - CurrentPos;
		TargetDir.y = 0.f;
		TargetDir.Normalize();
		float DotProduct = CurrentDir.Dot(TargetDir);

		if (DotProduct >= -0.95f && DotProduct <= 0.95f)
		{
			float Degree = RadianToDegree(CurrentDir.Dot(TargetDir));

			if (CurrentDir.Cross(TargetDir).y > 0.f)
			{
				m_Object->AddWorldRotationY(Degree);
			}

			else
			{
				m_Object->AddWorldRotationY(-Degree);
			}

			Vector3 Rot = m_Object->GetWorldRot();
			Matrix RotMat;
			RotMat.Rotation(Rot);
			Vector3 Dir = Vector3(0.f, 0.f, -1.f);

			Dir = Dir.TransformCoord(RotMat);

			Agent->SetCurrentFaceDir(Dir);
		}


		else
			int a = 3;

		Agent->ClearPathList();
		Agent->SetCoolStart(true);
	}

	return NodeResult::Node_True;
}

NodeResult CClearPathListNode::OnUpdate(float DeltaTime)
{


	return NodeResult::Node_True;
}

NodeResult CClearPathListNode::OnEnd(float DeltaTime)
{
	return NodeResult::Node_True;
}

NodeResult CClearPathListNode::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}

