#pragma once

#include "../GameInfo.h"
#include "../Component/CameraComponent.h"

class CCameraManager
{
	friend class CScene;

private:
	CCameraManager();
	~CCameraManager();

private:
	class CScene* m_Scene;

private:
	CSharedPtr<CCameraComponent>	m_CurrentCamera;
	CSharedPtr<CCameraComponent>	m_KeepCamera;
	CSharedPtr<CCameraComponent>	m_UICamera;
private :
	CSharedPtr<CCameraComponent>	m_AnimationEditorCamera;
	CSharedPtr<CCameraComponent>	m_ParticleEffectEditorCamera;

public:
	void ShakeCamera(float Time, float Amount);

public:
	CCameraComponent* GetCurrentCamera()	const
	{
		return m_CurrentCamera;
	}

	CCameraComponent* GetUICamera()	const
	{
		return m_UICamera;
	}

	CCameraComponent* GetAnimationEditorCamera()	const
	{
		return m_AnimationEditorCamera;
	}

	CCameraComponent* GetParticleEffectEditorCamera()	const
	{
		return m_ParticleEffectEditorCamera;
	}

	void SetCurrentCamera(CCameraComponent* Camera)
	{
		m_CurrentCamera = Camera;
	}

	void SetAnimationEditorCamera(CCameraComponent* Camera)
	{
		m_AnimationEditorCamera = Camera;
	}

	void SetParticleEditorCamera(CCameraComponent* Camera)
	{
		m_ParticleEffectEditorCamera = Camera;
	}

	void KeepCamera()
	{
		m_KeepCamera = m_CurrentCamera;
	}

	void ReturnCamera()
	{
		m_CurrentCamera = m_KeepCamera;

		m_KeepCamera = nullptr;
	}

public:
	void Start();
	void Init();
	void Update(float DeltaTime);
	void PostUpdate(float DeltaTime);
};

