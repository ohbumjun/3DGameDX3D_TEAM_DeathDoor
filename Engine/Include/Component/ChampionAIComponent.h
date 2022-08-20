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
	bool m_InHome; // ������ ��ȯ�� ���¿��� ���� �ڱ� ������ ���������� �����ϴ���

};

