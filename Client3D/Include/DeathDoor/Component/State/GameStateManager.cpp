
#include "GameStateManager.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/CollisionSection.h"
#include "../GameStateComponent.h"
#include "DamageState.h"
#include "AirBorneState.h"

CGameStateManager::CGameStateManager()
{
}

CGameStateManager::~CGameStateManager()
{
}

void CGameStateManager::Init()
{
	CAirBorneState* Airborne = new CAirBorneState;

	AddState("Airborne", Airborne);

	CDamageState* DamageState = new CDamageState;

	AddState("Damage", DamageState);
}

//void CGameStateManager::CheckAirborneTarget(const NotifyParameter& Param)
//{
//	// Param.MyPos를 기준으로 검사할 Collision Section을 찾고, 그 Section내에서 Param.Range내에 있는 오브젝트의 StateList에 Airborne State를 넣어준다
//	CColliderBox3D* ColliderBox = Param.SrcObject->FindComponentFromType<CColliderBox3D>();
//
//	CColliderSphere* ColliderSphere = nullptr;
//
//	bool ColliderBoxExist = false;
//	bool ColliderSphereExist = false;
//
//	if (ColliderBox)
//		ColliderBoxExist = true;
//
//	else
//	{
//		ColliderSphere = Param.SrcObject->FindComponentFromType<CColliderSphere>();
//		if (ColliderSphere)
//			ColliderSphereExist = true;
//	}
//
//	// 2가지의 충돌체 모두 갖고 있지 않는 object면 에어본 처리 X
//	if (!ColliderBoxExist && !ColliderSphereExist)
//		return;
//
//	CColliderComponent* Collider = nullptr;
//	if (ColliderBoxExist)
//		Collider = ColliderBox;
//	else
//		Collider = ColliderSphere;
//
//	Vector3 ColliderPos = Collider->GetWorldPos();
//
//	CSceneCollision* Collision = CSceneManager::GetInst()->GetScene()->GetCollision();
//
//	int MinIndexX = Collision->GetSectionIndexX(Vector3(ColliderPos.x - Param.Range, ColliderPos.y, ColliderPos.z));
//	int MinIndexZ = Collision->GetSectionIndexZ(Vector3(ColliderPos.x, ColliderPos.y, ColliderPos.z - Param.Range));
//	int MaxIndexX = Collision->GetSectionIndexX(Vector3(ColliderPos.x + Param.Range, ColliderPos.y, ColliderPos.z));
//	int MaxIndexZ = Collision->GetSectionIndexZ(Vector3(ColliderPos.x, ColliderPos.y, ColliderPos.z + Param.Range));
//
//	int SectionCountX = Collision->GetSectionCountX();
//
//	std::vector<int> vecTargetSection;
//
//	for (size_t i = MinIndexZ; i <= MaxIndexZ; ++i)
//	{
//		for (size_t j = MinIndexX; j <= MaxIndexZ; ++j)
//		{
//			vecTargetSection.push_back((int)(i * SectionCountX + j));
//		}
//	}

	//size_t Count = vecTargetSection.size();

	//for (size_t i = 0; i < Count; ++i)
	//{
	//	// PrevSectionCollider로 받아와야한다(지금은 CScene::Collsion 호출 전이라 각 Section으로 Collider가 들어가있지 않은 상황이라 직전 프레임 기준 Section별
	//	// Collider가 들어있는 PrevSectionCollider로 받아와야 한다
	//	CCollisionSection* Section = Collision->GetCollisionSectionInfo()->vecSection[vecTargetSection[i]];
	//	size_t PrevSectionSize = Section->GetPrevColliderSize();

	//	for (size_t j = 0; j < PrevSectionSize; ++j)
	//	{
	//		CColliderComponent* PrevSectionCollider = Section->GetPrevCollider(j);
	//		if (PrevSectionCollider == Collider)
	//			continue;

	//		Vector3 CandidatePos = PrevSectionCollider->GetWorldPos();
	//		CGameObject* CandidateObject = PrevSectionCollider->GetGameObject();
	//		if (CandidatePos.Distance(Collider->GetWorldPos()) <= Param.Range)
	//		{
	//			CGameStateComponent* StateComp = CandidateObject->FindComponentFromType<CGameStateComponent>();

	//			if (StateComp)
	//			{
	//				StateComp->AddState("Airborne", Param.SrcObject);
	//				CState* DamageState = StateComp->AddState("Damage", Param.SrcObject);

	//				((CDamageState*)DamageState)->SetDamage(1234); // TODO : 추후 csv로 스킬 데미지 정확하게 읽어와서 바꿔주기
	//				//CandidateObject->SetOntHit(true);
	//			}
	//		}
	//	}
	//}

	// 충돌체가 속한 Section들
	//std::vector<int> vecColliderSection = Collider->GetPrevSections();
	//size_t Count = vecColliderSection.size();

	//std::vector<CGameObject*>	vecAirborneTarget;
	//CSceneCollision* Collision = CSceneManager::GetInst()->GetScene()->GetCollision();

	//for (size_t i = 0; i < Count; ++i)
	//{
	//	CCollisionSection* Section = Collision->GetCollisionSectionInfo()->vecSection[vecColliderSection[i]];

	//	size_t Size = Section->GetPrevColliderSize();

	//	for (size_t i = 0; i < Size; ++i)
	//	{
	//		if (Collider == Section->GetPrevCollider(i))
	//			continue;

	//		Vector3 CandidatePos = Section->GetPrevCollider(i)->GetWorldPos();
	//		CGameObject* CandidateObject = Section->GetPrevCollider(i)->GetGameObject();

	//		// 에어본 적용 대상이다
	//		if (abs(CandidatePos.Distance(Collider->GetWorldPos())) < Param.Range)
	//		{
	//			CStateComponent* StateComp = CandidateObject->FindComponentFromType<CStateComponent>();

	//			if (StateComp)
	//				StateComp->AddState("Airborne");
	//		}
	//	}
	//}


//}
//
//void CGameStateManager::GiveDamage(const NotifyParameter& Param)
//{
//	CGameObject* TargetObject = Param.TargetObject;
//
//	CGameStateComponent* StateComp = TargetObject->FindComponentFromType<CGameStateComponent>();
//
//	if (StateComp)
//	{
//		CState* DamageState = StateComp->AddState("Damage", Param.SrcObject);
//		((CDamageState*)DamageState)->SetDamage(Param.Amount); // Amount는 GameDataManager::SetMonsterNotify에서 미니언 공격력 읽어와서 설정해줌
//	}
//}
//
//void CGameStateManager::FindRestraintTarget(const NotifyParameter& Param)
//{
//}
//
//void CGameStateManager::FindStunTarget(const NotifyParameter& Param)
//{
//}
//
//void CGameStateManager::FindKnockBackTarget(const NotifyParameter& Param)
//{
//}