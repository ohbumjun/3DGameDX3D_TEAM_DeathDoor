#pragma once

#include "ColliderComponent.h"

class CColliderHalfLine :
    public CColliderComponent
{
    friend class CGameObject;

protected:
    CColliderHalfLine();
    CColliderHalfLine(const CColliderHalfLine& com);
    virtual ~CColliderHalfLine();

protected:
    HalfLineInfo   m_Info;
    Vector3 m_IntersectPoint;

public:
    HalfLineInfo GetInfo()  const
    {
        return m_Info;
    }

    const Vector3& GetIntersectPoint()  const
    {
        return m_IntersectPoint;
    }

    void SetIntersectPoint(const Vector3& Point)
    {
        m_IntersectPoint = Point;
    }

public:
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual void PrevRender();
    virtual void Render();
    virtual void PostRender();
    virtual CColliderHalfLine* Clone();
    virtual bool Save(FILE* File);
    virtual bool Load(FILE* File);
    virtual bool Collision(CColliderComponent* Dest);
    virtual bool CollisionMouse(const Vector2& MousePos);

public:
    void RefreshInfo();
};

