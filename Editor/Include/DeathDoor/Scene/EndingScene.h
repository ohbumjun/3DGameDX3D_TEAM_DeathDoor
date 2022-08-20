#pragma once

#include "Scene\SceneMode.h"

class CEndingScene :
    public CSceneMode
{
	friend class CScene;

protected:
	CEndingScene();
	virtual ~CEndingScene();

public:
	virtual bool Init() override;
	virtual void Start() override;

public:
	virtual bool Save(FILE* File) override;
	virtual bool Load(FILE* File) override;
};

