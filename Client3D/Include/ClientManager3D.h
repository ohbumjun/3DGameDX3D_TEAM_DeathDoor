#pragma once

#include "GameInfo.h"

class CClientManager3D
{
public:
	bool Init(HINSTANCE hInst);
	void CreateDefaultSceneMode();
	void LoadInitialScene();
	int Run();

public:
	void CreateSceneMode(class CScene* Scene, size_t Type);
	class CGameObject* CreateObject(class CScene* Scene, size_t Type);
	class CComponent* CreateComponent(class CGameObject* Obj, size_t Type);

public:
	void SetDebugRender(float DeltaTime);

	DECLARE_SINGLE(CClientManager3D)
};

