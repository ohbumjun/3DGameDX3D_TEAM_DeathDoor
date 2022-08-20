
#include "NavMeshComponent.h"
#include "../Resource/Mesh/NavMesh.h"
#include "../Resource/Mesh/StaticMesh.h"
#include "../Resource/ResourceManager.h"
#include "../Scene/Scene.h"
#include "../Scene/Navigation3DManager.h"
#include "../Scene/SceneResource.h"
#include "../Scene/SceneManager.h"

#include <DirectXCollision.h>

CNavMeshComponent::CNavMeshComponent()	:
	m_DebugRender(false),
	m_PlayerSpawnPolyIndex(0)
{
	SetTypeID<CNavMeshComponent>();
	m_ComponentType = Component_Type::SceneComponent;
	m_Render = true;

	m_LayerName = "Default";

	m_DrawShadow = false;

	m_Transform->AddChangePosCallBack<CNavMeshComponent>(this, &CNavMeshComponent::OnUpdateWorld);
	m_Transform->AddChangeRotCallBack<CNavMeshComponent>(this, &CNavMeshComponent::OnUpdateWorld);
	m_Transform->AddChangeScaleCallBack<CNavMeshComponent>(this, &CNavMeshComponent::OnUpdateWorld);
}

CNavMeshComponent::CNavMeshComponent(const CNavMeshComponent& com)	:
	CSceneComponent(com)
{
	m_NavMesh = com.m_NavMesh;
	m_Shader = com.m_Shader;
}

CNavMeshComponent::~CNavMeshComponent()
{
	auto iter = m_UseCellList.begin();
	auto iterEnd = m_UseCellList.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(*iter);
	}
}

void CNavMeshComponent::SetNavMesh(const std::string& Name)
{
	m_NavMesh = (CNavMesh*)CResourceManager::GetInst()->FindMesh("NavMesh");

	m_Scene->GetNavigation3DManager()->SetNavMeshData(this);

	Vector3 Pos = m_Transform->GetWorldPos();

	// 인자로 넣어주긴 하는데 안씀
	//OnUpdateWorld(Pos, Pos);
}

void CNavMeshComponent::SetNavMesh(CNavMesh* NavMesh)
{
	m_NavMesh = NavMesh;
	Vector3 Pos = m_Transform->GetWorldPos();

	// 인자로 넣어주긴 하는데 안씀
	OnUpdateWorld(Pos, Pos);
}

void CNavMeshComponent::Start()
{
	CSceneComponent::Start();

	if (m_NavMesh)
	{
		CSceneManager::GetInst()->GetScene()->GetCollision()->SetNavMeshMin(m_NavMesh->GetMin());
		CSceneManager::GetInst()->GetScene()->GetCollision()->SetNavMeshMax(m_NavMesh->GetMax());

		size_t Count = m_NavMesh->GetNavMeshPolygonCount();

		for (size_t i = 0; i < Count; ++i)
		{
			NavigationCell* Cell = new NavigationCell;
			NavMeshPolygon Polygon = m_NavMesh->GetNavMeshPolygon(i);
			Vector3 P1 = Polygon.m_vecVertexPos[0];
			Vector3 P2 = Polygon.m_vecVertexPos[1];
			Vector3 P3 = Polygon.m_vecVertexPos[2];
			Cell->Center = (P1 + P2 + P3) / 3.f;
			Cell->Polygon = Polygon;
			m_UseCellList.push_back(Cell);
			m_mapCell.insert(std::make_pair(Polygon.m_Index, Cell));
		}
	}
}

bool CNavMeshComponent::Init()
{
	CSceneComponent::Init();

	m_Shader = m_Scene->GetResource()->FindShader("NavMeshShader");

	SetNavMesh("NavMesh");

	SetDebugRender(true);

    return true;
}

void CNavMeshComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);
}

void CNavMeshComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);
}

void CNavMeshComponent::PrevRender()
{
	CSceneComponent::PrevRender();
}

void CNavMeshComponent::Render()
{
#ifdef _DEBUG
	
	if (m_DebugRender)
	{
		CSceneComponent::Render();

		m_WireFrameState->SetState();

		if (m_Shader)
			m_Shader->SetShader();

		if (m_NavMesh)
			m_NavMesh->Render();

		m_WireFrameState->ResetState();
	}
#endif // _DEBUG
}

void CNavMeshComponent::PostRender()
{
	CSceneComponent::PostRender();
}

CNavMeshComponent* CNavMeshComponent::Clone()
{
	return new CNavMeshComponent(*this);
}

