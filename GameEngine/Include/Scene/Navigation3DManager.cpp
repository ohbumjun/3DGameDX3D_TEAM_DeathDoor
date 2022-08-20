
#include "Navigation3DManager.h"
#include "../Component/LandScape.h"
#include "../Component/CameraComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Input.h"
#include "../Component/NavMeshComponent.h"
#include "../Resource/Mesh/NavMesh.h"
#include "NavigationThread3D.h"

//#include "DirectXMath.h"
#include <DirectXCollision.h>

CNavigation3DManager::CNavigation3DManager()	:
	m_LandScape(nullptr),
	m_NavMeshComponent(nullptr),
	m_PlayerPolyIndex(-1)
{
}

CNavigation3DManager::~CNavigation3DManager()
{
	size_t	Size = m_vecNavigationThread.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecNavigationThread[i]);
	}

	m_vecNavigationThread.clear();
}

CNavMeshComponent* CNavigation3DManager::GetNavMeshData() const
{
	return m_NavMeshComponent;
}

void CNavigation3DManager::SetNavMeshData(CNavMeshComponent* NavComp)
{
	m_NavMeshComponent = NavComp;
}


void CNavigation3DManager::SetNavData(CLandScape* NavData)
{
	m_NavData = NavData;
}

void CNavigation3DManager::AddNavResult(const NavResultData& NavData)
{
	m_ResultQueue.push(NavData);
}

void CNavigation3DManager::SetLandScape(CLandScape* LandScape)
{
	m_LandScape = LandScape;
}

CLandScape* CNavigation3DManager::GetLandScape() const
{
	return m_LandScape;
}

float CNavigation3DManager::GetY(const Vector3& Pos)
{
	if (!m_NavData)
		return Pos.y;

	return m_NavData->GetHeight(Pos);
}

bool CNavigation3DManager::CheckPickingPoint(Vector3& OutPos)
{
	if (!m_LandScape)
		return false;

	CCameraComponent* Current = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	Matrix ProjMat = Current->GetProjMatrix();
	Matrix ViewMat = Current->GetViewMatrix();

	Ray ray = CInput::GetInst()->GetRay(ViewMat);
	Vector3 RayDir = ray.Dir;
	Vector3 RayStartPos = ray.Pos;

	float Step = 0.1f;

	while (true)
	{
		Vector3 Point_I1 = RayStartPos;
		Vector3 Point_I2 = RayStartPos + RayDir * Step;

		float Height1 = m_LandScape->GetHeight(Point_I1);
		float Height2 = m_LandScape->GetHeight(Point_I2);

		Vector3 ResultPos;

		if (m_LandScape->CheckInArea(Point_I1, Point_I2, ResultPos))
		{
			OutPos = ResultPos;
			break;
		}

		// LandScape 밖에 클릭하면 피킹 지점 찾지 않도록 예외 처리
		if (Point_I1.x < m_LandScape->GetMin().x && ray.Dir.x <= 0.f)
			return false;

		if (Point_I1.z < m_LandScape->GetMin().z && ray.Dir.z <= 0.f)
			return false;

		if (Point_I1.x > m_LandScape->GetMax().x && ray.Dir.x >= 0.f)
			return false;

		if (Point_I1.z > m_LandScape->GetMax().z && ray.Dir.z >= 0.f)
			return false;

		else
		{
			RayStartPos = Point_I2;
		}
	}

	return true;
}

