#pragma once

#include "GameObject/GameObject.h"

class C3DCameraObject :
    public CGameObject
{
	friend class CScene;

protected:
	C3DCameraObject();
	C3DCameraObject(const C3DCameraObject& obj);
	virtual ~C3DCameraObject();

public:
	bool Init() override;
	void Start() override;
	void Update(float DeltaTime) override;

public:
	float GetCameraSpeed() const
	{
		return m_Speed;
	}

	void SetCameraSpeed(float Speed)
	{
		m_Speed = Speed;
	}

private:
	void OnDragMoveStart(float DeltaTime);
	void OnDragMove(float DeltaTime);
	void OnDragMoveEnd(float DeltaTime);
	void OnDragRotate(float DeltaTime);
	void OnZoomIn(float DetlaTime);
	void OnZoomOut(float DetlaTime);

private:
	CSharedPtr<class CSceneComponent> m_FocusPoint;
	CSharedPtr<class CCameraComponent> m_Camera;

	Vector3 m_MoveStartCamPos;
	float m_Speed;
};

