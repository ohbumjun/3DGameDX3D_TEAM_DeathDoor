#pragma once
#include "ObjectComponent.h"
class CChampionAIComponent :
    public CObjectComponent
{
	friend class CGameObject;

protected:
	CChampionAIComponent();
	CChampionAIComponent(const CChampionAIComponent& com);
	virtual ~CChampionAIComponent();

private:
	LoLLine m_Line;
	int m_DetectRange;
	bool m_InHome; // 집으로 귀환한 상태여서 지금 자기 라인의 최전선으로 가야하는지

};

