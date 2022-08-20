#pragma once

#include "IMGUIWidget.h"
#include "ImGuizmo.h"
#include "GameObject\GameObject.h"

class CIMGUIGizmo :
	public CIMGUIWidget
{
public:
	CIMGUIGizmo();
	~CIMGUIGizmo();

public:
	virtual bool Init() override;
	virtual void Render() override;

public:
	void SetGameObject(class CGameObject* Object);

	void SetComponent(class CSceneComponent* Component);

	void SetOperationMode(ImGuizmo::OPERATION Operation)
	{
		m_Operation = Operation;
	}

	void SetMode(ImGuizmo::MODE Mode)
	{
		m_Mode = Mode;
	}

	void SetIdentity()
	{
		m_matWorld.Identity();
		m_Component = nullptr;
	}

	void OnDestroyComponent(class CSceneComponent* Component);

private:
	Matrix m_matWorld;

	ImGuizmo::OPERATION m_Operation;
	ImGuizmo::MODE m_Mode;

	class CSceneComponent* m_Component;

	bool m_Using;
};

