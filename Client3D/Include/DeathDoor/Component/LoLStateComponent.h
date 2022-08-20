#pragma once

#include "Component/StateComponent.h"

class CLoLStateComponent :
    public CStateComponent
{
	friend class CGameObject;

protected:
	CLoLStateComponent();
	CLoLStateComponent(const CLoLStateComponent& com);
	virtual ~CLoLStateComponent();

};

