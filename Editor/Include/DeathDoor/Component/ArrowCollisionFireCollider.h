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
    // Arrow �� ���ؼ� ���ۿ� ���� �پ��ٸ�, Light Component�� Ȱ��ȭ �Ǿ� ���� ���̴�.
    bool IsFireOnByArrow() const;
};

