#pragma once

#include "ColliderComponentWidget.h"

class CColliderSphereWidget :
    public CColliderComponentWidget
{
public:
	CColliderSphereWidget();
	~CColliderSphereWidget();

public:
	virtual bool Init() override;

public:
	virtual void SetSceneComponent(class CSceneComponent* Com) override;

private:
	float m_Radius;
	class CIMGUITree* m_RadiusTree;
	class CIMGUIInputFloat* m_RadiusInput;


private:
	void OnChangeRadius(float Radius);
};

