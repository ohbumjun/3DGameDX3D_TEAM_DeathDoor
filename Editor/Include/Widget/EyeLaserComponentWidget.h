#pragma once

#include "SceneComponentWidget.h"

class CEyeLaserComponentWidget :
    public CSceneComponentWidget
{
public:
    CEyeLaserComponentWidget();
    virtual ~CEyeLaserComponentWidget();

public:
    virtual bool Init() override;

private:
    class CIMGUIButton* m_MaterialTextureChangeButton;

    class CIMGUIComboBox* m_MaterialSlotCombo;
    class CIMGUITextInput* m_MeshName;
    class CIMGUITextInput* m_ShaderName;
    class CIMGUIColor3* m_BaseColorEdit;

    class CIMGUIColor3* m_EmissiveColorEdit;
    class CIMGUICheckBox* m_TransparencyEdit;
    class CIMGUISliderFloat* m_OpacityEdit;

public:
    virtual void SetSceneComponent(class CSceneComponent* Com) override;

private:
    void OnEditBaseColor(const Vector3& Color);
    void OnEditEmissiveColor(const Vector3& Color);
    void OnCheckTransparency(int Idx, bool Check);
    void OnEditOpacity(float Opacity);
    void OnChangeMaterialTexture();
    void OnSelectMaterialSlotCombo(int Idx, const char* Label);

private:
    void RefreshMeshWidget(class CMesh* Mesh);
};

