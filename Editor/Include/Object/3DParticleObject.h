#pragma once
#include "GameObject\GameObject.h"
#include "Component/Arm.h"

class C3DParticleObject :
    public CGameObject
{
	friend class CScene;
	friend class CEffectEditor;

protected:
	C3DParticleObject();
	C3DParticleObject(const C3DParticleObject& obj);
	virtual ~C3DParticleObject();
private :
	class CParticleComponent* m_ParticleComponent;
	class CCameraComponent* m_ParticleCamera;
	class CArm* m_ParticleArm;
	std::function<void(float)> m_CameraRotateCallback;
private :
	bool m_IsCameraRotate;
	bool m_IsCameraZoom;
	float m_CameraRotateSpeed;
	float m_CameraZoomSpeed;
	bool m_RotateInv;
public :
	class CArm* GetArmComponent() const
	{
		return m_ParticleArm;
	}
	class CCameraComponent* GetCameraComponent() const
	{
		return m_ParticleCamera;
	}
	float GetCameraRotateSpeed() const
	{
		return m_CameraRotateSpeed;
	}
	bool IsCameraRotate() const
	{
		return m_IsCameraRotate;
	}
	bool IsCameraRotateInv() const
	{
		return m_RotateInv;
	}
	float GetCameraZoomSpeed() const
	{
		return m_CameraZoomSpeed;
	}
	bool IsCameraZoom() const
	{
		return m_IsCameraZoom; 
	}
	Vector3 GetCameraRelativeRotation() const
	{
		return m_ParticleArm->GetRelativeRot();
	}
	const Vector3& GetCameraOfffset() const
	{
		return m_ParticleArm->GetOffset();
	}
	float GetCameraYOffset() const
	{
		return m_ParticleArm->GetOffset().y;
	}
public :
	void SetRotateInv(bool Enable)
	{
		m_RotateInv = Enable;
	}
	void SetCameraZoom(bool Enable) 
	{
		m_IsCameraZoom = Enable;
	}
	void SetCameraRotate(bool Enable) 
	{
		m_IsCameraRotate = Enable;
	}
	void SetCameraRotateSpeed(float Speed)
	{
		m_CameraRotateSpeed = Speed;
	}
	void SetCameraZoomSpeed(float Speed)
	{
		m_CameraZoomSpeed = Speed;
	}
	void SetYOffset(float YOffset)
	{
		const Vector3& ArmOffset = m_ParticleArm->GetOffset();
		m_ParticleArm->SetOffset(ArmOffset.x, YOffset, ArmOffset.y);
	}
	void SetRelativeRotationXPos (float XRot)
	{
		const Vector3& RelativeRotation = m_ParticleArm->GetRelativeRot();
		m_ParticleArm->SetRelativeRotation(XRot, RelativeRotation.y, RelativeRotation.z);
	}
	void SetRelativeRotationY(float YRot)
	{
		const Vector3& RelativeRotation = m_ParticleArm->GetRelativeRot();
		m_ParticleArm->SetRelativeRotation(RelativeRotation.x, YRot, RelativeRotation.z);
	}
public:
	virtual bool Init() override;
	virtual void Start() override;
	virtual void Update(float DeltaTime) override;
public :
	template<typename T>
	void SetCameraRotateCallback(T* Obj, void(T::*Func)(float Val))
	{
		m_CameraRotateCallback = std::bind(Func, Obj, std::placeholders::_1);
	}
};