void CNavMeshComponent::GetNavPolgonVertexPos(int Index, std::vector<Vector3>& vecPos)
{
	NavMeshPolygon Polygon = m_NavMesh->GetNavMeshPolygon(Index);

	size_t Count = Polygon.m_vecVertexPos.size();

	for (size_t i = 0; i < Count; ++i)
		vecPos.push_back(Polygon.m_vecVertexPos[i]);
}

void CNavMeshComponent::GetAdjPolyIndex(int Index, std::vector<int>& vecPolyIndex)
{
	NavMeshPolygon Polygon = m_NavMesh->GetNavMeshPolygon(Index);

	size_t Count = Polygon.m_vecAdjIndex.size();

	for (size_t i = 0; i < Count; ++i)
		vecPolyIndex.push_back(Polygon.m_vecAdjIndex[i]);
}

const Vector3& CNavMeshComponent::GetVertexPos(int PolyIndex, int VertexIndex)
{
	return m_NavMesh->GetNavMeshPolygon(PolyIndex).m_vecVertexPos[VertexIndex];
}

void CNavMeshComponent::OnUpdateWorld(const Vector3& World, const Vector3& Relative)
{
	size_t Count = m_NavMesh->GetNavMeshPolygonCount();

	m_Transform->ForceUpdateMat();
	Matrix WorldMat = m_Transform->GetWorldMatrix();

	for (int i = 0; i < Count; ++i)
	{
		Vector3 Pos1 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[0];
		Vector3 Pos2 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[1];
		Vector3 Pos3 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[2];

		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[0] = Pos1.TransformCoord(WorldMat);
		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[1] = Pos2.TransformCoord(WorldMat);
		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[2] = Pos3.TransformCoord(WorldMat);
	}

	Count = m_NavMesh->GetVertexPosCount();

	for (size_t i = 0; i < Count; ++i)
	{
		Vector3 Pos = m_NavMesh->GetVertexOriginPos(i);

		m_NavMesh->GetVertexPos(i) = Pos.TransformCoord(WorldMat);
	}

	Vector3 OriginMin = m_NavMesh->GetOriginMin();
	Vector3 OriginMax = m_NavMesh->GetOriginMax();

	m_NavMesh->m_Min = OriginMin.TransformCoord(WorldMat);
	m_NavMesh->m_Max = OriginMax.TransformCoord(WorldMat);

	if (m_NavMesh->m_Min.x > m_NavMesh->m_Max.x && m_NavMesh->m_Min.z > m_NavMesh->m_Max.z)
	{
		Vector3 Tmp = m_NavMesh->m_Min;
		m_NavMesh->m_Min = m_NavMesh->m_Max;
		m_NavMesh->m_Max = Tmp;
	}
}

//void CNavMeshComponent::OnUpdateRotation(const Vector3& WorldRot, const Vector3& RelativeRot)
//{
//	size_t Count = m_NavMesh->GetNavMeshPolygonCount();
//
//	Matrix Rot;
//	Rot.Rotation(WorldRot);
//
//	for (size_t i = 0; i < Count; ++i)
//	{
//		Vector3 Pos1 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[0];
//		Vector3 Pos2 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[1];
//		Vector3 Pos3 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[2];
//
//		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[0] = Pos1.TransformCoord(Rot);
//		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[1] = Pos2.TransformCoord(Rot);
//		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[2] = Pos3.TransformCoord(Rot);
//	}
//
//	Count = m_NavMesh->GetVertexPosCount();
//
//	for (size_t i = 0; i < Count; ++i)
//	{
//		Vector3 Pos = m_NavMesh->GetVertexOriginPos(i);
//		Vector3 NewPos = Pos.TransformCoord(Rot);
//
//		m_NavMesh->SetVertexPos(i, NewPos);
//	}
//
//	Vector3 OriginMin = m_NavMesh->GetOriginMin();
//	Vector3 OriginMax = m_NavMesh->GetOriginMax();
//
//	OriginMin.TransformCoord(Rot);
//	OriginMax.TransformCoord(Rot);
//
//	m_NavMesh->m_Min = OriginMin;
//	m_NavMesh->m_Max = OriginMax;
//}
//
//void CNavMeshComponent::OnUpdateScale(const Vector3& WorldScale, const Vector3& RelativeScale)
//{
//	size_t Count = m_NavMesh->GetNavMeshPolygonCount();
//
//	Matrix MatScale;
//	MatScale.Scaling(WorldScale);
//
//	for (size_t i = 0; i < Count; ++i)
//	{
//		Vector3 Pos1 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[0];
//		Vector3 Pos2 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[1];
//		Vector3 Pos3 = m_NavMesh->GetNavMeshPolygon(i).m_vecVertexOriginPos[2];
//
//		Vector3 NewPos1 = Pos1.TransformCoord(MatScale);
//		Vector3 NewPos2 = Pos2.TransformCoord(MatScale);
//		Vector3 NewPos3 = Pos3.TransformCoord(MatScale);
//
//		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[0] = NewPos1;
//		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[1] = NewPos2;
//		m_NavMesh->GetNavMeshPolygon(i).m_vecVertexPos[2] = NewPos3;
//
//		if (i == 1536)
//			int a = 3;
//	}
//
//	Count = m_NavMesh->GetVertexPosCount();
//
//	for (size_t i = 0; i < Count; ++i)
//	{
//		Vector3 Pos = m_NavMesh->GetVertexOriginPos(i);
//		Vector3 NewPos = Pos.TransformCoord(MatScale);
//
//		m_NavMesh->SetVertexPos(i, NewPos);
//	}
//
//	Vector3 OriginMin = m_NavMesh->GetOriginMin();
//	Vector3 OriginMax = m_NavMesh->GetOriginMax();
//
//	OriginMin.TransformCoord(MatScale);
//	OriginMax.TransformCoord(MatScale);
//
//	m_NavMesh->m_Min = OriginMin;
//	m_NavMesh->m_Max = OriginMax;
//}


