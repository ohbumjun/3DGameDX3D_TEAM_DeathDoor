#pragma once

#include "ColliderComponent.h"

class CColliderRay :
    public CColliderComponent
{
    friend class CGameObject;

protected:
    CColliderRay();
    CColliderRay(const CColliderRay& com);
    virtual ~CColliderRay();

protected:
    Ray   m_RayInfo;
    Vector3 m_StartPos;
    Vector3 m_EndPos;

public:
    const Ray& GetInfo()  const
    {
        return m_RayInfo;
    }

    const Vector3& GetStartPos()    const
    {
        return m_StartPos;
    }

    const Vector3& GetEndPos()    const
    {
        return m_EndPos;
    }

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual void PrevRender();
    virtual void Render();
    virtual void PostRender();
    virtual CColliderRay* Clone();
    virtual bool Save(FILE* File);
    virtual bool Load(FILE* File);
    virtual bool Collision(CColliderComponent* Dest);
    virtual bool CollisionMouse(const Vector2& MousePos);
};

