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
    // 반드시 Init 이후 이 함수 호출해야 함
    // SceneComponent 상속 위젯 만들때마다 오버라이드 해야 함
    virtual void SetSceneComponent(class CSceneComponent* Com) override;
private :
    class CIMGUITextInput* m_LoadedParticleName;
    class CIMGUITextInput* m_LoadedParticleFileName;
    class CIMGUIButton* m_LoadParticleButton;
    class CIMGUIButton* m_SaveParticleButton;
    class CIMGUIButton* m_ResetParticleButton;
private: // Material Emissive 수정
    class CIMGUIColor3* m_BaseColorEdit;
private : // 시간에 따른 Emissive 변화 정보 수정 기능
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
    // 다시 Particle 세팅하기
    void OnResetParticleInfo();
    void OnSetBillBoardEffectEdit(const char*, bool Enable);
    // HelperFunction
private :
    void ParticleLoadSuccessCallback(class CParticle* LoadedParticle);
    void SetParticleClassToParticleComponent(class CParticle* Particle);
    void ReflectParticleToIMGUI();
};

// Particle 기본 세팅
// void OnSetParticleMaterialSetting(CSceneComponent* Com);
// void OnReflectCurrentParticleSetting();


// 기존 IMGUI ---
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

// 기존 세팅 코드 ---
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