bool CNavMeshComponent::Save(FILE* File)
{
	CSceneComponent::Save(File);

	m_NavMesh->SaveMesh(File);

	return true;
}

bool CNavMeshComponent::Load(FILE* File)
{
	CSceneComponent::Load(File);

	m_NavMesh = new CNavMesh;

	m_NavMesh->LoadMesh(File);

	m_Scene->GetNavigation3DManager()->SetNavMeshData(this);

	return true;
}

bool CNavMeshComponent::SaveOnly(FILE* File)
{
	return false;
}

bool CNavMeshComponent::LoadOnly(FILE* File)
{
	return false;
}

void CNavMeshComponent::FindPath(const Vector3& Start, const Vector3& End, std::list<Vector3>& vecPath)
{
	m_OpenList.clear();
	m_CloseList.clear();

	if (m_UseCellList.size() == 0)
	{
		size_t Count = m_NavMesh->GetNavMeshPolygonCount();

		for (size_t i = 0; i < Count; ++i)
		{
			NavigationCell* Cell = new NavigationCell;
			NavMeshPolygon Polygon = m_NavMesh->GetNavMeshPolygon(i);
			Vector3 P1 = Polygon.m_vecVertexPos[0];
			Vector3 P2 = Polygon.m_vecVertexPos[1];
			Vector3 P3 = Polygon.m_vecVertexPos[2];
			Cell->Center = (P1 + P2 + P3) / 3.f;
			Cell->Polygon = Polygon;
			m_UseCellList.push_back(Cell);
			m_mapCell.insert(std::make_pair(Polygon.m_Index, Cell));
		}
	}

	NavigationCell* StartCell = FindCell(Vector3(Start.x, Start.y + 10.f, Start.z));
	NavigationCell* EndCell = FindCell(Vector3(End.x, End.y + 10.f, End.z));

	if (!StartCell || !EndCell)
		int a = 3;

	if (StartCell == EndCell)
	{
		vecPath.push_back(EndCell->Center);
		return;
	}
	
	AddCellCloseList(StartCell, EndCell);
	AddAdjCellOpenList(StartCell, EndCell, StartCell->Polygon.m_Index);

	while (true)
	{
		if (!m_CloseList.empty())
		{
			auto iter = m_CloseList.end();
			--iter;

			// 닫힌 목록에 목표 Cell이 들어오면 그 Cell들의 부모 Cell들을 타고 올라가면서 경로를 완성하고 길찾기 종료
			if (EndCell == (*iter))
			{
				vecPath.push_back(End);
				//vecPath.push_back((*iter)->Center);
				int ParentIndex = (*iter)->ParentIdx;

				NavigationCell* CurrentCell = (*iter);

				while (true)
				{
					if (ParentIndex == -1)
					{
						ResetAllCell();
						return;
					}

					Vector3 Pos = CurrentCell->Center;

					NavigationCell* ParentCell = FindCell(ParentIndex);

					Vector3 ParentCellPos = ParentCell->Center;

					Vector3 LerpPos = Vector3(0.8f, 0.8f, 0.8f) * ParentCellPos + Vector3(0.2f, 0.2f, 0.2f) * Pos;

					vecPath.push_back(ParentCell->Center);

					ParentIndex = ParentCell->ParentIdx;
					CurrentCell = ParentCell;
				}

				break;
			}
		}

		// Total이 낮은 순으로 정렬
		m_OpenList.sort(SortByTotal);

		auto iter2 = m_OpenList.begin();

		AddCellCloseList(*iter2, EndCell);

		AddAdjCellOpenList(*iter2, EndCell, (*iter2)->Polygon.m_Index);

		DelteCellOpenList(*iter2);
	}
}

