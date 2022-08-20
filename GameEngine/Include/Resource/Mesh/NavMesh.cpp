
#include "NavMesh.h"
#include "MeshManager.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneResource.h"
#include "../../Scene/SceneManager.h"

#include <fstream>
#include <sstream>

CNavMesh::CNavMesh() :
	m_PolyCount(0),
	m_Min(1000000.f, 1000000.f, 1000000.f),
	m_Max(-1000000.f, -1000000.f, -1000000.f)
{
}

CNavMesh::~CNavMesh()
{
}

bool CNavMesh::Init()
{
	return false;
}

bool CNavMesh::LoadMeshFullPathMultibyte(const char* FullPath)
{
	std::ifstream input(FullPath);
	std::string line;

	if (!input)
		return false;

	int VtxCount = 0;
	int PolyIndex = 0;

	while (std::getline(input, line))
	{
		std::vector<std::string> vecStr;

		SplitByDelimit(line, ' ', vecStr);

		if (vecStr[0] == "v")
		{		
			std::stringstream ss;
			float XData;

			ss << vecStr[1];
			ss >> XData;
			ss.clear();

			float YData;

			ss << vecStr[2];
			ss >> YData;
			ss.clear();

			float ZData;

			ss << vecStr[3];
			ss >> ZData;
			ss.clear();

			Vector3 Vertex;
			// recast navmesh 프로그램의 x축 방향과 DX에서 x축 방향이 반대
			//Vertex.x = XData;
			Vertex.x = -XData;
			Vertex.y = YData;
			Vertex.z = ZData;

			m_vecVertexPos.push_back(Vertex);
			m_vecOriginVertexPos.push_back(Vertex);
			
			if (m_Min.x > Vertex.x)
				m_Min.x = Vertex.x;
			if (m_Min.y > Vertex.y)
				m_Min.y = Vertex.y;
			if (m_Min.z > Vertex.z)
				m_Min.z = Vertex.z;

			if (m_Max.x < Vertex.x)
				m_Max.x = Vertex.x;
			if (m_Max.y < Vertex.y)
				m_Max.y = Vertex.y;
			if (m_Max.z < Vertex.z)
				m_Max.z = Vertex.z;
		}

		else if (vecStr[0] == "f")
		{
			std::stringstream ss;
			int Index1;

			ss << vecStr[1];
			ss >> Index1;
			ss.clear();

			--Index1;

			int Index2;

			ss << vecStr[2];
			ss >> Index2;
			ss.clear();

			--Index2;

			int Index3;

			ss << vecStr[3];
			ss >> Index3;
			ss.clear();

			--Index3;

			//m_vecIndex.push_back(Index3);
			m_vecIndex.push_back(Index3);
			m_vecIndex.push_back(Index2);
			m_vecIndex.push_back(Index1);
		}
	}
	
	// Transform을 쉽게 하기 위해서 중심을 (0, 0, 0)에 맞춰주기 위해서 아래와 같이 모든 정점에 중심점을 빼준다
	Vector3 Middle = (m_Min + m_Max) / 2.f;

	size_t Count = m_vecVertexPos.size();
	for (size_t i = 0; i < Count; ++i)
	{
		m_vecVertexPos[i] = m_vecVertexPos[i] - Middle;
		m_vecOriginVertexPos[i] = m_vecOriginVertexPos[i] - Middle;
	}


	CreatePolygonInfo();

	CheckAdjInfo();

	//Count = m_vecNavMeshPolygon.size();

	//for (size_t i = 0; i < Count; ++i)
	//{
	//	for (size_t j = 0; j < 3; ++j)
	//	{
	//		m_vecNavMeshPolygon[i].m_vecVertexPos[j] = m_vecNavMeshPolygon[i].m_vecVertexPos[j] - Middle;
	//		m_vecNavMeshPolygon[i].m_vecVertexOriginPos[j] = m_vecNavMeshPolygon[i].m_vecVertexPos[j] - Middle;

	//	}
	//}

	m_Min = m_Min - Middle;
	m_Max = m_Max - Middle;
	m_OriginMin = m_Min;
	m_OriginMax = m_Max;

	

	m_Scene->GetResource()->CreateNavMesh(this, "NavMesh",
		&m_vecVertexPos[0], sizeof(Vector3), (int)m_vecVertexPos.size(),
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);


	return true;
}

