#pragma once

#include "GameObject.h"
#include "../Component/LightComponent.h"

class CLightObj :
    public CGameObject
{
    friend class CScene;
    friend class CSceneManager;

protected:
    CLightObj();
    CLightObj(const CLightObj& obj);
    virtual ~CLightObj();

private:
    CSharedPtr<CLightComponent>    m_Light;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CLightObj* Clone();
};

