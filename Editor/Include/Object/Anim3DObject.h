#pragma once
#include "GameObject\GameObject.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/CameraComponent.h"
#include "Component/Arm.h"

class CAnim3DObject :
    public CGameObject
{
    friend class CScene;
    friend class CAnimationEditor;
protected:
    CAnim3DObject();
    CAnim3DObject(const CAnim3DObject& obj);
    virtual ~CAnim3DObject();

private:
    CSharedPtr<CAnimationMeshComponent>    m_Mesh;
    CSharedPtr<CArm>                    m_Arm;
    CSharedPtr<CCameraComponent>        m_Camera;
    Vector3     m_Velocity;
    class CAnimationSequenceInstance* m_Animation;
    bool m_IsCameraRot;
    bool m_IsCameraZoom;
public :
    bool IsCameraRot() const
    {
        return m_IsCameraRot;
    }
    bool IsCameraZoom() const
    {
        return m_IsCameraZoom;
    }
    class CAnimationSequenceInstance* GetAnimationInstance() const
    {
        return m_Animation;
    }
public :
    void SetCameraRot(bool Enable)
    {
        m_IsCameraRot = Enable;
    }
    void SetCameraZoom(bool Enable)
    {
        m_IsCameraZoom = Enable;
    }
public:
    virtual bool Init();
    virtual void Start();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CAnim3DObject* Clone();

private:
    void MoveFront(float DeltaTime);
    void MoveBack(float DeltaTime);
    void RotationYInv(float DeltaTime);
    void RotationY(float DeltaTime);
    void Attack(float DeltaTime);
private  :
    void SetMeshAndMaterialInfo();
public:
    CCameraComponent* GetCamera() const
    {
        return m_Camera;
    }
};

