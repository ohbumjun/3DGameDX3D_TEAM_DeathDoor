#pragma once
#include "MonsterDataComponent.h"
class CTinyCrowDataComponent :
    public CMonsterDataComponent
{
	friend class CGameObject;

private:
	CTinyCrowDataComponent();
	CTinyCrowDataComponent(const CTinyCrowDataComponent& com);
	virtual ~CTinyCrowDataComponent();

private:
	float m_AccTime;
	class CGameObject* m_CrowOwner;

public:
	void SetCrowOwner(class CGameObject* Owner);

public:
	virtual void Start() override;
	virtual void Update(float DeltaTime);

	void OnCollisionPlayer(const CollisionResult& Result);
};

