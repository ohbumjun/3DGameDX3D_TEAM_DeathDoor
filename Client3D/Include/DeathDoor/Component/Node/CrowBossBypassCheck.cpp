
#include "CrowBossBypassCheck.h"
#include "../GameStateComponent.h"
#include "../GameBehaviorTree.h"
#include "../CrowBossDataComponent.h"
#include "Scene/Scene.h"
#include "../MonsterNavAgent.h"

CCrowBossBypassCheck::CCrowBossBypassCheck()
{
	SetTypeID(typeid(CCrowBossBypassCheck).hash_code());
}

CCrowBossBypassCheck::CCrowBossBypassCheck(const CCrowBossBypassCheck& Node)	:
	CConditionNode(Node)
{
}

CCrowBossBypassCheck::~CCrowBossBypassCheck()
{
}

// ByPassCheck의 Parent가 SetCurrentNode로 지정된 상태
NodeResult CCrowBossBypassCheck::OnStart(float DeltaTime)
{
	CCrowBossDataComponent* Data = dynamic_cast<CCrowBossDataComponent*>(dynamic_cast<CGameStateComponent*>(m_Owner->GetOwner())->GetData());

	if (Data->IsAfterShoot())
	{
		Data->SetAfterShoot(false);
		return NodeResult::Node_True;
	}


	Vector3 PlayerOriginPos = Data->GetPlayerOriginPos();
	Vector3 PlayerCurrentPos = m_Object->GetScene()->GetPlayerObject()->GetWorldPos();
	Vector3 MyOriginPos = Data->GetMyOriginPos();
	Vector3 MyCurrentPos = m_Object->GetWorldPos();

	Vector3 Dir = PlayerOriginPos - MyOriginPos;
	Dir.Normalize();

	Vector3 FaceDir = Data->GetMonsterNavAgent()->GetCurrentFaceDir();
	//float Speed = Data->GetMoveSpeed();

	//m_Object->AddWorldPos(FaceDir * 4.f * DeltaTime);

	Vector3 OriginDiff = PlayerOriginPos - MyOriginPos;
	Vector3 CurrentDiff = PlayerCurrentPos - MyCurrentPos;

	if (MyOriginPos.x < PlayerOriginPos.x)
	{
		if (MyCurrentPos.x > PlayerCurrentPos.x + 4.f)
			return NodeResult::Node_True;
	}

	if (MyOriginPos.x > PlayerOriginPos.x)
	{
		if (MyCurrentPos.x < PlayerCurrentPos.x - 4.f)
			return NodeResult::Node_True;
	}

	if (MyOriginPos.z < PlayerOriginPos.z)
	{
		if (MyCurrentPos.z > PlayerCurrentPos.z + 4.f)
			return NodeResult::Node_True;
	}

	if (MyOriginPos.z > PlayerOriginPos.z)
	{
		if (MyCurrentPos.z < PlayerCurrentPos.z - 4.f)
			return NodeResult::Node_True;
	}

	/*if (OriginDiff.x < 0.f && OriginDiff.z < 0.f)
	{
		if (CurrentDiff.x > 4.f && CurrentDiff.z > 4.f)
			return NodeResult::Node_True;
	}

	else if (OriginDiff.x < 0.f && OriginDiff.z > 0.f)
	{
		if (CurrentDiff.x > 4.f && CurrentDiff.z < -4.f)
			return NodeResult::Node_True;
	}


	else if (OriginDiff.x > 0.f && OriginDiff.z > 0.f)
	{
		if (CurrentDiff.x < -4.f && CurrentDiff.z < -4.f)
			return NodeResult::Node_True;
	}

	else if (OriginDiff.x > 0.f && OriginDiff.z < 0.f)
	{
		if (CurrentDiff.x < -4.f && CurrentDiff.z > 4.f)
			return NodeResult::Node_True;
	}*/


	

	return NodeResult::Node_False;
}

NodeResult CCrowBossBypassCheck::OnUpdate(float DeltaTime)
{
	return NodeResult();
}

NodeResult CCrowBossBypassCheck::OnEnd(float DeltaTime)
{
	return NodeResult();
}
