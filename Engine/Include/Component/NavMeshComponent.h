#pragma once

#include "SceneComponent.h"
#include "../Resource/ResourceManager.h"
#include "../Render/RenderManager.h"
#include "../Render/RenderState.h"

class CNavMeshComponent :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CCameraManager;
	friend class CNavigationThread3D;

protected:
	CNavMeshComponent();
	CNavMeshComponent(const CNavMeshComponent& com);
	virtual ~CNavMeshComponent();

protected:
	CSharedPtr<CNavMesh>	m_NavMesh;
	CSharedPtr<class CShader> m_Shader;

	bool m_DebugRender;
	CSharedPtr<CRenderState> m_WireFrameState;

	Vector3 m_PlayerSpawnPos;
	int m_PlayerSpawnPolyIndex;
	//int		m_CountX;
	//int		m_CountZ;
	//std::vector<NavMeshPolygon>		m_vecNavMeshPolygon;
	//Vector3 m_Min;
	//Vector3 m_Max;

	std::list<NavigationCell*> m_UseCellList;
	std::list<NavigationCell*> m_OpenList;
	std::list<NavigationCell*> m_CloseList;
	std::unordered_map<int, NavigationCell*> m_mapCell;

public:
	void SetPlayerSpawnPolyIndex(int Index)
	{
		m_PlayerSpawnPolyIndex = Index;
	}

	int GetPlayerSpawnPolyIndex()	const
	{
		return m_PlayerSpawnPolyIndex;
	}

	void SetPlayerSpawnPos(const Vector3& Pos)
	{
		m_PlayerSpawnPos = Pos;
	}

	const Vector3& GetPlayerSpawnPos()	const
	{
		return m_PlayerSpawnPos;
	}

	CNavMesh* GetNavMesh()	const
	{
		return m_NavMesh;
	}

	void SetDebugRender(bool Debug)
	{
		m_DebugRender = Debug;

		if (m_DebugRender)
		{
			m_Shader = CResourceManager::GetInst()->FindShader("Standard3DWireFrameShader");
			m_WireFrameState = CRenderManager::GetInst()->FindRenderState("Wireframe");
		}
	}

	bool GetDebugRender() const
	{
		return m_DebugRender;
	}

public:
	void SetNavMesh(const std::string& Name);
	void SetNavMesh(class CNavMesh* NavMesh);
	void GetNavPolgonVertexPos(int Index, std::vector<Vector3>& vecPos);
	void GetAdjPolyIndex(int Index, std::vector<int>& vecPolyIndex);
	const Vector3& GetVertexPos(int PolyIndex, int VertexIndex);
	//void OnUpdatePos(const Vector3& WorldPos, const Vector3& RelativePos);
	//void OnUpdateRotation(const Vector3& WorldRot, const Vector3& RelativeRot);
	//void OnUpdateScale(const Vector3& WorldScale, const Vector3& RelativeScale);
	void OnUpdateWorld(const Vector3& World, const Vector3& Relative);

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CNavMeshComponent* Clone();
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;


	void FindPath(const Vector3& Start, const Vector3& End, std::list<Vector3>& vecPath);
	NavigationCell* FindCell(const Vector3& Pos);

	void AddCellCloseList(NavigationCell* Cell, NavigationCell* End);
	// 첫번째 인자로 들어온 Cell의 인접 Cell들을 열린 목록에 넣어준다
	void AddAdjCellOpenList(NavigationCell* Cell, NavigationCell* End, int ParentIndex);
	void DelteCellOpenList(NavigationCell* Cell);
	void MakePathList(std::list<Vector3>& PathList);
	bool CheckStraightPath(const Vector3& StartPos, const Vector3& EndPos, std::vector<Vector3>& vecPath);
	void ResetAllCell();
	
	NavigationCell* FindCell(int PolyIndex);
	void FindAdjCell(int PolyIndex, std::vector<NavigationCell*>& vecCell);

	static bool SortByTotal(NavigationCell* Src, NavigationCell* Dest)
	{
		return Src->Total < Dest->Total;
	}
};

