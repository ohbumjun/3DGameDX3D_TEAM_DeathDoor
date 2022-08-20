#pragma once

#include "SceneComponentWidget.h"

class CWaterComponentWidget :
    public CSceneComponentWidget
{
public:
    CWaterComponentWidget();
    virtual ~CWaterComponentWidget();

public:
    virtual bool Init() override;
    
public:
    virtual void SetSceneComponent(class CSceneComponent* Com) override;

private:
    void OnClickLoadMaterial();
    void OnEditBaseColor(const Vector3& Color);
    void OnEditAmbientColor(const Vector3& Color);
    void OnEditSpecluarColor(const Vector3& Color);
    void OnEditSpecluarPower(float Power);
    void OnEditEmissiveColor(const Vector3& Color);
    void OnChangeSpeed(float Val);
    void OnChangeFoamDepth(float Val);
    void OnChangeAttn1(float Val);
    void OnChangeAttn2(float Val);
    void OnDropMaterialName(const std::string& Name);

private:
    class CIMGUITextInput* m_MatName;
    class CIMGUIButton* m_LoadButton;
    class CIMGUIInputFloat* m_Speed;
    class CIMGUIInputFloat* m_FoamDepth;
    class CIMGUIInputFloat* m_Attn1;
    class CIMGUIInputFloat* m_Attn2;
    class CIMGUIColor3* m_BaseColorEdit;
    class CIMGUIColor3* m_AmbientColorEdit;
    class CIMGUIColor3* m_SpecularColorEdit;
    class CIMGUIInputFloat* m_SpecluarPowerEdit;
    class CIMGUIColor3* m_EmissiveColorEdit;
};

