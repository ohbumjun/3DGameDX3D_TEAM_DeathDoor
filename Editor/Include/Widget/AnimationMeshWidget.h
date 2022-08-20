#pragma once

#include "SceneComponentWidget.h"

class CAnimationMeshWidget :
    public CSceneComponentWidget
{
public:
    CAnimationMeshWidget();
    virtual ~CAnimationMeshWidget();
private :
    // Anim Clip Table
    class CIMGUITableElemList* m_AnimInfoTable;
    class CIMGUITextInput* m_CurrentAnimSequence;

    // Anim Instance
    class CIMGUIButton* m_LoadAnimInstanceBtn;
    class CIMGUIButton* m_SaveAnimInstanceBtn;

    // Load Animation Mesh
    //class CIMGUIButton* m_LoadAnimationMeshBtn;
    
    // Seq Name
    class CIMGUITextInput* m_NewNameInput;
    class CIMGUIButton* m_ReNameSequenceBtn;
private :
    class CAnimationSequenceInstance* m_Animation;
    std::string m_LoadedMeshName;
public:
    virtual bool Init() override;
    
public:
    virtual void SetSceneComponent(class CSceneComponent* Com) override;

private:
    void OnClickLoadMesh();
    void OnSelectMaterialSlotCombo(int Idx, const char* Label);
    void OnEditBaseColor(const Vector3& Color);
    void OnEditAmbientColor(const Vector3& Color);
    void OnEditSpecluarColor(const Vector3& Color);
    void OnEditSpecluarPower(float Power);
    void OnEditEmissiveColor(const Vector3& Color);
    void OnCheckTransparency(int Idx, bool Check);
    void OnEditOpacity(float Opacity);
    void OnCheckMetallic(int Idx, bool Check);

private :
    void OnLoadAnimationInstance();
    void OnSaveAnimationInstance();
    void OnEditAnimationKeyName();
    void OnRefreshAnimationInfo();
    void OnDropShaderName(const std::string& Name);

public :
    void SetAnimationRelatedInfoToWidget(class CAnimationSequenceInstance* AnimationInstance);
    bool LoadElementsForSqcLoading();
    void ClearExistingAnimationSeqInfos();

private:
    void RefreshMeshWidget(class CMesh* Mesh);
    bool MakeShaderWidget(class CMaterial* Mat, const std::string& ShaderName);

private:
    class CIMGUICollapsingHeader* m_ShaderWidgetTree;
    class CShaderWidget* m_ShaderWidget;

    class CIMGUITextInput* m_MeshName;
    class CIMGUIButton* m_LoadMeshButton;

    class CIMGUIComboBox* m_MaterialSlotCombo;
    class CIMGUITextInput* m_ShaderName;
    class CIMGUIColor3* m_BaseColorEdit;
    class CIMGUIColor3* m_AmbientColorEdit;
    class CIMGUIColor3* m_SpecularColorEdit;
    class CIMGUIInputFloat* m_SpecluarPowerEdit;
    class CIMGUIColor3* m_EmissiveColorEdit;
    class CIMGUICheckBox* m_TransparencyEdit;
    class CIMGUISliderFloat* m_OpacityEdit;
    class CIMGUICheckBox* m_Metallic;
};

