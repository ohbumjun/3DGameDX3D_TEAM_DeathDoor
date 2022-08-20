#pragma once

#include "ObjectComponent.h"
//#include "../Scene/Scene.h"
//#include "../Scene/SceneManager.h"

class CNavAgent :
    public CObjectComponent
{
	friend class CGameObject;

protected:
	CNavAgent();
	CNavAgent(const CNavAgent& com);
	virtual ~CNavAgent();

private:
	class CSceneComponent* m_UpdateComponent;
	float					m_MoveSpeed;
	float					m_RotationSpeed;
	bool					m_ApplyNavMesh;
	bool					m_Rotaiting;
	Vector3					m_CurrentFaceDir;

protected:
	std::list<Vector3>		m_PathList;

public:
	const Vector3& GetCurrentFaceDir()	const
	{
		return m_CurrentFaceDir;
	}

	void SetCurrentFaceDir(const Vector3& Dir)
	{
		m_CurrentFaceDir = Dir;
	}

	void SetUpdateComponent(class CSceneComponent* UpdateComponent);

	bool Move(const Vector3& EndPos);

	bool MoveOnNavMesh(const Vector3 EndPos);
	// 특정한 때(ex.평타 공격)에 특정 속도로 움직이고 싶은 경우
	bool MoveOnNavMesh(const Vector3 Dir, float Speed);

	void SetMoveSpeed(float Speed)
	{
		m_MoveSpeed = Speed;
	}

	void SetRotationSpeed(float Speed)
	{
		m_RotationSpeed = Speed;
	}

	void SetApplyNavMesh(bool Apply)
	{
		m_ApplyNavMesh = Apply;
	}

	bool GetApplyNavMesh()	const
	{
		return m_ApplyNavMesh;
	}

	const Vector3& GetTargetPos() const;

	std::list<Vector3>& GetPathList()
	{
		return m_PathList;
	}

	void DeleteTargetPos()
	{
		m_PathList.pop_back();
	}

public:
	void AddTargetPos(const Vector3& TargetPos)
	{
		m_PathList.push_back(TargetPos);
	}

	void ClearPathList()
	{
		m_PathList.clear();
	}

	bool IsEmptyPathList()	const
	{
		return m_PathList.empty();
	}

	bool IsRotaiting() const
	{
		return m_Rotaiting;
	}

	size_t GetPathListCount()	const
	{
		return m_PathList.size();
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CNavAgent* Clone();
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File);
	virtual bool LoadOnly(FILE* File);


private:
	void PathResult(const std::list<Vector3>& PathList);


public:
	void FillPathList(const std::list<Vector3>& PathList);
	void AddPath(const Vector3& EndPos);
	bool FindPath(class CSceneComponent* OwnerComponent, const Vector3& End);
	bool FindPathExcept(class CSceneComponent* OwnerComponent, const Vector3& End, std::vector<Vector3>& vecExceptPos);
	bool FindPathExcept(class CSceneComponent* OwnerComponent, const Vector3& End, std::vector<NavigationCell*>& vecExceptCell);
	bool CheckStraightPath(const Vector3& StartPos, const Vector3& EndPos, std::vector<Vector3>& vecPath);
	void ForceUpdateFaceDir();
};