bool CNavigation3DManager::CheckPlayerNavMeshPoly(float& Height)
{
	if (!m_NavMeshComponent)
		return false;

	CGameObject* Player = m_Scene->GetPlayerObject();

	if (!Player)
		return false;

	// 플레이어가 어떤 polygon에 위치할지 초기 설정이 되어있지 않아서 모든 polygon을 돌면서 확인
	if (m_PlayerPolyIndex == -1)
	{
		Vector3 PlayerPos = Player->GetWorldPos();

		size_t Count = m_NavMeshComponent->GetNavMesh()->GetNavMeshPolygonCount();

		for (size_t i = 0; i < Count; ++i)
		{
			//Vector3 P1 = m_NavMeshComponent->GetVertexPos(i, 0);
			//Vector3 P2 = m_NavMeshComponent->GetVertexPos(i, 1);
			//Vector3 P3 = m_NavMeshComponent->GetVertexPos(i, 2);
			std::vector<Vector3> vecPos;
			m_NavMeshComponent->GetNavPolgonVertexPos((int)i, vecPos);
			Vector3 P1 = vecPos[0];
			Vector3 P2 = vecPos[1];
			Vector3 P3 = vecPos[2];

			//Matrix WorldMat = m_NavMeshComponent->GetWorldMatrix();

			//P1 = P1.TransformCoord(WorldMat);
			//P2 = P2.TransformCoord(WorldMat);
			//P3 = P3.TransformCoord(WorldMat);

			//PlayerPos.y += 10.f;
			XMVECTOR v1 = PlayerPos.Convert();

			XMVECTOR Dir = Vector3(0.f, -1.f, 0.f).Convert();
			XMVECTOR _P1 = P1.Convert();
			XMVECTOR _P2 = P2.Convert();
			XMVECTOR _P3 = P3.Convert();

			float Dist = 0.f;

			bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

			Vector3 CandidatePoly = (P1 + P2 + P3) / 3.f;
			float PolyHeight = CandidatePoly.y;

			// 갑자기 아래로 꺼진다면 아래 층에 네비메쉬 폴리곤을 Intersect Check한 가능성이 높다
			if (PolyHeight + 6.f < PlayerPos.y)
			{
				Intersect = false;
			}

			if (Intersect)
			{
				m_PlayerPolyIndex = (int)i;
				
				float Dist1 = P1.Distance(PlayerPos);
				float Dist2 = P2.Distance(PlayerPos);
				float Dist3 = P3.Distance(PlayerPos);

				Vector3 LerpVec = Vector3(1/Dist1, 1/Dist2, 1/Dist3);
				LerpVec.Normalize();

				// Weighted Average
				Height = LerpVec.x * LerpVec.x * P1.y + LerpVec.y * LerpVec.y * P2.y + LerpVec.z * LerpVec.z * P3.y + 0.1f;

				return true;
			}

		}

		//Vector3 P1 = m_NavMeshComponent->GetVertexPos(0, 0);
		//Vector3 P2 = m_NavMeshComponent->GetVertexPos(0, 1);
		//Vector3 P3 = m_NavMeshComponent->GetVertexPos(0, 2);

		//Matrix WorldMat = m_NavMeshComponent->GetWorldMatrix();

		//P1 = P1.TransformCoord(WorldMat);
		//P2 = P2.TransformCoord(WorldMat);
		//P3 = P3.TransformCoord(WorldMat);

		//Player->SetWorldPos(Vector3((P1.x + P2.x + P3.x) / 3.f, (P1.y + P2.y + P3.y) / 3.f, (P1.z + P2.z + P3.z) / 3.f));
		//m_PlayerPolyIndex = 0;
		return false;
	}

	// 플레이어가 어떤 polygon에 위치할지 이미 설정이 되어있는 상태라서 지금 Polygon에 존재하는지 체크,
	// 지금 Polygon에 존재하지 않는다면 다른 polygon으로 건너갔다고 판단
	else
	{
		std::vector<Vector3> vecPos;
		m_NavMeshComponent->GetNavPolgonVertexPos(m_PlayerPolyIndex, vecPos);

		Matrix WorldMat = m_NavMeshComponent->GetWorldMatrix();

		Vector3 P1 = vecPos[0];
		Vector3 P2 = vecPos[1];
		Vector3 P3 = vecPos[2];

		//P1 = P1.TransformCoord(WorldMat);
		//P2 = P2.TransformCoord(WorldMat);
		//P3 = P3.TransformCoord(WorldMat);

		CGameObject* Player = m_Scene->GetPlayerObject();
		Vector3 PlayerPos = Player->GetWorldPos();

		XMVECTOR v1 = PlayerPos.Convert();

		XMVECTOR Dir = Vector3(0.f, -1.f, 0.f).Convert();
		XMVECTOR _P1 = P1.Convert();
		XMVECTOR _P2 = P2.Convert();
		XMVECTOR _P3 = P3.Convert();

		float Dist = 0.f;

		bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

		if (Intersect)
		{
			float Dist1 = P1.Distance(PlayerPos);
			float Dist2 = P2.Distance(PlayerPos);
			float Dist3 = P3.Distance(PlayerPos);

			Vector3 LerpVec = Vector3(1 / Dist1, 1 / Dist2, 1 / Dist3);
			LerpVec.Normalize();

			// Weighted Average
			Height = LerpVec.x * LerpVec.x * P1.y + LerpVec.y * LerpVec.y * P2.y + LerpVec.z * LerpVec.z * P3.y + 0.1f;

			return true;
		}

		// 인접 Polygon을 돌아서 어떤 polygon으로 옮겨갔는지 확인한다.
		// 모든 인접 폴리곤을 돌았는데도 교차하는 polygon이 없다면 이동 불가능한 곳으로 이동하려는 것이다
		else
		{
			std::vector<int> vecAdjPolyIndex;
			m_NavMeshComponent->GetAdjPolyIndex(m_PlayerPolyIndex, vecAdjPolyIndex);

			size_t Count = vecAdjPolyIndex.size();

			for (size_t i = 0; i < Count; ++i)
			{
				std::vector<Vector3> vecPos;
				int AdjPolyIndex = vecAdjPolyIndex[i];
				m_NavMeshComponent->GetNavPolgonVertexPos(AdjPolyIndex, vecPos);

				P1 = vecPos[0];
				P2 = vecPos[1];
				P3 = vecPos[2];

				//P1 = P1.TransformCoord(WorldMat);
				//P2 = P2.TransformCoord(WorldMat);
				//P3 = P3.TransformCoord(WorldMat);

				XMVECTOR _P1 = P1.Convert();
				XMVECTOR _P2 = P2.Convert();
				XMVECTOR _P3 = P3.Convert();

				float Dist = 0.f;

				bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

				if (Intersect)
				{
					float Dist1 = P1.Distance(PlayerPos);
					float Dist2 = P2.Distance(PlayerPos);
					float Dist3 = P3.Distance(PlayerPos);

					Vector3 LerpVec = Vector3(1 / Dist1, 1 / Dist2, 1 / Dist3);
					LerpVec.Normalize();

					// Weighted Average
					Height = LerpVec.x * LerpVec.x * P1.y + LerpVec.y * LerpVec.y * P2.y + LerpVec.z * LerpVec.z * P3.y + 0.1f;

					m_PlayerPolyIndex = AdjPolyIndex;
					return true;
				}
			}

			return false;
		}
	}

	return false;
}

