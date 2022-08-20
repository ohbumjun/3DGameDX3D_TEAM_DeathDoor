
#include "CollisionSection.h"
#include "../Component/ColliderComponent.h"
#include "../Input.h"
#include "../Component/NavAgent.h"
#include "../GameObject/GameObject.h"

CCollisionSection::CCollisionSection()
{
}

CCollisionSection::~CCollisionSection()
{
}

void CCollisionSection::Init(int IndexX, int IndexY, int IndexZ, int Index,
	const Vector3& Min, const Vector3& Max, 
	const Vector3& SectionSize, const Vector3& SectionTotalSize)
{
	m_IndexX = IndexX;
	m_IndexY = IndexY;
	m_IndexZ = IndexZ;
	m_Index = Index;
	m_Min = Min;
	m_Max = Max;
	m_SectionSize = SectionSize;
	m_SectionTotalSize = SectionTotalSize;

	m_vecCollider.reserve(100);
}

void CCollisionSection::Clear()
{
	m_vecCollider.clear();
}

void CCollisionSection::DeleteCollider(CColliderComponent* Collider)
{
	size_t Count = m_vecCollider.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecCollider[i] == Collider)
		{
			auto iter = m_vecCollider.begin();
			std::advance(iter, i);

			m_vecCollider.erase(iter);
			return;
		}
	}
}

void CCollisionSection::AddCollider(CColliderComponent* Collider)
{
	m_vecCollider.push_back(Collider);

	Collider->AddSectionIndex(m_Index);
}

void CCollisionSection::Collision(float DeltaTime)
{
	size_t	Count = m_vecCollider.size();

	if (Count < 2)
		return;

	for (size_t i = 0; i < Count - 1; ++i)
	{
		CColliderComponent* Src = m_vecCollider[i];

		for (size_t j = i + 1; j < Count; ++j)
		{
			CColliderComponent* Dest = m_vecCollider[j];

			if (Src->CheckCurrentFrameCollision(Dest))
				continue;

			CollisionProfile* SrcProfile = Src->GetCollisionProfile();
			CollisionProfile* DestProfile = Dest->GetCollisionProfile();

			if (SrcProfile->vecInteraction[(int)DestProfile->Channel] ==
				Collision_Interaction::Ignore &&
				DestProfile->vecInteraction[(int)SrcProfile->Channel] ==
				Collision_Interaction::Ignore)
				continue;

			if (Src->Collision(Dest))
			{
				// Src, Dest의 m_Result에 서로를 설정해주기
				Src->SetCollisionResultSrc(Src);
				Src->SetCollisionResultDest(Dest);

				Dest->SetCollisionResultSrc(Dest);
				Dest->SetCollisionResultDest(Src);
				// 지금 충돌이 된건지를 판단한다.
				// 즉, 이전 프레임에 충돌된 목록에 없다면 지금 막 충돌이 시작된 것이다.
				if (!Src->CheckPrevCollision(Dest))
				{
					Src->AddPrevCollision(Dest);
					Dest->AddPrevCollision(Src);

					Src->CallCollisionCallback(Collision_State::Begin);
					Dest->CallCollisionCallback(Collision_State::Begin);
				}

				// 이전 프레임부터 계속 충돌중인 경우
				else
				{
					Src->CallCollisionCallback(Collision_State::Stay);
					Dest->CallCollisionCallback(Collision_State::Stay);

					if (SrcProfile->vecInteraction[(int)DestProfile->Channel] ==
						Collision_Interaction::CollisionRigid &&
						DestProfile->vecInteraction[(int)SrcProfile->Channel] ==
						Collision_Interaction::CollisionRigid)
					{
						if(!Src->GetRigidCollisionIgnore() && !Dest->GetRigidCollisionIgnore())
							CollisionRigid(Src, Dest, DeltaTime);
					}

				}

				Src->AddCurrentFrameCollision(Dest);
				Dest->AddCurrentFrameCollision(Src);

				Src->SetCollisionResultSrc(nullptr);
				Src->SetCollisionResultDest(nullptr);

				Dest->SetCollisionResultSrc(nullptr);
				Dest->SetCollisionResultDest(nullptr);
			}

			// 이전 프레임에 충돌이 되었는데 현재프레임에 충돌이 안되는 상황이라면
			// 충돌되었다가 이제 떨어지는 의미이다.
			else if (Src->CheckPrevCollision(Dest))
			{
				// Src, Dest의 m_Result에 서로를 설정해주기
				Src->SetCollisionResultSrc(Src);
				Src->SetCollisionResultDest(Dest);

				Dest->SetCollisionResultSrc(Dest);
				Dest->SetCollisionResultDest(Src);

				Src->DeletePrevCollision(Dest);
				Dest->DeletePrevCollision(Src);

				Src->CallCollisionCallback(Collision_State::End);
				Dest->CallCollisionCallback(Collision_State::End);

				Src->SetCollisionResultSrc(nullptr);
				Src->SetCollisionResultDest(nullptr);

				Dest->SetCollisionResultSrc(nullptr);
				Dest->SetCollisionResultDest(nullptr);
			}
		}
	}
}

