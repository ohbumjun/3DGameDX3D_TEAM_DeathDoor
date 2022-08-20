#pragma once

#include "SceneComponentWidget.h"

class CArmComponentWidget :
    public CSceneComponentWidget
{
public:
	CArmComponentWidget();
	~CArmComponentWidget();

public:
	virtual bool Init() override;

public:
	virtual void SetSceneComponent(class CSceneComponent* Com) override;

private:
	void OnChangeTargetDist(float Val);
	void OnChangeOffset(const Vector3& Offset);

private:
	class CIMGUIInputFloat* m_TargetDist;
	class CIMGUIInputFloat3* m_Offset;
};

