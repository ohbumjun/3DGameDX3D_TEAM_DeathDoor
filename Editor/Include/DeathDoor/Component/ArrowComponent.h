#pragma once

#include "ProjectileComponent.h"

class CArrowComponent :
    public CProjectileComponent
{
	friend class CGameObject;

protected:
	CArrowComponent();
	CArrowComponent(const CArrowComponent& com);
	virtual ~CArrowComponent();

private:
	bool m_OnFire;
	class CColliderComponent* m_Collider;

public:
	virtual bool Init() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float DeltaTime) override;
	virtual void PostUpdate(float DeltaTime) override;
	virtual void PrevRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual CObjectComponent* Clone() override;

public:
	virtual bool Save(FILE* File) override;
	virtual bool Load(FILE* File) override;
	virtual bool SaveOnly(FILE* File) override;
	virtual bool LoadOnly(FILE* File) override;

public :
	bool IsArrowOnFire() const
	{
		return m_OnFire;
	}
public :
	void SetArrowOnFireEnable(bool Enable)
	{
		m_OnFire = Enable;
	}
};

