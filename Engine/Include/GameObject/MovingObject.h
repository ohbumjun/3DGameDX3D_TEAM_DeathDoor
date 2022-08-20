#pragma once

#include "GameObject.h"

class CMovingObject :
    public CGameObject
{
	friend class CScene;

protected:
	CMovingObject();
	CMovingObject(const CMovingObject& obj);
	virtual ~CMovingObject();


protected:



public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CMovingObject* Clone();

};

