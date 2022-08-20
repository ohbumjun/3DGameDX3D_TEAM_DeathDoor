#pragma once
#include "ObjectDataComponent.h"
class CTriggerFireLamp :
    public CObjectDataComponent
{
	friend class CGameObject;

private:
	CTriggerFireLamp();
	virtual ~CTriggerFireLamp();

private:
	bool m_CurrentActive;
	int m_NumCurActiveFireLamp;
	int m_NumFullActiveFireLamp;

public:
	void SetCurrentActive(bool Active)
	{
		m_CurrentActive = Active;
	}

	bool IsCurrentActive()	const
	{
		return m_CurrentActive;
	}

	void IncActiveCount();

public:
	virtual void Start() override;
};