bool CNavMesh::LoadMeshFullPathMultibyte(std::string& OutName, const char* FullPath)
{
	return false;
}


void CNavMesh::CreatePolygonInfo()
{
	size_t Count = m_vecIndex.size();
	int PolyIndex = 0;
	for (size_t i = 0; i < Count; i+=3)
	{
		int Index1 = m_vecIndex[i];
		int Index2 = m_vecIndex[i+1];
		int Index3 = m_vecIndex[i+2];

		Vector3 Vertex1 = m_vecVertexPos[Index1];
		Vector3 Vertex2 = m_vecVertexPos[Index2];
		Vector3 Vertex3 = m_vecVertexPos[Index3];

		NavMeshPolygon Poly;
		Poly.m_Index = PolyIndex;
		Poly.m_vecVertexPos.push_back(Vertex1);
		Poly.m_vecVertexPos.push_back(Vertex2);
		Poly.m_vecVertexPos.push_back(Vertex3);
		
		Poly.m_vecVertexOriginPos.push_back(Vertex1);
		Poly.m_vecVertexOriginPos.push_back(Vertex2);
		Poly.m_vecVertexOriginPos.push_back(Vertex3);

		m_vecNavMeshPolygon.push_back(Poly);

		++PolyIndex;
	}

	int a = 3;
}

void CNavMesh::ReCreateVertexPos()
{
}

void CNavMesh::CheckAdjInfo()
{
	size_t Count = m_vecNavMeshPolygon.size();

	// 인접 노드 체크
	for (int i = 0; i < Count; ++i)
	{
		for (int j = 0; j < Count; ++j)
		{
			for (int p = 0; p < 3; ++p)
			{
				for (int q = 0; q < 3; ++q)
				{
					if (i != j && m_vecNavMeshPolygon[i].m_vecVertexPos[p].Distance(m_vecNavMeshPolygon[j].m_vecVertexPos[q]) < 0.001f)
					{
						// 인접 리스트에 없다면 넣어준다
						if (!CheckAdjVector(i, j))
						{
							m_vecNavMeshPolygon[i].m_vecAdjIndex.push_back(j);

							m_vecNavMeshPolygon[j].m_vecAdjIndex.push_back(i);
						}
					}
				}
			}
		}
	}
}

bool CNavMesh::CheckAdjVector(int PolyIndex, int CheckIndex)
{
	size_t Count = m_vecNavMeshPolygon[PolyIndex].m_vecAdjIndex.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecNavMeshPolygon[PolyIndex].m_vecAdjIndex[i] == CheckIndex)
			return true;
	}

	return false;
}

bool CNavMesh::SaveMesh(FILE* File)
{
	size_t Count = m_vecNavMeshPolygon.size();
	fwrite(&Count, sizeof(size_t), 1, File);
	for (size_t i = 0; i < Count; ++i)
	{
		NavMeshPolygon Poly = m_vecNavMeshPolygon[i];
		size_t VtxCount = Poly.m_vecVertexPos.size();

		fwrite(&VtxCount, sizeof(size_t), 1, File);
		
		for (size_t j = 0; j < VtxCount; ++j)
		{
			fwrite(&Poly.m_vecVertexPos[j], sizeof(Vector3), 1, File);
			fwrite(&Poly.m_vecVertexOriginPos[j], sizeof(Vector3), 1, File);
		}

		size_t AdjCount = Poly.m_vecAdjIndex.size();

		fwrite(&AdjCount, sizeof(size_t), 1, File);

		for (size_t j = 0; j < AdjCount; ++j)
		{
			fwrite(&Poly.m_vecAdjIndex[j], sizeof(int), 1, File);
		}

		fwrite(&Poly.m_Index, sizeof(int), 1, File);
	}

	Count = m_vecVertexPos.size();
	fwrite(&Count, sizeof(size_t), 1, File);
	for (size_t i = 0; i < Count; ++i)
	{
		fwrite(&m_vecVertexPos[i], sizeof(Vector3), 1, File);
		fwrite(&m_vecOriginVertexPos[i], sizeof(Vector3), 1, File);
	}

	Count = m_vecIndex.size();
	fwrite(&Count, sizeof(size_t), 1, File);
	for (size_t i = 0; i < Count; ++i)
	{
		fwrite(&m_vecIndex[i], sizeof(int), 1, File);
	}

	fwrite(&m_Min, sizeof(Vector3), 1, File);
	fwrite(&m_Max, sizeof(Vector3), 1, File);

	return true;
}

