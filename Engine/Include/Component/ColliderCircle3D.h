#pragma once

#include "ColliderComponent.h"

class CColliderCircle3D :
    public CColliderComponent
{
    friend class CGameObject;

protected:
    CColliderCircle3D();
    CColliderCircle3D(const CColliderCircle3D& com);
    virtual ~CColliderCircle3D();

protected:
    Circle3DInfo   m_Info;

public:
    Circle3DInfo GetInfo() const
    {
        return m_Info;
    }

    void SetInfo(const Vector3& Center, float Radius)
    {
        m_Info.Center = Center;
        m_Info.Radius = Radius;

        SetWorldScale(m_Info.Radius, m_Info.Radius, m_Info.Radius);
    }

    void SetRadius(float Radius)
    {
        m_Info.Radius = Radius;
    }

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual void PrevRender();
    virtual void Render();
    virtual void PostRender();
    virtual CColliderCircle3D* Clone();
    virtual bool Save(FILE* File);
    virtual bool Load(FILE* File);
    virtual bool Collision(CColliderComponent* Dest);
    virtual bool CollisionMouse(const Vector2& MousePos);
};