NavigationCell* CNavMeshComponent::FindCell(const Vector3& Pos)
{
	auto iter = m_UseCellList.begin();
	auto iterEnd = m_UseCellList.end();

	for ( ; iter != iterEnd; ++iter)
	{
		NavMeshPolygon Poly = (*iter)->Polygon;

		Vector3 P1 = Poly.m_vecVertexPos[0];
		Vector3 P2 = Poly.m_vecVertexPos[1];
		Vector3 P3 = Poly.m_vecVertexPos[2];

		XMVECTOR v1 = Pos.Convert();

		XMVECTOR Dir = Vector3(0.f, -1.f, 0.f).Convert();
		XMVECTOR _P1 = P1.Convert();
		XMVECTOR _P2 = P2.Convert();
		XMVECTOR _P3 = P3.Convert();

		float Dist = 0.f;

		bool Intersect = DirectX::TriangleTests::Intersects(v1, Dir, _P1, _P2, _P3, Dist);

		if (Intersect)
		{
			return (*iter);
		}
	}

	return nullptr;
}

void CNavMeshComponent::AddCellCloseList(NavigationCell* Cell, NavigationCell* End)
{
 	NavigationCell* CheckCell = FindCell(Cell->Polygon.m_Index);

	if (!CheckCell->Enable)
		return;

	if (CheckCell->Type == NCLT_CLOSE)
		return;

	Cell->Type = NCLT_CLOSE;
	
	m_CloseList.push_back(Cell);
}

void CNavMeshComponent::AddAdjCellOpenList(NavigationCell* Cell, NavigationCell* End, int ParentIndex)
{
	size_t Count = Cell->Polygon.m_vecAdjIndex.size();
	
	for (size_t i = 0; i < Count; ++i)
	{
		NavigationCell* AdjCell = FindCell(Cell->Polygon.m_vecAdjIndex[i]);

		if (AdjCell)
		{
			// 인접 Cell이 이미 닫힌 목록에 들어가있다면 패스
			if (AdjCell->Type == NCLT_CLOSE)
				continue;

			if (!AdjCell->Enable)
				continue;

			// 이미 열린 목록에 있는 놈이라면 Total을 비교해서 더 작은 것으로 교체
			if (AdjCell->Type == NCLT_OPEN)
			{
				float NewG = Cell->G + Cell->Center.Distance(AdjCell->Center);
				float NewH = End->Center.Distance(AdjCell->Center);
				float NewTotal = NewG + NewH;

				if (AdjCell->Total > NewTotal)
				{
					AdjCell->G = NewG;
					AdjCell->H = NewH;
					AdjCell->Total = NewTotal;
					AdjCell->ParentIdx = ParentIndex;
				}
			}

			else
			{
				// 비용 계산해서 열린 목록에 넣어주기
				AdjCell->Type = NCLT_OPEN;
				AdjCell->G = Cell->G + Cell->Center.Distance(AdjCell->Center);
				AdjCell->H = AdjCell->Center.Distance(End->Center);
				AdjCell->Total = AdjCell->G + AdjCell->H;
				AdjCell->ParentIdx = ParentIndex;

				m_OpenList.push_back(AdjCell);
			}
		}
	}
}

void CNavMeshComponent::DelteCellOpenList(NavigationCell* Cell)
{
	auto iter = m_OpenList.begin();
	auto iterEnd = m_OpenList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter) == Cell)
		{
			m_OpenList.erase(iter);
			return;
		}
	}
}

void CNavMeshComponent::MakePathList(std::list<Vector3>& PathList)
{

}

