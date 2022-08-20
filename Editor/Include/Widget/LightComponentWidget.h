#pragma once

#include "SceneComponentWidget.h"

class CLightComponentWidget final :
    public CSceneComponentWidget
{
public:
    CLightComponentWidget();
    virtual ~CLightComponentWidget();

public:
    virtual bool Init() override;

public:
    // 반드시 Init 이후 이 함수 호출해야 함
    // SceneComponent 상속 위젯 만들때마다 오버라이드 해야 함
    virtual void SetSceneComponent(class CSceneComponent* Com) override;

public:
    void OnEditLightColor(const Vector3& Color);
    void OnEditLightType(int Idx, const char* Label);
    void OnEditLightDist(float Value);
    void OnEditLightAngleIn(float Value);
    void OnEditLightAngleOut(float Value);
    void OnEditLightAtt1(float Value);
    void OnEditLightAtt2(float Value);
    void OnEditLightAtt3(float Value);

private:
    class CIMGUIComboBox* m_LightTypeEdit;
    class CIMGUIColor3* m_LightColorEdit;
    class CIMGUIInputFloat* m_LightDistEdit;
    class CIMGUIInputFloat* m_LightAngleInEdit;
    class CIMGUIInputFloat* m_LightAngleOutEdit;
    class CIMGUISliderFloat* m_LightAtt1Edit;
    class CIMGUISliderFloat* m_LightAtt2Edit;
    class CIMGUISliderFloat* m_LightAtt3Edit;
};

