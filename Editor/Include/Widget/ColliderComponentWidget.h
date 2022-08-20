#pragma once
#include "SceneComponentWidget.h"

class CColliderComponentWidget :
    public CSceneComponentWidget
{
public:
	CColliderComponentWidget();
	~CColliderComponentWidget();

public:
	virtual bool Init() override;

public:
	virtual void SetSceneComponent(class CSceneComponent* Com) override;

private:
	Vector3 m_Offset;
	class CIMGUITree* m_ColliderComponentTree;
	class CIMGUIInputFloat3* m_OffsetInput;
	class CIMGUIComboBox* m_CollisionProfileCombo;


private:
	void OnChangeOffset(const Vector3& Offset);
	void OnChangeProfile(int Index, const char* Label);

public:
	void RefreshCollisionProfile();
};

