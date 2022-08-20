#pragma once
#include "GameObject.h"
class CMapObject :
    public CGameObject
{
	friend class CScene;

protected:
	CMapObject();
	CMapObject(const CMapObject& obj);
	virtual ~CMapObject();


protected:



public:
	virtual void Start();
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
	virtual CMapObject* Clone();
};