bool CNavMesh::LoadMesh(FILE* File)
{
	size_t PolyCount = 0;

	fread(&PolyCount, sizeof(size_t), 1, File);

	for (size_t i = 0; i < PolyCount; ++i)
	{
		NavMeshPolygon Poly;

		size_t VtxCount = 0;
		fread(&VtxCount, sizeof(size_t), 1, File);

		for (size_t j = 0; j < VtxCount; ++j)
		{
			Vector3 VertexPos;
			fread(&VertexPos, sizeof(Vector3), 1, File);
			Poly.m_vecVertexPos.push_back(VertexPos);

			Vector3 OriginPos;
			fread(&OriginPos, sizeof(Vector3), 1, File);
			Poly.m_vecVertexOriginPos.push_back(OriginPos);
		}

		size_t AdjCount = 0;

		fread(&AdjCount, sizeof(size_t), 1, File);

		for (size_t j = 0; j < AdjCount; ++j)
		{
			int AdjIdx = -1;
			fread(&AdjIdx, sizeof(int), 1, File);
			Poly.m_vecAdjIndex.push_back(AdjIdx);
		}

		fread(&Poly.m_Index, sizeof(int), 1, File);

		m_vecNavMeshPolygon.push_back(Poly);
	}


	size_t Count = 0;
	fread(&Count, sizeof(size_t), 1, File);

	for (size_t i = 0; i < Count; ++i)
	{
		Vector3 Vertex;
		fread(&Vertex, sizeof(Vector3), 1, File);
		m_vecVertexPos.push_back(Vertex);

		Vector3 OriginVertex;
		fread(&OriginVertex, sizeof(Vector3), 1, File);
		m_vecOriginVertexPos.push_back(OriginVertex);
	}


	fread(&Count, sizeof(size_t), 1, File);

	for (size_t i = 0; i < Count; ++i)
	{
		int Index;
		fread(&Index, sizeof(int), 1, File);
		m_vecIndex.push_back(Index);
	}

	fread(&m_Min, sizeof(Vector3), 1, File);
	fread(&m_Max, sizeof(Vector3), 1, File);

	m_OriginMin = m_Min;
	m_OriginMax = m_Max;

	if (m_OriginMin.x > m_OriginMax.x && m_OriginMin.z > m_OriginMax.z)
	{
		Vector3 Tmp = m_OriginMin;
		m_OriginMin = m_OriginMax;
		m_OriginMax = Tmp;
	}

	CreateNavMesh(m_Name);

	return true;
}

bool CNavMesh::CreateNavMesh(const std::string& Name)
{
	return CSceneManager::GetInst()->GetScene()->GetResource()->CreateNavMesh(this, Name,
		&m_vecOriginVertexPos[0], sizeof(Vector3), (int)m_vecVertexPos.size(),
		D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		&m_vecIndex[0], sizeof(int), (int)m_vecIndex.size(),
		D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT);
}

int CNavMesh::CheckPointIndex(const Vector3& Point)
{
	size_t Count = m_vecVertexPos.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecVertexPos[i] == Point)
		{
			return m_vecIndex[i];
		}
	}

	return -1;
}

void CNavMesh::SplitByDelimit(std::string Input, char delimit, std::vector<std::string>& vecOut)
{
	size_t Length = Input.length();

	std::string str;

	for (size_t i = 0; i < Length; ++i)
	{
		if (Input[i] == delimit)
		{
			vecOut.push_back(str);
			str.clear();
		}

		else if (i == Length - 1)
		{
			str += Input[i];
			vecOut.push_back(str);
			str.clear();
		}

		else
			str += Input[i];
	}
}

bool CNavMesh::ConvertFBX(CFBXLoader* Loader, const char* FullPath)
{
	return false;
}