
#include "NormalAttack.h"
#include "Component/AnimationMeshComponent.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Component/BehaviorTree.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../PlayerDataComponent.h"
#include "../GameStateComponent.h"

CNormalAttack::CNormalAttack()	:
	m_NavAgent(nullptr),
	m_AccDistance(0.f),
	m_ConsecutiveAttackCount(0)
{
	SetTypeID(typeid(CNormalAttack).hash_code());
}

CNormalAttack::CNormalAttack(const CNormalAttack& Node)	:
	CActionNode(Node)
{
}

CNormalAttack::~CNormalAttack()
{
}

NodeResult CNormalAttack::OnStart(float DeltaTime)
{
	m_AnimationMeshComp = m_Owner->GetAnimationMeshComp();

	if (m_ConsecutiveAttackCount > 3)
	{
		m_Owner->SetCurrentNode(nullptr);
		m_CallStart = false;
		return NodeResult::Node_True;
	}

	std::string ObjectName = m_Object->GetName();

	std::string CurrentSequence = m_AnimationMeshComp->GetAnimationInstance()->GetCurrentAnimation()->GetName();
	std::string SequenceName;

	if (m_Object->GetObjectType() == Object_Type::Player)
	{
		CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());
		

		if (CurrentSequence.find("SlashL") != std::string::npos)
			SequenceName = ObjectName + "SlashR";

		else
			SequenceName = ObjectName + "SlashL";

		CAnimationSequenceInstance* Instance = m_AnimationMeshComp->GetAnimationInstance();

		bool IsPlaying = Instance->CheckCurrentAnimation(SequenceName);

		if (!IsPlaying)
		{
			switch (Data->GetConsecutiveAttackCount())
			{
			case 0:
			case 1:
				m_Object->GetScene()->GetResource()->SoundPlay("SwordSwing1");
				break;
			case 2:
				m_Object->GetScene()->GetResource()->SoundPlay("SwordSwing2");
				break;
			case 3:
				m_Object->GetScene()->GetResource()->SoundPlay("SwordSwing3");
				break;
			}
		}

		if(Data->GetSword())
			Data->GetSword()->GetAnimationInstance()->ChangeAnimation(SequenceName);
		
	}

	

	m_AnimationMeshComp->GetAnimationInstance()->ChangeAnimation(SequenceName);

	if (m_AnimationMeshComp)
	{
		CAnimationSequenceInstance* Instance = m_AnimationMeshComp->GetAnimationInstance();

		// AnimationSequenceInstance에서 m_ChangeTime이 있기 때문에, 바로 애니메이션이 바뀌는게 아니라 m_ChangeTime에 걸쳐서 애니메이션이 바뀌는데
		// CurrentAnimation은 NormalAttack이라서 ChangeAnimation해도 아무것도 안하고 바로 return되고 ChangeAnimation은 Idle같은 시퀀스로 설정되어 있는 상태에서
		// 이 상태에서 OnUpdate로 들어가고 Idle로 완전히 Sequence가 바뀐 상태에서 NormalAttack::OnUpdate에서 CurrentAnimation은 NormalAttack이라고 생각하지만
		// CurrentAnimation은 결국 Idle로 될 것이고, ChangeAnimation은 nullptr가 될 것이므로 OnUpdate의 else에만 계속 들어오게 되는 문제가 생기므로
		// 이런 상황에서는 KeepAnimation을 호출해서 ChangeAnimation을 지우고 CurrentAnimation은 Attack으로 유지시켜준다
		if (Instance->GetChangeAnimation() && Instance->GetCurrentAnimation()->GetName() == SequenceName)
			Instance->KeepCurrentAnimation();

		else
			Instance->ChangeAnimation(SequenceName);
	}

	m_CallStart = true;

	if (!m_NavAgent)
	{
		m_NavAgent = m_Object->FindObjectComponentFromType<CNavAgent>();
	}
	
	return NodeResult::Node_True;
}

