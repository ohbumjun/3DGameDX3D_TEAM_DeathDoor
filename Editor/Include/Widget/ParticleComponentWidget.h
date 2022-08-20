#pragma once
#include "SceneComponentWidget.h"

class CParticleComponentWidget :
    public CSceneComponentWidget
{
public:
    CParticleComponentWidget();
    virtual ~CParticleComponentWidget();
public:
    virtual bool Init() override;
public:
    // �ݵ�� Init ���� �� �Լ� ȣ���ؾ� ��
    // SceneComponent ��� ���� ���鶧���� �������̵� �ؾ� ��
    virtual void SetSceneComponent(class CSceneComponent* Com) override;
private :
    class CIMGUITextInput* m_LoadedParticleName;
    class CIMGUITextInput* m_LoadedParticleFileName;
    class CIMGUIButton* m_LoadParticleButton;
    class CIMGUIButton* m_SaveParticleButton;
    class CIMGUIButton* m_ResetParticleButton;
private: // Material Emissive ����
    class CIMGUIColor3* m_BaseColorEdit;
private : // �ð��� ���� Emissive ��ȭ ���� ���� ���
    class CIMGUIColor3* m_StartEmissiveColorEdit;
    class CIMGUIColor3* m_EndEmissiveColorEdit;
    class CIMGUICheckBox* m_ApplyLinearEmissiveChangeEdit;
    class CIMGUICheckBox* m_BillBoardEnableEdit;
private:
    class CParticle* m_ParticleClass;
private :
    void OnEditBaseColor(const Vector3& Color);
    void OnEditStartEmissiveColor(const Vector3& Color);
    void OnEditEndEmissiveColor(const Vector3& Color);
    void OnEditEmissiveLinearCheck(const char*, bool Check);
private :
    void OnLoadParticleClass();
    void OnSaveParticleClass();
    void OnDropParticleToParticleWidget(const std::string& InputName);
    // �ٽ� Particle �����ϱ�
    void OnResetParticleInfo();
    void OnSetBillBoardEffectEdit(const char*, bool Enable);
    // HelperFunction
private :
    void ParticleLoadSuccessCallback(class CParticle* LoadedParticle);
    void SetParticleClassToParticleComponent(class CParticle* Particle);
    void ReflectParticleToIMGUI();
};

// Particle �⺻ ����
// void OnSetParticleMaterialSetting(CSceneComponent* Com);
// void OnReflectCurrentParticleSetting();


// ���� IMGUI ---
 // class CIMGUIInputFloat* m_SpawnTimeMaxEdit;
// class CIMGUIInputFloat3* m_StartMinEdit;
// class CIMGUIInputFloat3* m_StartMaxEdit;
// class CIMGUIInputInt* m_SpawnCountMaxEdit;
// class CIMGUIInputFloat3* m_ScaleMinEdit;
// class CIMGUIInputFloat3* m_ScaleMaxEdit;
// class CIMGUIInputFloat* m_LifeTimeMinEdit;
// class CIMGUIInputFloat* m_LifeTimeMaxEdit;
// class CIMGUIInputFloat* m_SpeedMinEdit;
// class CIMGUIInputFloat* m_SpeedMaxEdit;
// class CIMGUIColor4* m_ColorMinEdit;
// class CIMGUIColor4* m_ColorMaxEdit;
// class CIMGUICheckBox* m_IsMoveEdit;
// class CIMGUICheckBox* m_IsGravityEdit;
// class CIMGUICheckBox* m_IsRandomMoveEdit;
// class CIMGUIInputFloat3* m_MoveDirEdit;
// class CIMGUIInputFloat3* m_MoveAngleEdit;
// class CIMGUIButton* m_SetMaterialTextureButton;
// class CIMGUIImage* m_ParticleTexture;

// ���� ���� �ڵ� ---
// void OnSpawnTimeMaxEdit(float Num);
// void OnStartMinEdit(const Vector3&);
// void OnStartMaxEdit(const Vector3&);
// void OnSpawnCountMaxEdit(int Num);
// void OnScaleMinEdit(const Vector3&);
// void OnScaleMaxEdit(const Vector3&);
// void OnLifeTimeMinEdit(float Num);
// void OnLifeTimeMaxEdit(float Num);
// void OnSpeedMinEdit(float Num);
// void OnSpeedMaxEdit(float Num);
// void OnColorMinEdit(const Vector4& Color);
// void OnColorMaxEdit(const Vector4& Color);
// void OnIsMoveEdit(const char*, bool);
// void OnIsGravityEdit(const char*, bool);
// void OnIsRandomMoveEdit(const char*, bool);
// void OnMoveDirEdit(const Vector3& Dir);
// void OnMoveAngleEdit(const Vector3& Angle);
// void OnSetParticleTexture();