CColliderComponent* CCollisionSection::CollisionMouse(bool Is2D, float DeltaTime)
{
	if (Is2D)
	{
		Vector2	MousePos = CInput::GetInst()->GetMouseWorld2DPos();

		size_t	Size = m_vecCollider.size();

		if (Size > 1)
			qsort(&m_vecCollider[0], Size, (size_t)sizeof(CColliderComponent*), CCollisionSection::SortY);

		for (size_t i = 0; i < Size; ++i)
		{
			if (m_vecCollider[i]->CollisionMouse(MousePos))
				return m_vecCollider[i];
		}
	}

	else
	{
	}

	return nullptr;
}

void CCollisionSection::CollisionRigid(CColliderComponent* Src, CColliderComponent* Dest, float DeltaTime)
{
	CGameObject* SrcObject = Src->GetGameObject();
	CGameObject* DestObject = Dest->GetGameObject();

	if (Src->GetGameObject()->GetObjectType() == Object_Type::Player)
	{
		Vector3 SrcPrevPos = SrcObject->GetPrevFramePos();
		Vector3 DestPrevPos = DestObject->GetPrevFramePos();

		CNavAgent* Agent = SrcObject->FindObjectComponentFromType<CNavAgent>();
		Vector3 FaceDir = Agent->GetCurrentFaceDir();
		Vector3 PlayerCurrentPos = SrcObject->GetWorldPos();
		Vector3 PlayerNextFramePos = PlayerCurrentPos + FaceDir;
		Vector3 DestObjectPos = DestObject->GetWorldPos();

		float CurrentDist = PlayerCurrentPos.Distance(DestObjectPos);
		float NextFrameDist = PlayerNextFramePos.Distance(DestObjectPos);

		if (CurrentDist > NextFrameDist)
		{
			DestObject->SetWorldPos(DestPrevPos);
			SrcObject->SetWorldPos(SrcPrevPos);
			return;
		}

	}

	else if (Dest->GetGameObject()->GetObjectType() == Object_Type::Player)
	{
		Vector3 SrcPrevPos = SrcObject->GetPrevFramePos();
		Vector3 DestPrevPos = DestObject->GetPrevFramePos();

		CNavAgent* Agent = DestObject->FindObjectComponentFromType<CNavAgent>();
		Vector3 FaceDir = Agent->GetCurrentFaceDir();
		Vector3 PlayerCurrentPos = DestObject->GetWorldPos();
		Vector3 PlayerNextFramePos = PlayerCurrentPos + FaceDir;
		Vector3 SrcObjectPos = SrcObject->GetWorldPos();

		float CurrentDist = PlayerCurrentPos.Distance(SrcObjectPos);
		float NextFrameDist = PlayerNextFramePos.Distance(SrcObjectPos);

		if (CurrentDist > NextFrameDist)
		{
			DestObject->SetWorldPos(DestPrevPos);
			SrcObject->SetWorldPos(SrcPrevPos);
			return;
		}
	}

	else
	{
		// TODO : NavAgent를 갖고 있는 플레이어와, MonsterNavAgent를 갖고 있는 몬스터는 모두 Component이름을 Nav로 해줘야한다

			CNavAgent* SrcObjNavAgent = (CNavAgent*)SrcObject->FindObjectComponent("Nav");
			CNavAgent* DestObjNavAgent = (CNavAgent*)DestObject->FindObjectComponent("Nav");
			Vector3 SrcObjPos = SrcObject->GetRootComponent()->GetWorldPos();
			Vector3 DestObjPos = DestObject->GetRootComponent()->GetWorldPos();

			if (!SrcObjNavAgent || !DestObjNavAgent)
				return;

			if (!SrcObjNavAgent->IsEmptyPathList() && !DestObjNavAgent->IsEmptyPathList())
			{
				Vector3 DestObjecTargetPos = DestObjNavAgent->GetTargetPos();

				DestObjNavAgent->DeleteTargetPos();
				Vector3 Dir = DestObjPos - SrcObjPos;
				Dir.y = 0.f;
				Dir.Normalize();
				DestObjNavAgent->AddPath(DestObjPos + Vector3(Dir.x / 2.f, 0.f, Dir.z / 2.f));
				SrcObjNavAgent->AddPath(SrcObject->GetPrevFramePos());
				return;
			}

			// 앞에 있는 몬스터가 플레이어와 충돌해서 멈춰있고, 내가 아직 길찾기가 남은 경우 나의 방향을 조금씩 회전해준다
			else if(!SrcObjNavAgent->IsEmptyPathList())
			{
				std::list<Vector3> PathList = SrcObjNavAgent->GetPathList();
				Vector3 FaceDir1 = SrcObjNavAgent->GetCurrentFaceDir();

				Vector3 DestPos = PathList.back();

				PathList.pop_back();

				Matrix RotMat1;
				RotMat1.Rotation(0.f, 10.f, 0.f);

				FaceDir1 = FaceDir1.TransformCoord(RotMat1);

				Vector3 NewPos1 = SrcObjPos + Vector3(FaceDir1.x * 2.f, 0.f, FaceDir1.z * 2.f);

				SrcObjNavAgent->AddPath(NewPos1);
			}

			else if (!DestObjNavAgent->IsEmptyPathList())
			{
				std::list<Vector3> PathList = DestObjNavAgent->GetPathList();
				Vector3 FaceDir1 = DestObjNavAgent->GetCurrentFaceDir();

				Vector3 DestPos = PathList.back();

				PathList.pop_back();

				Matrix RotMat1;
				RotMat1.Rotation(0.f, 10.f, 0.f);

				FaceDir1 = FaceDir1.TransformCoord(RotMat1);

				Vector3 NewPos1 = DestObjPos + Vector3(FaceDir1.x * 2.f, 0.f, FaceDir1.z * 2.f);

				DestObjNavAgent->AddPath(NewPos1);
			}

	}
}

CColliderComponent* CCollisionSection::GetCollider(int Idx) const
{
	return m_vecCollider[Idx];
}

//CColliderComponent* CCollisionSection::GetPrevCollider(int Idx) const
//{
//	return m_vecPrevCollider[Idx];
//}

int CCollisionSection::SortY(const void* Src, const void* Dest)
{
	CColliderComponent* SrcCollider = *((CColliderComponent**)Src);
	CColliderComponent* DestCollider = *((CColliderComponent**)Dest);

	if (SrcCollider->GetMin().y < DestCollider->GetMin().y)
		return -1;

	else if (SrcCollider->GetMin().y > DestCollider->GetMin().y)
		return 1;

	return 0;
}