bool CNavigation3DManager::CheckNavMeshPoly(const Vector3& Pos, float& Height, int& PolyIndex)
{
	if (!m_NavMeshComponent)
		return false;

	size_t Count = m_NavMeshComponent->GetNavMesh()->GetNavMeshPolygonCount();

	for (size_t i = 0; i < Count; ++i)
	{
		std::vector<Vector3> vecPos;
		m_NavMeshComponent->GetNavPolgonVertexPos((int)i, vecPos);

		//Vector3 P1 = m_NavMeshComponent->GetVertexPos(i, 0);
		//Vector3 P2 = m_NavMeshComponent->GetVertexPos(i, 1);
		//Vector3 P3 = m_NavMeshComponent->GetVertexPos(i, 2);

		Vector3 P1 = vecPos[0];
		Vector3 P2 = vecPos[1];
		Vector3 P3 = vecPos[2];

		XMVECTOR v1 = Pos.Convert();

		XMVECTOR Dir = Vector3(0.f, -1.f, 0.f).Convert();
		XMVECTOR _P1 = P1.Convert();
		XMVECTOR _P2 = P2.Convert();
		XMVECTOR _P3 = P3.Convert();

		float Dist = 0.f;

		bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

		Vector3 CandidatePoly = (P1 + P2 + P3) / 3.f;
		float PolyHeight = CandidatePoly.y;

		// 갑자기 아래로 꺼진다면 아래 층에 네비메쉬 폴리곤을 Intersect Check한 가능성이 높다
		if (PolyHeight + 6.f < Pos.y)
		{
			Intersect = false;
		}

		if (Intersect)
		{
			float Dist1 = P1.Distance(Pos);
			float Dist2 = P2.Distance(Pos);
			float Dist3 = P3.Distance(Pos);

			Vector3 LerpVec = Vector3(1 / Dist1, 1 / Dist2, 1 / Dist3);
			LerpVec.Normalize();

			// Weighted Average
			Height = LerpVec.x * LerpVec.x * P1.y + LerpVec.y * LerpVec.y * P2.y + LerpVec.z * LerpVec.z * P3.y + 0.15f;

			PolyIndex = (int)i;

			return true;
		}
	}

	return false;
}

