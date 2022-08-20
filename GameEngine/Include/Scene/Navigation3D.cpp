
#include "Navigation3D.h"
#include "../Component/NavMeshComponent.h"

CNavigation3D::CNavigation3D()
{
	m_ArriveDist = 0.f;
}

CNavigation3D::~CNavigation3D()
{
}

//
//bool CNavigation3D::RayIntersectTriangle(Ray& tRay)
//{
//	CNavigationMesh* pNavMesh = m_vecNavMesh[0];
//
//	if (!pNavMesh)
//		return false;
//
//	bool	bRet = pNavMesh->RayIntersectTriangle(tRay);
//
//	return bRet;
//}

bool CNavigation3D::FindPath(const Vector3& Start,
	const Vector3& End, std::list<Vector3>& vecPath)
{
	if (!m_NavComponent)
		return false;

	m_NavComponent->FindPath(Start, End, vecPath);

	//m_ArriveDist = pNavMesh->GetArriveDist();

	return true;
}
