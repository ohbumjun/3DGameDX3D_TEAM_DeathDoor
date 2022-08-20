#pragma once

#include "IMGUIWidgetList.h"
#include "Component/CameraComponent.h"

class CCameraMoveWidget :
    public CIMGUIWidgetList
{
public:
    CCameraMoveWidget();
    virtual ~CCameraMoveWidget();

public:
    virtual bool Init() override;

public:
    void SetCameraComponent(class CCameraComponent* Cam)
    {
        m_Cam = Cam;
    }
    
    void SetCamMoveData(CamMoveData* Data);

    void SetCameraWidget(class CCameraWidget* CamWidget)
    {
        m_CamWidget = CamWidget;
    }

public:
    void OnChangeDestPos(const Vector3& Pos);
    void OnChangeDestReachTime(float Val);
    void OnChangeStayTime(float Val);
    void OnClickDelete();

private:
    CamMoveData* m_MoveData;
    class CCameraComponent* m_Cam;
    class CCameraWidget* m_CamWidget;
    class CIMGUIInputFloat3* m_DestPos;
    class CIMGUIInputFloat* m_DestReachTime;
    class CIMGUIInputFloat* m_StayTime;
    class CIMGUIButton* m_Delete;
};