bool CNavigation3DManager::RefreshPlayerNavMeshPoly(const Vector3& Pos)
{
	size_t Count = m_NavMeshComponent->GetNavMesh()->GetNavMeshPolygonCount();

	for (size_t i = 0; i < Count; ++i)
	{
		std::vector<Vector3> vecPos;
		m_NavMeshComponent->GetNavPolgonVertexPos((int)i, vecPos);
		Vector3 P1 = vecPos[0];
		Vector3 P2 = vecPos[1];
		Vector3 P3 = vecPos[2];


		XMVECTOR v1 = Pos.Convert();

		XMVECTOR Dir = Vector3(0.f, -1.f, 0.f).Convert();
		XMVECTOR _P1 = P1.Convert();
		XMVECTOR _P2 = P2.Convert();
		XMVECTOR _P3 = P3.Convert();

		float Dist = 0.f;

		bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

		Vector3 CandidatePoly = (P1 + P2 + P3) / 3.f;
		float PolyHeight = CandidatePoly.y;

		// 갑자기 아래로 꺼진다면 아래 층에 네비메쉬 폴리곤을 Intersect Check한 가능성이 높다
		if (PolyHeight + 6.f < Pos.y)
		{
			Intersect = false;
		}

		if (Intersect)
		{
			m_PlayerPolyIndex = (int)i;

			return true;
		}

	}

	return false;
}

bool CNavigation3DManager::CheckCurrentNavMeshPoly(const Vector3& Pos, int CurrentPolyIndex, float& Height, int& OutPolyIndex)
{
	if (!m_NavMeshComponent)
		return false;

	NavMeshPolygon Polygon = m_NavMeshComponent->GetNavMesh()->GetNavMeshPolygon(CurrentPolyIndex);

	Vector3 P1 = Polygon.m_vecVertexPos[0];
	Vector3 P2 = Polygon.m_vecVertexPos[1];
	Vector3 P3 = Polygon.m_vecVertexPos[2];

	XMVECTOR v1 = Pos.Convert();

	XMVECTOR Dir = Vector3(0.f, -1.f, 0.f).Convert();
	XMVECTOR _P1 = P1.Convert();
	XMVECTOR _P2 = P2.Convert();
	XMVECTOR _P3 = P3.Convert();

	float Dist = 0.f;

	bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

	if (Intersect)
	{
		float Dist1 = P1.Distance(Pos);
		float Dist2 = P2.Distance(Pos);
		float Dist3 = P3.Distance(Pos);

		Vector3 LerpVec = Vector3(1 / Dist1, 1 / Dist2, 1 / Dist3);
		LerpVec.Normalize();

		// Weighted Average
		Height = LerpVec.x * LerpVec.x * P1.y + LerpVec.y * LerpVec.y * P2.y + LerpVec.z * LerpVec.z * P3.y + 0.1f;

		OutPolyIndex = CurrentPolyIndex;

		return true;
	}

	else
	{
		bool Intersect = CheckAdjNavMeshPoly(Pos, CurrentPolyIndex, Height, OutPolyIndex);

		return Intersect;
	}

	return false;
}

bool CNavigation3DManager::CheckAdjNavMeshPoly(const Vector3& Pos, int CurrentPolyIndex, float& Height, int& PolyIndex)
{
	std::vector<int> vecAdjPolyIndex;

	if(CurrentPolyIndex != -1)
		m_NavMeshComponent->GetAdjPolyIndex(CurrentPolyIndex, vecAdjPolyIndex);

	size_t Count = vecAdjPolyIndex.size();

	for (size_t i = 0; i < Count; ++i)
	{
		std::vector<Vector3> vecPos;
		int AdjPolyIndex = vecAdjPolyIndex[i];
		m_NavMeshComponent->GetNavPolgonVertexPos(AdjPolyIndex, vecPos);

		Vector3 P1 = vecPos[0];
		Vector3 P2 = vecPos[1];
		Vector3 P3 = vecPos[2];

		XMVECTOR _P1 = P1.Convert();
		XMVECTOR _P2 = P2.Convert();
		XMVECTOR _P3 = P3.Convert();
		XMVECTOR Dir = Vector3(0.f, -1.f, 0.f).Convert();
		XMVECTOR v1 = Pos.Convert();

		float Dist = 0.f;

		bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

		if (Intersect)
		{
			float Dist1 = P1.Distance(Pos);
			float Dist2 = P2.Distance(Pos);
			float Dist3 = P3.Distance(Pos);

			Vector3 LerpVec = Vector3(1 / Dist1, 1 / Dist2, 1 / Dist3);
			LerpVec.Normalize();

			// Weighted Average
			Height = LerpVec.x * LerpVec.x * P1.y + LerpVec.y * LerpVec.y * P2.y + LerpVec.z * LerpVec.z * P3.y + 0.1f;
			PolyIndex = AdjPolyIndex;

			return true;
		}
	}

	return false;
}

