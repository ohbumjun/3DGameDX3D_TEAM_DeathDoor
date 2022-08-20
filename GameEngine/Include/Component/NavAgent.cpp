
#include "NavAgent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Navigation3DManager.h"
#include "SceneComponent.h"
#include "../GameObject/GameObject.h"
#include "NavMeshComponent.h"

CNavAgent::CNavAgent()	:
	m_MoveSpeed(0.f),
	m_RotationSpeed(360.f),
	m_ApplyNavMesh(true),
	m_Rotaiting(false)
{
	m_CurrentFaceDir = Vector3(0.f, 0.f, 1.f);
	SetTypeID<CNavAgent>();
}

CNavAgent::CNavAgent(const CNavAgent& com)	:
	CObjectComponent(com)
{
	m_MoveSpeed = com.m_MoveSpeed;
}

CNavAgent::~CNavAgent()
{
}

void CNavAgent::SetUpdateComponent(CSceneComponent* UpdateComponent)
{
	m_UpdateComponent = UpdateComponent;

	Vector3 Rot = m_UpdateComponent->GetWorldRot();

	Matrix RotMat;
	RotMat.Rotation(Rot);

	m_CurrentFaceDir = m_CurrentFaceDir.TransformCoord(RotMat);
	m_CurrentFaceDir.Normalize();
}

bool CNavAgent::Move(const Vector3& EndPos)
{
	if (!m_UpdateComponent)
		return false;

	return m_Scene->GetNavigationManager()->FindPath<CNavAgent, CSceneComponent>(this, &CNavAgent::PathResult, 
		m_UpdateComponent, EndPos);
}

bool CNavAgent::MoveOnNavMesh(const Vector3 EndPos)
{
	// NavMesh없으면 자유롭게 모든 곳 이동 가능하게 하기
	if (!m_Scene->GetNavigation3DManager()->GetNavMeshData())
	{
		m_Object->AddWorldPos(EndPos);
		return true;
	}

	m_Object->AddWorldPos(EndPos);

	float Height = 0.f;
	bool Valid = m_Scene->GetNavigation3DManager()->CheckPlayerNavMeshPoly(Height);

	if (Valid)
	{
		Vector3 Pos = m_Object->GetWorldPos();
		m_Object->SetWorldPos(Pos.x, Height + 0.3f, Pos.z);
		return true;
	}

	else
	{
		m_Object->SetWorldPos(m_Object->GetPrevFramePos());
		return false;
	}
}

bool CNavAgent::MoveOnNavMesh(const Vector3 Dir, float Speed)
{
	if (!m_Scene->GetNavigation3DManager()->GetNavMeshData())
		return false;

	m_Object->AddWorldPos(Dir * Speed);

	float Height = 0.f;
	bool Valid = m_Scene->GetNavigation3DManager()->CheckPlayerNavMeshPoly(Height);

	if (Valid)
	{
		Vector3 Pos = m_Object->GetWorldPos();
		m_Object->SetWorldPos(Pos.x, Height + 0.2f, Pos.z);
		return true;
	}

	else
	{
		m_Object->SetWorldPos(m_Object->GetPrevFramePos());
		return false;
	}
}

const Vector3& CNavAgent::GetTargetPos() const
{
	if (!m_PathList.empty())
		return m_PathList.back();

}

void CNavAgent::Start()
{
	if (!m_UpdateComponent)
		m_UpdateComponent = m_Object->GetRootComponent();
}

bool CNavAgent::Init()
{
	return true;
}

void CNavAgent::Update(float DeltaTime)
{
	if (m_UpdateComponent)
	{
		// NavAgent를 가지고 있는 모든 움직이는 오브젝트가 초기에 바라보는 방향은 -z 방향이라고 가정
		Vector3 CurrentFaceDir = Vector3(0.f, 0.f, -1.f);
		Vector3 Rot = m_UpdateComponent->GetWorldRot();

		Matrix RotMat;

		RotMat.Rotation(Rot);

		CurrentFaceDir = CurrentFaceDir.TransformCoord(RotMat);
		m_CurrentFaceDir = CurrentFaceDir;

		if (!m_PathList.empty())
		{
			Vector3	TargetPos = m_PathList.back();
			Vector3	Pos = m_UpdateComponent->GetWorldPos();

			float	TargetDistance = Pos.Distance(TargetPos);

			Vector3	Dir = TargetPos - Pos;
			Dir.Normalize();

			m_UpdateComponent->AddWorldPos(Dir * m_MoveSpeed * DeltaTime);

			if (TargetDistance <= 0.5f)
			{
				m_PathList.pop_back();
			}

			float Dot = Vector3(Dir.x, 0.f, Dir.z).Dot(Vector3(CurrentFaceDir.x, 0.f, CurrentFaceDir.z));
			float Angle = Vector3(Dir.x, 0.f, Dir.z).Angle(Vector3(CurrentFaceDir.x, 0.f, CurrentFaceDir.z));

			if (abs(Angle) > 1.f)
			{
				float Degree = RadianToDegree(acosf(Dot));
				Vector3 CrossResult = Vector3(Dir.x, 0.f, Dir.z).Cross(Vector3(CurrentFaceDir.x, 0.f, CurrentFaceDir.z));

				if (CrossResult.y > 0.f)
				{
					m_UpdateComponent->AddWorldRotationY(-m_RotationSpeed * DeltaTime);
				}

				else
				{
					m_UpdateComponent->AddWorldRotationY(m_RotationSpeed * DeltaTime);
				}

				m_Rotaiting = true;
			}
			else
			{
				if (!isnan(Angle))
				{
					m_UpdateComponent->AddWorldRotationY(Angle);
				}
				m_Rotaiting = false;
			}
		}
	}

	// NavAgent가 있는데 ApplyNavMesh가 false -> NavMesh를 적용 받지 않아서 아래로 떨어져야 하는 순간
	if (!m_ApplyNavMesh)
	{
		m_Object->AddWorldPos(0.f, -12.f * DeltaTime, 0.f);
	}
}

