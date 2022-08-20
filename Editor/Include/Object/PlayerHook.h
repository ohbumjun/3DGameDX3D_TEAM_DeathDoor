#pragma once

#include "GameObject/GameObject.h"

class CPlayerHook :
    public CGameObject
{
    friend class CScene;

protected:
    CPlayerHook();
    CPlayerHook(const CPlayerHook& obj);
    virtual ~CPlayerHook();

public:
    virtual bool Init();
    virtual void Start();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CPlayerHook* Clone();

private:
    float m_Distance;
    float m_AccDistance;
    float m_AccUnitDistance;
    class CStaticMeshComponent* m_HookHeadMesh;
    std::list<class CSceneComponent*> m_HookBodyList;

public:
    // ��Ÿ����� �� ��������� ����
    bool Move(const Vector3& Dir, float Speed, bool& Collision);

public:
    void DisableRenderAllComponent();
    void EnableRenderAllComponent();
};
