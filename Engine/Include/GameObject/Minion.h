#pragma once
#include "MovingObject.h"
class CMinion :
    public CMovingObject
{
	friend class CScene;

protected:
	CMinion();
	CMinion(const CMinion& obj);
	virtual ~CMinion();

protected:



public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CMinion* Clone();
};

