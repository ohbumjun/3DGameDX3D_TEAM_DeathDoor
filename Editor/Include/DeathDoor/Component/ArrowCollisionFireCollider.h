#pragma once
#include "Component\ColliderSphere.h"

class CArrowCollisionFireCollider :
    public CColliderSphere
{
    friend class CGameObject;

protected:
    CArrowCollisionFireCollider();
    CArrowCollisionFireCollider(const CArrowCollisionFireCollider& com);
    virtual ~CArrowCollisionFireCollider();
private :
    bool m_CollidedWithArrow;
    bool m_FirstManagedChildByTriggerLamp;
public:
    virtual void Start() override;
    virtual void Update(float DeltaTime) override;

public:
    void OnCollidePlayerArrow(const CollisionResult& Result);
public :
    void SetFirstManagedChildByTriggerLamp(bool Enable)
    {
        m_FirstManagedChildByTriggerLamp = Enable;
    }
    // Arrow 에 의해서 장작에 불이 붙었다면, Light Component가 활성화 되어 있을 것이다.
    bool IsFireOnByArrow() const;
};

