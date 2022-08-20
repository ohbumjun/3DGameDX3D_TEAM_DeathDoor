#pragma once

#include "ObjectDataComponent.h"

class CTriggerBoxData :
    public CObjectDataComponent
{
	friend class CGameObject;

private:
	CTriggerBoxData();
	CTriggerBoxData(const CTriggerBoxData& com);
	virtual ~CTriggerBoxData();

private:
	bool m_CurrentActive;
	int m_BoxOrder;

public:
	void SetCurrentActive(bool Active)
	{
		m_CurrentActive = Active;
	}

	bool IsCurrentActive()	const
	{
		return m_CurrentActive;
	}

	void SetBoxOrder(int Order)
	{
		m_BoxOrder = Order;
	}

	int GetBoxOrder()	const
	{
		return m_BoxOrder;
	}

public:
	virtual void Start() override;
	virtual void Update(float DeltaTime) override;


public:
	virtual bool Save(FILE* File);
	virtual bool Load(FILE* File);
};

