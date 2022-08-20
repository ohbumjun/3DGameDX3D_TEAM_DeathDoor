#pragma once

#include "GameObject.h"
#include "../Component/StaticMeshComponent.h"

class CSkyObject :
    public CGameObject
{
    friend class CScene;
    friend class CSceneManager;
    friend class CEffectEditor;

protected:
    CSkyObject();
    CSkyObject(const CSkyObject& obj);
    virtual ~CSkyObject();

private:
    CSharedPtr<CStaticMeshComponent>    m_Mesh;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CSkyObject* Clone();

public:
    bool SetSkyTexture(const char* FileName, const std::string& PathName = TEXTURE_PATH);
    bool SetSkyTextureFullPath(const char* FullPath);
    CTexture* GetSkyTexture();
};