bool CNavMeshComponent::CheckStraightPath(const Vector3& StartPos, const Vector3& EndPos, std::vector<Vector3>& vecPath)
{
	///////
	float slope = 0.f;
	int CurrentPosX = (int)StartPos.x;
	int CurrentPosZ = (int)StartPos.z;
	int DestPosX = (int)EndPos.x;
	int DestPosZ = (int)EndPos.z;

	if (abs(StartPos.x - EndPos.x) < 1.f && abs(StartPos.z - EndPos.z) < 1.f)
	{
		vecPath.push_back(EndPos);
		return true;
	}

	int dx = DestPosX - CurrentPosX;
	int dz = DestPosZ - CurrentPosZ;

	vecPath.push_back(Vector3((float)CurrentPosX, EndPos.y, (float)CurrentPosZ));

	int Absdx = (int)abs(dx);
	int Absdz = (int)abs(dz);

	slope = (float)Absdz / Absdx;

	if (slope > 1)
	{
		int pi = 2 * (Absdx - Absdz);

		while (true)
		{
			if (pi >= 0)
			{
				if (dx > 0)
					++CurrentPosX;
				else
					--CurrentPosX;
				if (dz > 0)
					++CurrentPosZ;
				else
					--CurrentPosZ;

				pi = pi + 2 * (Absdx - Absdz);
			}

			else
			{
				if (dz > 0)
					++CurrentPosZ;
				else
					--CurrentPosZ;

				pi = pi + 2 * (Absdx);
			}
			
			vecPath.push_back(Vector3(CurrentPosX, EndPos.y, CurrentPosZ));

			if (CurrentPosX == DestPosX && CurrentPosZ == DestPosZ)
				break;
		}
	}

	else
	{
		int pi = 2 * (Absdz - Absdx);

		while (true)
		{
			if (pi >= 0)
			{
				if (dx > 0)
					++CurrentPosX;
				else
					--CurrentPosX;
				if (dz > 0)
					++CurrentPosZ;
				else
					--CurrentPosZ;

				pi = pi + 2 * (Absdz - Absdx);
			}

			else
			{
				if (dx > 0)
					++CurrentPosX;
				else
					--CurrentPosX;

				pi = pi + 2 * Absdz;
			}

			vecPath.push_back(Vector3((float)CurrentPosX, (float)EndPos.y, (float)CurrentPosZ));

			if (CurrentPosX == DestPosX && CurrentPosZ == DestPosZ)
				break;
		}
	}

	// 브레젠험 알고리즘으로 얻은 (vecPath에 있는) 직선 경로가 NavMesh상에 존재하는 경로인지 체크 

	CNavigation3DManager* Manager = CSceneManager::GetInst()->GetScene()->GetNavigation3DManager();

	size_t Count = vecPath.size();
	float Height = 0.f;
	int PolyIndex = 0;

	for (size_t i = 0; i < Count; ++i)
	{
		bool Valid = Manager->CheckNavMeshPoly(vecPath[i], Height, PolyIndex);

		if (!Valid)
			return false;
	}

	return true;
}

void CNavMeshComponent::ResetAllCell()
{
	m_OpenList.clear();
	m_CloseList.clear();

	auto iter = m_UseCellList.begin();
	auto iterEnd = m_UseCellList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->G = 0;
		(*iter)->H = 0;
		(*iter)->Total = 0;
		(*iter)->Type = NCLT_NONE;
		(*iter)->ParentIdx = -1;
		(*iter)->Enable = true;
	}

	auto iter2 = m_mapCell.begin();
	auto iterEnd2 = m_mapCell.end();

	for (; iter2 != iterEnd2; ++iter2)
	{
		iter2->second->G = 0;
		iter2->second->H = 0;
		iter2->second->Total = 0;
		iter2->second->Type = NCLT_NONE;
		iter2->second->ParentIdx = -1;
		iter2->second->Enable = true;
	}
}



NavigationCell* CNavMeshComponent::FindCell(int PolyIndex)
{
	auto iter = m_mapCell.find(PolyIndex);

	if (iter == m_mapCell.end())
		return nullptr;

	return iter->second;
}

void CNavMeshComponent::FindAdjCell(int PolyIndex, std::vector<NavigationCell*>& vecCell)
{
	auto iter = m_mapCell.find(PolyIndex);

	if (iter == m_mapCell.end())
		return;

	size_t Count = iter->second->Polygon.m_vecAdjIndex.size();

	for (size_t i = 0; i < Count; ++i)
	{
		NavigationCell* AdjCell = m_mapCell.find(iter->second->Polygon.m_vecAdjIndex[i])->second;
		vecCell.push_back(AdjCell);

		int a = 3;
	}
}
