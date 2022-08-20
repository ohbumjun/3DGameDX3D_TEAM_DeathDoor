#pragma once
#include "MovingObject.h"
class CChampion :
    public CMovingObject
{
	friend class CScene;

protected:
	CChampion();
	CChampion(const CChampion& obj);
	virtual ~CChampion();

protected:


public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CChampion* Clone();

};

