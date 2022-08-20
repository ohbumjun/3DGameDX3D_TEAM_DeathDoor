
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

		// AnimationSequenceInstance���� m_ChangeTime�� �ֱ� ������, �ٷ� �ִϸ��̼��� �ٲ�°� �ƴ϶� m_ChangeTime�� ���ļ� �ִϸ��̼��� �ٲ�µ�
		// CurrentAnimation�� NormalAttack�̶� ChangeAnimation�ص� �ƹ��͵� ���ϰ� �ٷ� return�ǰ� ChangeAnimation�� Idle���� �������� �����Ǿ� �ִ� ���¿���
		// �� ���¿��� OnUpdate�� ���� Idle�� ������ Sequence�� �ٲ� ���¿��� NormalAttack::OnUpdate���� CurrentAnimation�� NormalAttack�̶�� ����������
		// CurrentAnimation�� �ᱹ Idle�� �� ���̰�, ChangeAnimation�� nullptr�� �� ���̹Ƿ� OnUpdate�� else���� ��� ������ �Ǵ� ������ ����Ƿ�
		// �̷� ��Ȳ������ KeepAnimation�� ȣ���ؼ� ChangeAnimation�� ����� CurrentAnimation�� Attack���� ���������ش�
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

		// �̹� NormalAttackNode�� ���°��� �ѹ� MouseLButton �Է��� ���� ���̰�, LButtonCheckNode���� Queue�� Front element�� �ϳ� Pop�ߴµ�
		// Queue�� LButton Input�� �ϳ� �� ���Դٸ� -> �� + ��
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

				// ���� 3Ÿ�߿� ������ 3Ÿ�� �� �̻� ������ ��Ÿ�� ���ϰ� 3Ÿ ����� ���������� ����� �� �ְ� Queue���� �� Key�� ����ؼ� �ٽ� �־
				// ������ 3Ÿ�� ���������� ��ٸ� �� �ְ� �Ѵ�
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

		// TODO : else if�� �� + �� ó�� �߰��� �� �ִ� Ű ���� �߰��� �� ���� �߰�

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
