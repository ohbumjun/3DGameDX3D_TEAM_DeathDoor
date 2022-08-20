#pragma once

#include "../GameInfo.h"
#include "../Component/LandScape.h"
#include "../ThreadQueue.h"
#include "NavigationThread3D.h"
#include "../Component/NavAgent.h"


class CNavigation3DManager
{
	friend class CScene;

private:
	CNavigation3DManager();
	~CNavigation3DManager();

private:
	class CScene* m_Scene;
	std::vector<CNavigationThread3D*>	m_vecNavigationThread;
	CSharedPtr<CLandScape>				m_NavData;
	CThreadQueue<NavResultData>			m_ResultQueue;
	class CLandScape* m_LandScape;
	class CNavMeshComponent* m_NavMeshComponent;
	int m_PlayerPolyIndex;

public:
	void SetPlayerPolyIndex(int Index)
	{
		m_PlayerPolyIndex = Index;
	}

	int GetPlayerPolyIndex()	const
	{
		return m_PlayerPolyIndex;
	}

	class CNavMeshComponent* GetNavMeshData() const;
	void SetNavMeshData(CNavMeshComponent* NavComp);



	void SetNavData(CLandScape* NavData);
	void AddNavResult(const NavResultData& NavData);
	void SetLandScape(class CLandScape* LandScape);
	class CLandScape* GetLandScape()	const;

public:
	float GetY(const Vector3& Pos);
	bool CheckPickingPoint(Vector3& OutPos);
	// 높이를 결과로 리턴
	bool CheckPlayerNavMeshPoly(float& Height);
	// 높이와 Polygon Index를 리턴
	bool CheckNavMeshPoly(const Vector3& Pos, float& Height, int& PolyIndex);
	// 현재 위치 기준으로 m_PlayerPolyIndex를 갱신
	bool RefreshPlayerNavMeshPoly(const Vector3& Pos);

	// CurrentPolyIndex에 존재하는지 체크 후, 존재하지 않는다면 인접 Polygon을 탐색해서 OutPolyIndex에 반환해준다
	bool CheckCurrentNavMeshPoly(const Vector3& Pos, int CurrentPolyIndex, float& Height, int& OutPolyIndex);
	bool CheckAdjNavMeshPoly(const Vector3& Pos, int CurrentPolyIndex, float& Height, int& PolyIndex);
	bool CheckNavMeshPickingPoint(Vector3& OutPos);
	bool CheckStraightPath(const Vector3& StartPos, const Vector3& EndPos, std::vector<Vector3>& vecPath);
	NavigationCell* FindCell(int PolyIndex);
	void FindAdjCell(int PolyIndex, std::vector<NavigationCell*>& vecCell);

public:
	void Start();
	bool Init();
	void Update(float DeltaTime);

public:
	template <typename T1, typename T2>
	bool FindPath(T1* Obj, void(T1::* Func)(const std::list<Vector3>&), T2* OwnerComponent, const Vector3& End)
	{
		if (m_vecNavigationThread.empty())
			return false;

		int	Count = m_vecNavigationThread[0]->GetWorkCount();
		int	WorkIndex = 0;

		size_t	Size = m_vecNavigationThread.size();

		for (size_t i = 1; i < Size; ++i)
		{
			if (Count > m_vecNavigationThread[i]->GetWorkCount())
			{
				Count = m_vecNavigationThread[i]->GetWorkCount();
				WorkIndex = (int)i;
			}
		}

		m_vecNavigationThread[WorkIndex]->AddWork<T1, T2>(Obj, Func, OwnerComponent, End);

		return true;
	}

	template <typename T1, typename T2>
	bool FindPathExcept(T1* Obj, void(T1::* Func)(const std::list<Vector3>&), T2* OwnerComponent, const Vector3& End, std::vector<Vector3>& vecExceptPos)
	{
		if (m_vecNavigationThread.empty())
			return false;

		int	Count = m_vecNavigationThread[0]->GetWorkCount();
		int	WorkIndex = 0;

		size_t	Size = m_vecNavigationThread.size();

		for (size_t i = 1; i < Size; ++i)
		{
			if (Count > m_vecNavigationThread[i]->GetWorkCount())
			{
				Count = m_vecNavigationThread[i]->GetWorkCount();
				WorkIndex = (int)i;
			}
		}

		m_vecNavigationThread[WorkIndex]->DisableCell(vecExceptPos);
		m_vecNavigationThread[WorkIndex]->AddWork<T1, T2>(Obj, Func, OwnerComponent, End);

		return true;
	}

	template <typename T1, typename T2>
	bool FindPathExcept(T1* Obj, void(T1::* Func)(const std::list<Vector3>&), T2* OwnerComponent, const Vector3& End, std::vector<NavigationCell*>& vecExceptCell)
	{
		if (m_vecNavigationThread.empty())
			return false;

		int	Count = m_vecNavigationThread[0]->GetWorkCount();
		int	WorkIndex = 0;

		size_t	Size = m_vecNavigationThread.size();

		for (size_t i = 1; i < Size; ++i)
		{
			if (Count > m_vecNavigationThread[i]->GetWorkCount())
			{
				Count = m_vecNavigationThread[i]->GetWorkCount();
				WorkIndex = (int)i;
			}
		}

		m_vecNavigationThread[WorkIndex]->DisableCell(vecExceptCell);
		m_vecNavigationThread[WorkIndex]->AddWork<T1, T2>(Obj, Func, OwnerComponent, End);

		return true;
	}
};