NodeResult CNormalAttack::OnUpdate(float DeltaTime)
{
	CPlayerDataComponent* Data = dynamic_cast<CPlayerDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	int ConsecutiveCount = Data->GetConsecutiveAttackCount();

	if (m_ConsecutiveAttackCount > 3)
	{
		m_ConsecutiveAttackCount = 0;
		Data->SetConsecutiveAttackCount(0);
		m_IsEnd = true;
		return NodeResult::Node_False;
	}

	CAnimationSequenceInstance* Instance = m_AnimationMeshComp->GetAnimationInstance();

	if (!Data->IsKeyStateQueueEmpty())
	{
		char Key = Data->GetFrontKeyState();
		Data->PopKeyState();

		// 이미 NormalAttackNode로 들어온것은 한번 MouseLButton 입력이 들어온 것이고, LButtonCheckNode에서 Queue에 Front element를 하나 Pop했는데
		// Queue에 LButton Input이 하나 더 들어왔다면 -> 평 + 평
		if (Key == VK_LBUTTON && Instance->GetCurrentAnimation()->GetAnimationSequence()->GetCurrentFrameIdx() >= 2)
		{
			int Tmp = m_ConsecutiveAttackCount;

			if (Tmp == 0)
			{
				m_ConsecutiveAttackCount = 2;
				Data->SetConsecutiveAttackCount(2);
				Data->ForceUpdateAttackDirection();
			}

			else if (Tmp == 2)
			{
				m_ConsecutiveAttackCount = 3;
				Data->SetConsecutiveAttackCount(3);
				Data->ForceUpdateAttackDirection();
			}

			else if (Tmp == 3)
			{
				if (!Instance->IsCurrentAnimLoop() && Instance->IsCurrentAnimEnd())
				{
					m_ConsecutiveAttackCount = 0;
					Data->SetConsecutiveAttackCount(0);

					m_IsEnd = true;

					return NodeResult::Node_False;
				}

				// 연속 3타중에 마지막 3타는 더 이상 빠르게 연타를 못하고 3타 모션이 끝날때까지 대기할 수 있게 Queue에서 뺀 Key를 계속해서 다시 넣어서
				// 마지막 3타가 끝날때까지 기다릴 수 있게 한다
				else
				{
					Data->PushKeyState(Key);
					return NodeResult::Node_Running;
				}
			}

			m_Owner->SetCurrentNode(this);
			m_AccDistance = 0.f;
			m_CallStart = false;

			return NodeResult::Node_Running;
		}

		// TODO : else if로 평 + 평 처럼 추가될 수 있는 키 조합 추가될 때 마다 추가

		else if(Key == VK_SPACE)
		{


			return NodeResult::Node_Running;
		}
	}

	else if (!Instance->IsCurrentAnimLoop() && Instance->IsCurrentAnimEnd())
	{
		m_ConsecutiveAttackCount = 0;
		Data->SetConsecutiveAttackCount(0);

		m_IsEnd = true;

		return NodeResult::Node_False;
	}

	else
	{
		m_Owner->SetCurrentNode(this);

		Vector3 AttackDir = Data->GetAttackDir();
		Vector3 ZAxis = m_Object->GetWorldAxis(AXIS_Z);
		ZAxis *= -1.f;
		Vector3 ForwardDir = ZAxis / 10.f;

		if (m_AccDistance < 2.f)
			m_NavAgent->MoveOnNavMesh(ForwardDir);

		float Dist = ForwardDir.Length();
		m_AccDistance += Dist;

		return NodeResult::Node_Running;
	}

}

NodeResult CNormalAttack::OnEnd(float DeltaTime)
{
	m_AccDistance = 0.f;
	m_CallStart = false;
	m_IsEnd = false;
	m_Owner->SetCurrentNode(nullptr);

	return NodeResult::Node_True;
}

NodeResult CNormalAttack::Invoke(float DeltaTime)
{
	return CActionNode::Invoke(DeltaTime);
}
