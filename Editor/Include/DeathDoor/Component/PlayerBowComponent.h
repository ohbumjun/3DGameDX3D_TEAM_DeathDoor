#pragma once

#include "Component/StaticMeshComponent.h"

class CPlayerBowComponent :
    public CStaticMeshComponent
{
	friend class CGameObject;

protected:
	CPlayerBowComponent();
	CPlayerBowComponent(const CPlayerBowComponent& com);
	virtual ~CPlayerBowComponent();

private:
	class CPlayerDataComponent* m_PlayerData;
	class CGameObject* m_Arrow;
	bool m_Destroy;
	bool m_ShowBow;
	float m_EmvTimer;
	float m_EmvMaxTime;

public:
	void SetShowBow(bool Show)
	{
		m_ShowBow = Show;
	}

	bool GetShowBow()	const
	{
		return m_ShowBow;
	}

public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CPlayerBowComponent* Clone();

public:
	void ShowBow(const Vector3& ShootDir);
	void ShootArrow(const Vector3& ShootDir);
	void HideBow();
	void OnCollision(const CollisionResult& Result);

public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;
};

