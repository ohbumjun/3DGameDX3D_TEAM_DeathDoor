#pragma once

#include "Component\ColliderBox3D.h"

class CLadderCollider :
    public CColliderBox3D
{
	friend class CGameObject;

protected:
	CLadderCollider();
	CLadderCollider(const CLadderCollider& com);
	virtual ~CLadderCollider();

private:


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CLadderCollider* Clone();


public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

public:
	void OnPlayerLadderColliderBegin(const CollisionResult& Result);
	void OnPlayerLadderDisable(const CollisionResult& Result);
	// 플레이어가 사다리에 어느정도 올라가면 PlayerDataComponent의 m_LadderDownEnable을 true로 해주거나
	// 사다리에 어느정도 내려가면 m_LadderUpEnable = true로 해줘서 사다리에서 위아래 모두 이동이 가능하게 하는 함수
	void OnPlayerLadderColliderEnd(const CollisionResult& Result);
};

