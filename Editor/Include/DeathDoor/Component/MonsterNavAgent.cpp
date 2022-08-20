
#include "MonsterNavAgent.h"
#include "GameObject/GameObject.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Navigation3DManager.h"
#include "Component/NavMeshComponent.h"

CMonsterNavAgent::CMonsterNavAgent()	:
	m_AccTime(0.f),
	m_PathFindCoolTime(1.2f),
	m_PathFindCoolStart(false),
	m_PathFindEnable(true),
	m_CoolStart(false),
	m_CurPolyIndex(-1)
{
	SetTypeID<CMonsterNavAgent>();

	m_ComponentType = Component_Type::ObjectComponent;
}

CMonsterNavAgent::CMonsterNavAgent(const CMonsterNavAgent& com)	:
	CNavAgent(com)
{
}

CMonsterNavAgent::~CMonsterNavAgent()
{
}

bool CMonsterNavAgent::IsInNavMesh()
{
	CNavigation3DManager* Nav = CSceneManager::GetInst()->GetScene()->GetNavigation3DManager();

	Vector3 MyPos = m_Object->GetWorldPos();

	bool Found = false;

	if (m_CurPolyIndex == -1)
	{
		Found = Nav->CheckNavMeshPoly(MyPos, MyPos.y, m_CurPolyIndex);
	}
	else
	{
		int NewPolyIndex = 0;
		Found = Nav->CheckCurrentNavMeshPoly(MyPos, m_CurPolyIndex, MyPos.y, NewPolyIndex);

		if (Found)
		{
			m_CurPolyIndex = NewPolyIndex;
		}
	}

	return Found;
}

void CMonsterNavAgent::Start()
{
	CNavAgent::Start();
}

bool CMonsterNavAgent::Init()
{
	return true;
}

void CMonsterNavAgent::Update(float DeltaTime)
{
	bool PrevEmptyPahtList = m_PathList.empty();

	CNavAgent::Update(DeltaTime);

	bool AfterEmptyPathList = m_PathList.empty();

	// NavAgent에서 Path가 하나 남아있었고 그게 방금 없어진거라면 방금 길찾기 최종 목적지로 도착한 것
	if (!PrevEmptyPahtList && AfterEmptyPathList)
	{
		m_CoolStart = true;
	}

	if(m_CoolStart)
	{
		m_PathFindEnable = false;

		m_AccTime += DeltaTime;

		if (m_AccTime >= m_PathFindCoolTime)
		{
			m_AccTime = 0.f;
			m_PathFindEnable = true;
			m_CoolStart = false;
		}
	}
}

void CMonsterNavAgent::PostUpdate(float DeltaTime)
{
	CNavAgent::PostUpdate(DeltaTime);
}

void CMonsterNavAgent::PrevRender()
{
}

void CMonsterNavAgent::Render()
{
}

void CMonsterNavAgent::PostRender()
{
}

CMonsterNavAgent* CMonsterNavAgent::Clone()
{
	return new CMonsterNavAgent(*this);
}

bool CMonsterNavAgent::Save(FILE* File)
{
	return CNavAgent::Save(File);
}

bool CMonsterNavAgent::Load(FILE* File)
{
	return CNavAgent::Load(File);
}

bool CMonsterNavAgent::SaveOnly(FILE* File)
{
	return CNavAgent::SaveOnly(File);
}

bool CMonsterNavAgent::LoadOnly(FILE* File)
{
	return CNavAgent::LoadOnly(File);
}
