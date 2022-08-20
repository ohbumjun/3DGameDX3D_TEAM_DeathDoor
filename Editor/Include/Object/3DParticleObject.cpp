#include "3DParticleObject.h"
#include "Component/ParticleComponent.h"
#include "Component/CameraComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Input.h"
#include "../EditorInfo.h"

C3DParticleObject::C3DParticleObject() :
    m_IsCameraRotate(true),
    m_IsCameraZoom(true),
    m_RotateInv(false)
{
    SetTypeID<C3DParticleObject>();
    m_ExcludeSceneSave = true;
}

C3DParticleObject::C3DParticleObject(const C3DParticleObject& obj) : CGameObject(obj)
{
}

C3DParticleObject::~C3DParticleObject()
{
}

bool C3DParticleObject::Init()
{
    if (!CGameObject::Init())
        return false;

    m_ParticleComponent = CreateComponent<CParticleComponent>("Particle Sprite");
    m_ParticleComponent->SetLayerName("ParticleEditorLayer");

    m_ParticleArm = CreateComponent<CArm>("Arm");

    m_ParticleCamera = CreateComponent<CCameraComponent>("Particle Camera");

    // Component 아래 Arm
    m_ParticleComponent->AddChild(m_ParticleArm);

    // Arm 아래 Camera
    m_ParticleArm->AddChild(m_ParticleCamera);

    m_ParticleCamera->SetInheritRotX(true);
    m_ParticleCamera->SetInheritRotY(true);
    m_ParticleCamera->SetInheritRotZ(true);

    m_ParticleCamera->SetCameraType(Camera_Type::Camera3D);
    m_ParticleCamera->SetViewAngle(27.f);
    m_ParticleCamera->CreateCustomResolutionProjMatrix(ParticleEditorWidth, ParticleEditorHeight);

    // Camera Setting
    CSceneManager::GetInst()->GetScene()->GetCameraManager()->SetParticleEditorCamera(m_ParticleCamera);

    m_ParticleArm->SetOffset(0.f, 2.f, 0.f);
    m_ParticleArm->SetRelativeRotation(45.f, 0.f, 0.f);
    m_ParticleArm->SetTargetDistance(90.f);

	return true;
}

void C3DParticleObject::Start()
{
    // Camera Setting
    CSceneManager::GetInst()->GetScene()->GetCameraManager()->SetParticleEditorCamera(m_ParticleCamera);
}

void C3DParticleObject::Update(float DeltaTime)
{
    CGameObject::Update(DeltaTime);

    if (CInput::GetInst()->GetWheelDir() && m_IsCameraZoom)
    {  
        float Length = m_ParticleArm->GetTargetDistance() +
            CInput::GetInst()->GetWheelDir() * m_CameraZoomSpeed;

        m_ParticleArm->SetTargetDistance(Length);
    }

    if (m_IsCameraRotate)
    {
        float AddedRotationY = m_CameraRotateSpeed * DeltaTime;

        if (m_RotateInv) 
            AddedRotationY *= -1;

        m_ParticleArm->AddRelativeRotationY(AddedRotationY);

        if (m_CameraRotateCallback)
        {
            float RelativeRotY = m_ParticleArm->GetRelativeRot().y;
            // 음수 
            RelativeRotY = RelativeRotY < 0 ? 360 + RelativeRotY : RelativeRotY;
            // 360 이상
            RelativeRotY = RelativeRotY > 360 ? RelativeRotY - 360 : RelativeRotY;
            m_CameraRotateCallback(RelativeRotY);
        }
    }
}
