#pragma once

#include "DDSceneMode.h"

class CDDBossSceneMode :
    public CDDSceneMode
{
    friend class CScene;

protected:
    CDDBossSceneMode();
    virtual ~CDDBossSceneMode();

public:
    virtual void Start() override;

public:
    virtual bool SetExitPointObj(class CGameObject* Obj);

public:
    virtual void OnFadeInStart() override;
    virtual void OnClearDungeon() override;

protected:
    Vector3 m_OriginExitPos;
    class CPaperBurnComponent* m_PortalPaperBurn;
};