bool CNavigation3DManager::CheckNavMeshPickingPoint(Vector3& OutPos)
{
	if (!m_NavMeshComponent)
		return false;

	CCameraComponent* Current = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	Matrix ViewMat = Current->GetViewMatrix();

	Ray ray = CInput::GetInst()->GetRay(ViewMat);
	Vector3 RayDir = ray.Dir;
	Vector3 RayStartPos = ray.Pos;

	if (isnan(RayDir.x) || isnan(RayDir.y) || isnan(RayDir.z))
		return false;

	XMVECTOR _RayDir = RayDir.Convert();
	XMVECTOR _RayStartPos = RayStartPos.Convert();
	size_t Count = m_NavMeshComponent->GetNavMesh()->GetNavMeshPolygonCount();

	for (size_t i = 0; i < Count; ++i)
	{
		NavMeshPolygon Poly = m_NavMeshComponent->GetNavMesh()->GetNavMeshPolygon(i);

		Vector3 P1 = Poly.m_vecVertexPos[0];
		Vector3 P2 = Poly.m_vecVertexPos[1];
		Vector3 P3 = Poly.m_vecVertexPos[2];

		XMVECTOR _P1 = P1.Convert();
		XMVECTOR _P2 = P2.Convert();
		XMVECTOR _P3 = P3.Convert();

		if (P1 == P2 || P2 == P3 || P1 == P3)
			continue;

		float Dist1 = P1.Distance(P2);
		float Dist2 = P2.Distance(P3);
		float Dist3 = P3.Distance(P1);

		float Dist = 0.f;

		bool Intersect = DirectX::TriangleTests::Intersects(_RayStartPos, _RayDir, _P1, _P2, _P3, Dist);

		if (Intersect)
		{
			Vector3 Edge1 = P2 - P1;
			Vector3 Edge2 = P3 - P1;

			Vector3 Normal = Edge1.Cross(Edge2);
			Normal.Normalize();

			// Normal.X(x - p1.x) + Normal.Y(y - p1.y) + Normal.Z(z - p1.z) = 0
			// 평면의 방정식이 ax + by + cz + d 일때, a는 Normal.x, b는 Normal.y, c는 Normal.z
			// d는 -Normal.X * p1.x - Normal.Y * p1.y - Normal.Z * p1.z
			float Coff_a = Normal.x;
			float Coff_b = Normal.y;
			float Coff_c = Normal.z;
			float Coff_d = -Normal.x * P1.x - Normal.y * P1.y - Normal.z * P1.z;

			XMVECTOR Plane = { Coff_a, Coff_b, Coff_c, Coff_d };
			XMVECTOR Result = XMPlaneIntersectLine(Plane, RayStartPos.Convert(), (RayStartPos + RayDir * 1000.f).Convert());

			OutPos.x = Result.m128_f32[0];
			OutPos.y = Result.m128_f32[1];
			OutPos.z = Result.m128_f32[2];
			//OutPos = (P1 + P2 + P3) / 3.f;
			return true;
		}
	}

	return false;
}

bool CNavigation3DManager::CheckStraightPath(const Vector3& StartPos, const Vector3& EndPos, std::vector<Vector3>& vecPath)
{
	return m_NavMeshComponent->CheckStraightPath(StartPos, EndPos, vecPath);
}

NavigationCell* CNavigation3DManager::FindCell(int PolyIndex)
{
	return m_NavMeshComponent->FindCell(PolyIndex);
}

void CNavigation3DManager::FindAdjCell(int PolyIndex, std::vector<NavigationCell*>& vecCell)
{
	m_NavMeshComponent->FindAdjCell(PolyIndex, vecCell);
}

void CNavigation3DManager::Start()
{
	for (int i = 0; i < 4; ++i)
	{
		char	Name[256] = {};
		sprintf_s(Name, "NavThread%d", i);
		CNavigationThread3D* Thread = CThread::CreateThread<CNavigationThread3D>(Name);

		Thread->SetNavigationManager(this);

		if(m_NavMeshComponent)
			Thread->SetNavMeshComponent(m_NavMeshComponent->Clone());

		Thread->Start();

		m_vecNavigationThread.push_back(Thread);
	}
}

bool CNavigation3DManager::Init()
{
	return true;
}

void CNavigation3DManager::Update(float DeltaTime)
{
	if (!m_ResultQueue.empty())
	{
		NavResultData	Result = m_ResultQueue.front();
		m_ResultQueue.pop();

		Result.Callback(Result.vecPath);
	}
}