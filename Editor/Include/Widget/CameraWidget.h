#pragma once

#include "SceneComponentWidget.h"
#include "Component/CameraComponent.h"

class CCameraWidget :
    public CSceneComponentWidget
{
public:
	CCameraWidget();
	~CCameraWidget();

public:
	virtual bool Init() override;

public:
	virtual void SetSceneComponent(class CSceneComponent* Com) override;

public:
	void OnDeleteMoveData(CamMoveData* Data, class CCameraMoveWidget* DeleteWidget);

private:
	void OnChangeViewAngle(float Val);
	void OnClickSetCurrent();
	void OnClickShake();
	void OnCheckMoveDataEditMode(int Idx, bool Enable);
	void OnClickAddMoveData();
#ifdef _DEBUG
	void OnClickTestMove();
#endif // DEBUG
	void OnClickClearMove();

private:
	class CIMGUIInputFloat* m_ViewAngle;
	class CIMGUIButton* m_SetCurrent;
	class CIMGUIInputFloat* m_ShakeTime;
	class CIMGUIInputFloat* m_ShakeAmount;
	class CIMGUIButton* m_Shake;

	bool m_IsMoveEditMode;
	Vector3 m_OriginCamPosRelative;
	class CIMGUICheckBox* m_MoveDataEditMode;
	class CIMGUIButton* m_AddMoveData;
	class CIMGUIWidgetList* m_MoveDataList;
	class CIMGUIButton* m_MoveTest;
	class CIMGUIButton* m_ClearMoveData;
};

