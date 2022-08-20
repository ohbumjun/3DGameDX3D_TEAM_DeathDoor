#pragma once

#include "../GameInfo.h"

struct CollisionSectionInfo
{
	std::vector<class CCollisionSection*>	vecSection;
	Vector3	SectionSize;		// 영역 1칸의 크기
	Vector3	Center;				// 전체 영역에서의 중심
	Vector3	SectionTotalSize;	// 전체 영역 크기
	Vector3	Min;				// 전체 영역에서의 최소값
	Vector3	Max;				// 전체 영역에서의 최대값
	int		CountX;
	int		CountY;
	int		CountZ;

	//CollisionSectionInfo() :
	//	CountX(1),
	//	CountY(1),
	//	CountZ(1),
	//	SectionSize{1000.f, 1000.f, 1.f},
	//	SectionTotalSize{1000.f, 1000.f, 1.f},
	//	Min{-500.f, -500.f, -0.5f},
	//	Max{ 500.f, 500.f, 0.5f }
	//{
	//}

	CollisionSectionInfo() :
		CountX(1),
		CountY(1),
		CountZ(1),
		SectionSize{ 1000.f, 1000.f, 1.f },
		SectionTotalSize{ 1000.f, 1000.f, 1.f },
		Min{ 0.f, 0.f, 0.f },
		Max{ 1000.f, 0.f, 1000.f }
	{
	}
};


class CSceneCollision
{
	friend class CScene;

private:
	CSceneCollision();
	~CSceneCollision();

private:
	class CScene* m_Scene;

private:
	CollisionSectionInfo* m_Section;
	std::list<CSharedPtr<class CColliderComponent>>	m_ColliderList;
	CSharedPtr<class CColliderComponent> m_MouseCollision;
	bool	m_WidgetClick;
	Vector3 m_NavMeshMin;
	Vector3 m_NavMeshMax;

public:
	void SetNavMeshMin(const Vector3& Min)
	{
		m_NavMeshMin = Min;
	}

	void SetNavMeshMax(const Vector3& Max)
	{
		m_NavMeshMax = Max;
	}

	CollisionSectionInfo* GetCollisionSectionInfo()	const
	{
		return m_Section;
	}

	int GetSectionCountX()	const
	{
		return m_Section->CountX;
	}

	int GetSectionCountZ()	const
	{
		return m_Section->CountZ;
	}

public:
	void Start();
	bool Init();
	bool CollisionWidget(float DeltaTime);
	void Collision(float DeltaTime);

protected:
	void CollisionMouse(float DeltaTime);

public:
	void SetSectionSize(const Vector3& Size);
	void SetSectionSize(float x, float y, float z = 1.f);
	void SetSectionCenter(const Vector3& Center);
	void SetSectionCenter(float x, float y, float z);
	void SetSectionMin(const Vector3& Min);
	void SetSectionMin(float x, float y, float z);
	void SetSectionMax(const Vector3& Max);
	void SetSectionMax(float x, float y, float z);
	void SetSectionCount(int CountX, int CountY, int CountZ = 1);
	void CreateSection();
	void CreateSection3D();
	void Clear();
	void AddCollider(class CColliderComponent* Collider);
	int GetSectionIndex(const Vector3& Pos);
	int GetSectionIndexX(const Vector3& Pos);
	int GetSectionIndexZ(const Vector3& Pos);
	// 해당 Section에 들어있는 Collider를 모아놓은 vector를 리턴
	const std::vector<CColliderComponent*>& GetSectionCollider(int Index);
	// 인자로 주어진 Collision Profile을 가진 Collider가 하나라도 있는지
	bool IsExistColliderHasProfile(CollisionProfile* Profile);
	void DeleteColliderInSection(class CColliderComponent* Collider, int SectionIndex);
	void EraseCollider(class CColliderComponent* Collider);
	void ClearAll();

private:
	void CheckColliderSection();
	void CheckColliderSection3D();
};

