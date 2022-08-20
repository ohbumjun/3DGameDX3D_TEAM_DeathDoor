#pragma once

#include "ObjectComponent.h"

class CChampionAI :
    public CObjectComponent
{
	friend class CGameObject;

protected:
	CChampionAI();
	CChampionAI(const CChampionAI& com);
	virtual ~CChampionAI();
};