void CNavAgent::PostUpdate(float DeltaTime)
{
}

void CNavAgent::PrevRender()
{
}

void CNavAgent::Render()
{
}

void CNavAgent::PostRender()
{
}

CNavAgent* CNavAgent::Clone()
{
	return new CNavAgent(*this);
}

bool CNavAgent::Save(FILE* File)
{
	CComponent::Save(File);

	fwrite(&m_MoveSpeed, sizeof(float), 1, File);

	return true;
}

bool CNavAgent::Load(FILE* File)
{
	CComponent::Load(File);

	fread(&m_MoveSpeed, sizeof(float), 1, File);

	return true;
}

bool CNavAgent::SaveOnly(FILE* File)
{
	CComponent::SaveOnly(File);

	fwrite(&m_MoveSpeed, sizeof(float), 1, File);

	return true;
}

bool CNavAgent::LoadOnly(FILE* File)
{
	CComponent::LoadOnly(File);

	fread(&m_MoveSpeed, sizeof(float), 1, File);

	return true;
}

void CNavAgent::PathResult(const std::list<Vector3>& PathList)
{
	m_PathList.clear();
	m_PathList = PathList;
}

void CNavAgent::FillPathList(const std::list<Vector3>& PathList)
{
	auto iter = PathList.begin();
	auto iterEnd = PathList.end();

	for (; iter != iterEnd; ++iter)
	{
		m_PathList.push_back(*iter);
	}
}

void CNavAgent::AddPath(const Vector3& EndPos)
{
	m_PathList.push_back(EndPos);
}

bool CNavAgent::FindPath(CSceneComponent* OwnerComponent, const Vector3& End)
{
	bool Result = CSceneManager::GetInst()->GetScene()->GetNavigation3DManager()->FindPath<CNavAgent, CSceneComponent>(this, &CNavAgent::FillPathList, OwnerComponent, End);
	return Result;
}

bool CNavAgent::FindPathExcept(CSceneComponent* OwnerComponent, const Vector3& End, std::vector<Vector3>& vecExceptPos)
{
	bool Result = CSceneManager::GetInst()->GetScene()->GetNavigation3DManager()->FindPathExcept<CNavAgent, CSceneComponent>(this, &CNavAgent::FillPathList, OwnerComponent, End, vecExceptPos);
	return Result;
}

bool CNavAgent::FindPathExcept(CSceneComponent* OwnerComponent, const Vector3& End, std::vector<NavigationCell*>& vecExceptCell)
{
	bool Result = CSceneManager::GetInst()->GetScene()->GetNavigation3DManager()->FindPathExcept<CNavAgent, CSceneComponent>(this, &CNavAgent::FillPathList, OwnerComponent, End, vecExceptCell);
	return Result;
}

bool CNavAgent::CheckStraightPath(const Vector3& StartPos, const Vector3& EndPos, std::vector<Vector3>& vecPath)
{
	return CSceneManager::GetInst()->GetScene()->GetNavigation3DManager()->GetNavMeshData()->CheckStraightPath(StartPos, EndPos, vecPath);
}

void CNavAgent::ForceUpdateFaceDir()
{
	Vector3 CurrentFaceDir = Vector3(0.f, 0.f, -1.f);
	Vector3 Rot = m_UpdateComponent->GetWorldRot();

	Matrix RotMat;

	RotMat.Rotation(Rot);

	CurrentFaceDir = CurrentFaceDir.TransformCoord(RotMat);
	m_CurrentFaceDir = CurrentFaceDir;
}
