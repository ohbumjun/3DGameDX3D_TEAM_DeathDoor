#pragma once
#include "Component/NavAgent.h"

class CMonsterNavAgent :
    public CNavAgent
{
	friend class CGameObject;

protected:
	CMonsterNavAgent();
	CMonsterNavAgent(const CMonsterNavAgent& com);
	virtual ~CMonsterNavAgent();

private:
	float	m_AccTime;
	float	m_PathFindCoolTime;
	float	m_PathFindCoolStart;
	bool	m_PathFindEnable;
	bool	m_CoolStart;
	int		m_CurPolyIndex;

public:
	bool IsCoolStart()	const
	{
		return m_CoolStart;
	}

	void SetCoolStart(bool Start)
	{
		m_CoolStart = Start;
	}

	void SetPathFindEnable(bool Enable)
	{
		m_PathFindEnable = Enable;
	}

	bool GetPathFindEnable()	const
	{
		return m_PathFindEnable;
	}

public:
	bool IsInNavMesh();

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CMonsterNavAgent* Clone();
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File);
	virtual bool LoadOnly(FILE* File);

};

