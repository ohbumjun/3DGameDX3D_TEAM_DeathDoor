#pragma once

#include "Component/StateComponent.h"

class CGameStateComponent :
	public CStateComponent
{
	friend class CGameObject;

protected:
	CGameStateComponent();
	CGameStateComponent(const CGameStateComponent& com);
	virtual ~CGameStateComponent();

public:
	virtual void Start() override;

public:
	class CObjectDataComponent* GetData()
	{
		return m_Data;
	}

private:
	class CObjectDataComponent* m_Data;

public:
	class CObjectDataComponent* GetObjectDataComponent()	const;
};

