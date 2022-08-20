#pragma once

#include "Scene/SceneMode.h"

class CDefaultScene :
    public CSceneMode
{
public:
    CDefaultScene();
    ~CDefaultScene();

public:
    virtual bool Init();
    virtual void Update(float DetaTime) override;

private:
    bool LoadAnimationSequence2D();
    bool CreateMaterial();

private:
    class CDataManager* m_DataManager;

private:
    class CGameObject* m_PickObj;
    class CAnimationMeshComponent* m_PickAnimMesh;
    class CStaticMeshComponent* m_PickStaticMesh;
};

