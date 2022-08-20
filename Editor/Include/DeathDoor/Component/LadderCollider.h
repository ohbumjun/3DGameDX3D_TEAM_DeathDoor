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
	// �÷��̾ ��ٸ��� ������� �ö󰡸� PlayerDataComponent�� m_LadderDownEnable�� true�� ���ְų�
	// ��ٸ��� ������� �������� m_LadderUpEnable = true�� ���༭ ��ٸ����� ���Ʒ� ��� �̵��� �����ϰ� �ϴ� �Լ�
	void OnPlayerLadderColliderEnd(const CollisionResult& Result);
};

