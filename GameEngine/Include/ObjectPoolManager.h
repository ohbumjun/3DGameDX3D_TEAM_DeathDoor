#pragma once

#include "GameInfo.h"

class CObjectPoolManager
{
private:
	int m_InitObjectCount;
	std::vector<class CGameObject*> m_ObjectVector;

public:
	bool Init();
	class CGameObject* Allocate();
	void Deallocate(class CGameObject* obj);

	DECLARE_SINGLE(CObjectPoolManager)
};
