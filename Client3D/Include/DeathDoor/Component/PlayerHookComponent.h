#pragma once

#include "Component/StaticMeshComponent.h"
#include "../DDFlag.h"

class CPlayerHookComponent :
    public CStaticMeshComponent
{
	friend class CGameObject;

protected:
	CPlayerHookComponent();
	CPlayerHookComponent(const CPlayerHookComponent& com);
	virtual ~CPlayerHookComponent();

private:
	std::vector<CStaticMeshComponent*> m_vecHookChain;
	int m_CurrentHookIndex;
	int m_ClearHookIndex;
	Vector2	m_UnitSize;
	class CColliderHalfLine* m_Collider;
	float		m_AccTime;
	bool		m_InFlying;
	Vector3		m_ShootDir;
	Vector3		m_ShootDestPoint;
	bool		m_PlayFireSound;
	bool		m_PlayInShootSound;
	
	float m_AccLastHookDelayTime;

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CPlayerHookComponent* Clone();

public:
	// 충돌 여부를 리턴
	HookResult ShootHook(const Vector3& ShootDir, float DeltaTime);
	void OnHookCollision(const CollisionResult& Result);

public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

public:
	virtual void SetMesh(CStaticMesh* Mesh);

private:
	void ClearHookChain();

};

