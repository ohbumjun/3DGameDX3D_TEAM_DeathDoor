#pragma once

#include "Mesh.h"

class CNavMesh :
    public CMesh
{
	friend class CMeshManager;
	friend class CNavMeshComponent;

protected:
	CNavMesh();
	virtual ~CNavMesh();

private:
	std::vector<NavMeshPolygon>			m_vecNavMeshPolygon;
	std::vector<Vector3>				m_vecVertexPos;
	std::vector<Vector3>				m_vecOriginVertexPos;
	std::vector<int>					m_vecIndex;
	Vector3								m_Min;
	Vector3								m_Max;
	Vector3								m_OriginMin;
	Vector3								m_OriginMax;

	int m_PolyCount;

public:
	const Vector3& GetMin()	const
	{
		return m_Min;
	}

	const Vector3& GetMax()	const
	{
		return m_Max;
	}

	const Vector3& GetOriginMin()	const
	{
		return m_OriginMin;
	}

	const Vector3& GetOriginMax()	const
	{
		return m_OriginMax;
	}

	size_t GetNavMeshPolygonCount() const
	{
		return m_vecNavMeshPolygon.size();
	}

	NavMeshPolygon& GetNavMeshPolygon(int Idx)
	{
		return m_vecNavMeshPolygon[Idx];
	}


	size_t GetVertexPosCount()	const
	{
		return m_vecVertexPos.size();
	}

	Vector3& GetVertexPos(int Index)
	{
		return m_vecVertexPos[Index];
	}

	Vector3& GetVertexOriginPos(int Index)
	{
		return m_vecOriginVertexPos[Index];
	}

	void SetVertexPos(int Index, const Vector3& Pos)
	{
		m_vecVertexPos[Index] = Pos;
	}

	// m_mapIndex�� �̹� Vertex�� �����ϸ� �� Vertex�� Index�� �����ϰ� ������ -1�� �����ؼ�
	// �� Vertex���� ���ο� Index�� �ο�
	int CheckAndAssignIndex(const Vector3& Vertex);

public:
	virtual bool Init();
	virtual bool LoadMeshFullPathMultibyte(const char* FullPath);
	virtual bool LoadMeshFullPathMultibyte(std::string& OutName, const char* FullPath) override;

public:
	virtual bool ConvertFBX(class CFBXLoader* Loader, const char* FullPath);
	virtual bool SaveMesh(FILE* File);
	virtual bool LoadMesh(FILE* File);
	bool CreateNavMesh(const std::string& Name);

private:
	int CheckPointIndex(const Vector3& Point);
	void SplitByDelimit(std::string Input, char delimit, std::vector<std::string>& vecOut);
	void CreatePolygonInfo();
	void ReCreateVertexPos();
	void CheckAdjInfo();
	bool CheckAdjVector(int PolyIndex, int CheckIndex);
};

