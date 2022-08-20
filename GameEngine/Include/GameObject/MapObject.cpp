
#include "MapObject.h"

CMapObject::CMapObject()
{
	SetTypeID<CMapObject>();
}

CMapObject::CMapObject(const CMapObject& obj)	:
	CGameObject(obj)
{
}

CMapObject::~CMapObject()
{
}

void CMapObject::Start()
{
	CGameObject::Start();
}

bool CMapObject::Init()
{
	CGameObject::Init();

	return true;
}

void CMapObject::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CMapObject::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

void CMapObject::PrevRender()
{
	CGameObject::PrevRender();
}

void CMapObject::Render()
{
	CGameObject::Render();
}

void CMapObject::PostRender()
{
	CGameObject::PostRender();
}

CMapObject* CMapObject::Clone()
{
	return new CMapObject(*this);
}
