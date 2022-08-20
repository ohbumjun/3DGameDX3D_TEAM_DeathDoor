// IMGUI
#include "IMGUITextInput.h"
#include "IMGUIInputFloat.h"
#include "IMGUIInputInt.h"
#include "IMGUIInputFloat3.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "../EditorInfo.h"
#include "../EditorUtil.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUIImage.h"
#include "IMGUISliderFloat.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "IMGUIManager.h"
#include "IMGUIColor3.h"
#include "IMGUIColor4.h"
#include "IMGUIInputFloat.h"
#include "IMGUIComboBox.h"
#include "IMGUIDummy.h"
#include "IMGUITree.h"
// Window
#include "EffectEditor.h"
#include "../EditorManager.h"
#include "../Window/ResourceDisplayWindow.h"
// Object
#include "../Object/3DParticleObject.h"
// Engine
#include "EngineUtil.h"
#include "Component/ParticleComponent.h"
#include "Component/SpriteComponent.h"
#include "Component/Arm.h"
#include "Engine.h"
#include "PathManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Render/RenderManager.h"
#include "Resource/Particle/ParticleManager.h"
#include "Scene/SceneResource.h"
#include "GameObject/SkyObject.h"

CEffectEditor::CEffectEditor() :
    m_StartEdit(false),
    m_ParticleClass(nullptr)
{
}

CEffectEditor::~CEffectEditor()
{
}

bool CEffectEditor::Init()
{
    // Save, Load
    m_SaveParticleBtn = AddWidget<CIMGUIButton>("Save Particle", 90.f, 20.f);
    m_SaveParticleBtn->SetClickCallback<CEffectEditor>(this, &CEffectEditor::OnSaveParticleClass);

    CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(105);

    m_LoadParticleBtn = AddWidget<CIMGUIButton>("Load Particle", 90.f, 20.f);
    m_LoadParticleBtn->SetClickCallback<CEffectEditor>(this, &CEffectEditor::OnLoadParticleClass);

    Line = AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(210);

    CIMGUIText* HelpText = AddWidget<CIMGUIText>("ParticleSaveLoad", 90.f, 30.f);
    const char* ParticleSaveLoadText = R"(ex) Particle �� ���, Bin/ParticleClass ������ Save Ȥ��, Bin/ParticleClass �κ��� Load �ؾ� �Ѵ�.)";
    HelpText->SetText(ParticleSaveLoadText);
    HelpText->SetIsHelpMode(true);

    m_StartEditBtn = AddWidget<CIMGUIButton>("Start Edit", 90.f, 20.f);
    m_StartEditBtn->SetClickCallback<CEffectEditor>(this, &CEffectEditor::SetStartEditing);

    Line = AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(105);

    m_RestartBtn = AddWidget<CIMGUIButton>("ReStart", 90.f, 20.f);
    m_RestartBtn->SetClickCallback<CEffectEditor>(this, &CEffectEditor::OnRestartParticleComponentButton);

    // Particle Name
    m_CurrentParticleName = AddWidget<CIMGUITextInput>("Particle Name", 180.f, 30.f);
    m_CurrentParticleName->SetHintText("Current Particle Name");

    // Material
    m_CurrentMaterialName = AddWidget<CIMGUITextInput>("Current Mtrl Name(Drop)", 180.f, 30.f);
    m_CurrentMaterialName->SetHintText("Drop Here");
    m_CurrentMaterialName->SetDropCallBack<CEffectEditor>(this, &CEffectEditor::OnDropMaterialToParticle);

    Line = AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(350.f);

    HelpText = AddWidget<CIMGUIText>("MtrlFileName", 90.f, 30.f);
    const char* DropMtrlText = R"(ex) ResourceDisplayWindow �κ��� Material Key �� Drop �ϰų�,
    Bin/Material �� �ִ� .mtrl ������ Drop �ϸ�, �ش� Material �� ���õȴ�.)";
    HelpText->SetText(DropMtrlText);
    HelpText->SetIsHelpMode(true);

    m_LoadedMaterialFileName = AddWidget<CIMGUITextInput>("Loaded Mtrl File", 180.f, 30.f);
    m_LoadedMaterialFileName->SetHintText("Not Loaded From Disk");

    Line = AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(290.f);

    HelpText = AddWidget<CIMGUIText>("MtrlFileName", 90.f, 30.f);
    const char* LoadedMtrlHelpText = R"(ex)  �ϵ��ũ�κ���, Material File�� Load �Ͽ� ������ ��� 
    Load�� Material File�� �̸��� �����ش�.)";
    HelpText->SetText(LoadedMtrlHelpText);
    HelpText->SetIsHelpMode(true);

    m_MaterialLoadButton = AddWidget<CIMGUIButton>("Load/Set Material", 180.f, 20.f);
    m_MaterialLoadButton->SetClickCallback<CEffectEditor>(this, &CEffectEditor::OnLoadParticleMaterialCallback);

    // Generate Radius
    m_GenerateRadius = AddWidget<CIMGUISliderFloat>("Radius", 100.f, 20.f);
    m_GenerateRadius->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnEditGenerateRadius);
    m_GenerateRadius->SetMin(0.0f);
    m_GenerateRadius->SetMax(100.f);

    // SpawnCount
    m_SpawnCountMaxEdit = AddWidget<CIMGUIInputInt>("Init Spawn Count", 200.f);
    m_SpawnCountMaxEdit->SetVal(1000); // ó������ 1000���� ����

    // Camera
    CIMGUITree* Tree = AddWidget<CIMGUITree>("Camera");

    // Rotate
    m_IsRotateEdit = Tree->AddWidget<CIMGUICheckBox>("Rotate", 80.f);
    m_IsRotateEdit->AddCheckInfo("Rotate");
    m_IsRotateEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsCameraRotateEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(100.f);

    m_IsRotateInv = Tree->AddWidget<CIMGUICheckBox>("Inv", 80.f);
    m_IsRotateInv->AddCheckInfo("Inv");
    m_IsRotateInv->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnCameraRotateInvEdit);

    m_RotateSpeedSliderBar = Tree->AddWidget<CIMGUISliderFloat>("Rotate Speed", 100.f, 20.f);
    m_RotateSpeedSliderBar->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnSetCameraRotateSpeed);
    m_RotateSpeedSliderBar->SetMin(10.f);
    m_RotateSpeedSliderBar->SetMax(90.f);

    m_YRotateSliderBar = Tree->AddWidget<CIMGUISliderFloat>("Y Axis Rotate", 100.f, 20.f);
    m_YRotateSliderBar->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnSetCameraYAxisRotate);
    m_YRotateSliderBar->SetMin(0.f);
    m_YRotateSliderBar->SetMax(360.f);

    // Zoom
    m_IsZoomEdit = Tree->AddWidget<CIMGUICheckBox>("Zoom", 80.f);
    m_IsZoomEdit->AddCheckInfo("Zoom");
    m_IsZoomEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsCameraZoomEdit);

    m_ZoomSpeedSliderBar = Tree->AddWidget<CIMGUISliderFloat>("Zoom Speed", 100.f, 20.f);
    m_ZoomSpeedSliderBar->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnSetCameraZoomSpeed);
    m_ZoomSpeedSliderBar->SetMin(0.2f);
    m_ZoomSpeedSliderBar->SetMax(50.f);

    m_CameraYOffsetBar = Tree->AddWidget<CIMGUISliderFloat>("Camera Y Offset", 100.f, 20.f);
    m_CameraYOffsetBar->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnSetCameraYOffset);
    m_CameraYOffsetBar->SetMin(-50.f);
    m_CameraYOffsetBar->SetMax(50.f);

    m_CameraXRotSlideBar = Tree->AddWidget<CIMGUISliderFloat>("Camera X Rot", 100.f, 20.f);
    m_CameraXRotSlideBar->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnSetCameraXRot);
    m_CameraXRotSlideBar->SetMin(-88.f);
    m_CameraXRotSlideBar->SetMax(88.f);

    // Particle Texture
    m_ParticleTexture = AddWidget<CIMGUIImage>("Particle Texture", 200.f, 200.f);
    m_ParticleTexture->SetBorderColor(10, 10, 255);
    m_ParticleTexture->SetTableTitle("Texture");

    // Preset
    m_ParticlePreset = AddWidget<CIMGUIComboBox>("Preset", 300.f, 30.f);
    m_ParticlePreset->SetHideName(true);
    m_ParticlePreset->SetSelectCallback<CEffectEditor>(this, &CEffectEditor::OnClickParticlePreset);

    // Preset Setting
    m_ParticlePreset->AddItem("Select Preset");
    for (int i = 0; i < (int)ParticlePreset::Max; ++i)
    {
        m_ParticlePreset->AddItem(ParticlePresetNames[i]);
    }
    m_ParticlePreset->SetSelectIndex(0);


    // UV Move
    Tree = AddWidget<CIMGUITree>("UV Move");
    m_IsUVMoveEnableEdit = Tree->AddWidget<CIMGUICheckBox>("UV Move", 80.f);
    m_IsUVMoveEnableEdit->AddCheckInfo("UVMove");
    m_IsUVMoveEnableEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsUVMoveEnableEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(100.f);

    HelpText = Tree->AddWidget<CIMGUIText>("UVMoveHelp", 90.f, 30.f);
    const char* UVMoveText = R"(Texture ������ UV Move ȿ��)";
    HelpText->SetText(UVMoveText);
    HelpText->SetIsHelpMode(true);

    m_UVRowN = Tree->AddWidget<CIMGUIInputInt>("UV RowN", 150.f);
    m_UVRowN->SetCallBack(this, &CEffectEditor::OnUVRowNEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(250.f);

    HelpText = Tree->AddWidget<CIMGUIText>("UVRowN", 90.f, 30.f);
    const char* UVRowNText = R"(Texture Image �� ����)";
    HelpText->SetText(UVRowNText);
    HelpText->SetIsHelpMode(true);

    m_UVColN = Tree->AddWidget<CIMGUIInputInt>("UV ColN", 150.f);
    m_UVColN->SetCallBack(this, &CEffectEditor::OnUVColNEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(250.f);

    HelpText = Tree->AddWidget<CIMGUIText>("UVColN", 90.f, 30.f);
    const char* UVColNext = R"(Texture Image �� ����)";
    HelpText->SetText(UVColNext);
    HelpText->SetIsHelpMode(true);

    // BaseTexture
    Tree = AddWidget<CIMGUITree>("Ground Texture");
    m_GroundTextureScale =  Tree->AddWidget<CIMGUISliderFloat>("Ground Texture", 100.f, 20.f);
    m_GroundTextureScale->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnEditBaseGroundSize);
    m_GroundTextureScale->SetMin(10.f);
    m_GroundTextureScale->SetMax(800.f);

    // Bounce
    Tree = AddWidget<CIMGUITree>("Bounce");

    m_IsBounce = Tree->AddWidget<CIMGUICheckBox>("Bounce", 80.f);
    m_IsBounce->AddCheckInfo("Bounce");
    m_IsBounce->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsBounceEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(100.f);

    HelpText = Tree->AddWidget<CIMGUIText>("BounceHelp", 90.f, 30.f);
    const char* BounceHelpText = R"(ź�� ȿ��)";
    HelpText->SetText(BounceHelpText);
    HelpText->SetIsHelpMode(true);

    m_BounceResistance = Tree->AddWidget<CIMGUISliderFloat>("Bounce Resist", 100.f, 20.f);
    m_BounceResistance->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnEditBounceResistance);
    m_BounceResistance->SetMin(0.01f);
    m_BounceResistance->SetMax(0.99f);

    // Rotation Angle
    Tree = AddWidget<CIMGUITree>("Rotation Angle");

    m_MinSeperateRotAngleEdit = Tree->AddWidget<CIMGUIInputFloat3>("Min Angle", 150.f);
    m_MinSeperateRotAngleEdit->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnMinSeperateRotAngleEdit);

    m_MaxSeperateRotAngleEdit = Tree->AddWidget<CIMGUIInputFloat3>("Max Angle", 150.f);
    m_MaxSeperateRotAngleEdit->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnMaxSeperateRotAngleEdit);

    // Particle Shape
    Tree = AddWidget<CIMGUITree>("ParticleShape");

    m_ParticleShapeType = Tree->AddWidget<CIMGUIComboBox>("Shape", 150.f);
    m_ParticleShapeType->SetSelectCallback<CEffectEditor>(this, &CEffectEditor::OnClickParticleShape);

    m_ParticleShapeType->AddItem("Select Shape");
     for (int i = 0; i < (int)ParitcleShapeType::Max; ++i)
     {
         m_ParticleShapeType->AddItem(ParticleShapeNames[i]);
     }
    m_ParticleShapeType->SetSelectIndex(0);

    m_IsLoopGenerateRing = Tree->AddWidget<CIMGUICheckBox>("Loop", 80.f);
    m_IsLoopGenerateRing->AddCheckInfo("Ring Loop");
    m_IsLoopGenerateRing->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsLoopGenerateRingEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(120.f);

    // ���� ���⿡ ���� ȸ�� ���� -> ��, �ش� ������, m_SpecialMoveDirType �� ��ȿ�ϰ� ���õǾ�߸� �����ϴ�.
    m_RotateAccordingToDir = Tree->AddWidget<CIMGUICheckBox>("Rot To Dir", 80.f);
    m_RotateAccordingToDir->AddCheckInfo("Rot To Dir");
    m_RotateAccordingToDir->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsLinearRot);


    // Movement
    Tree = AddWidget<CIMGUITree>("Movement");

    m_IsMoveEdit = Tree->AddWidget<CIMGUICheckBox>("Move", 80.f);
    m_IsMoveEdit->AddCheckInfo("Move");
    m_IsMoveEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsMoveEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(90.f);

    m_IsGravityEdit = Tree->AddWidget<CIMGUICheckBox>("Gravity", 80.f);
    m_IsGravityEdit->AddCheckInfo("Gravity");
    m_IsGravityEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsGravityEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(180.f);

    m_IsFollowComponentWorldPosEdit = Tree->AddWidget<CIMGUICheckBox>("Follow Component", 80.f);
    m_IsFollowComponentWorldPosEdit->AddCheckInfo("Follow Component");
    m_IsFollowComponentWorldPosEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsFollowComponentPosEdit);

    // Spawn Time, Spawn Count
    Tree  = AddWidget<CIMGUITree>("Spawn Time, Disable Alive");

    m_SpawnTimeMaxEdit = Tree->AddWidget<CIMGUIInputFloat>("Spawn Time", 150.f);
    m_SpawnTimeMaxEdit->SetCallBack(this, &CEffectEditor::OnSpawnTimeMaxEdit);

    // 
    m_DisableNewAliveEdit = Tree->AddWidget<CIMGUICheckBox>("Disable New Alive", 150.f);
    m_DisableNewAliveEdit->AddCheckInfo("Disable New Alive");
    m_DisableNewAliveEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnDisableNewAlive);
    m_DisableNewAliveEdit->SetCheck(0, false); // ó������ false �� ���õǾ� �ִ�.

    // _ApplyDuringDisableNewAliveEdit = Tree->AddWidget<CIMGUICheckBox>("Apply Spawn Time While Disabled", 250.f);
    // _ApplyDuringDisableNewAliveEdit->AddCheckInfo("Apply Spawn Time While Disabled");
    // _ApplyDuringDisableNewAliveEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnApplySpawnTimeWhileDisableNewAlive);
    // _ApplyDuringDisableNewAliveEdit->SetCheck(0, false); // ó������ false �� ���õǾ� �ִ�.

    // Start Min, Start Max
    Tree = AddWidget<CIMGUITree>("Start Min, Max");

    m_StartMinEdit = Tree->AddWidget<CIMGUIInputFloat3>("StartMin", 150.f);
    m_StartMinEdit->SetCallBack(this, &CEffectEditor::OnStartMinEdit);

    m_StartMaxEdit = Tree->AddWidget<CIMGUIInputFloat3>("StartMax", 150.f);
    m_StartMaxEdit->SetCallBack(this, &CEffectEditor::OnStartMaxEdit);

    // Scale Min, Start Max
    Tree = AddWidget<CIMGUITree>("Scale Min, Max");

    m_ScaleMinEdit = Tree->AddWidget<CIMGUIInputFloat3>("ScaleMin", 150.f);
    m_ScaleMinEdit->SetCallBack(this, &CEffectEditor::OnScaleMinEdit);

    m_ScaleMaxEdit = Tree->AddWidget<CIMGUIInputFloat3>("ScaleMax", 150.f);
    m_ScaleMaxEdit->SetCallBack(this, &CEffectEditor::OnScaleMaxEdit);

    // LifeTime Min, Max
    Tree = AddWidget<CIMGUITree>("LifeTime Min, Max");

    m_IsLifeTimeLinearFromCenterEdit = Tree->AddWidget<CIMGUICheckBox>("LifeTimeLinear", 80.f);
    m_IsLifeTimeLinearFromCenterEdit->AddCheckInfo("LifeTimeLinear");
    m_IsLifeTimeLinearFromCenterEdit->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsLifeTimeLinearFromCenter);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(140.f);

    HelpText = Tree->AddWidget<CIMGUIText>("LifeTimeLinear", 90.f, 30.f);
    const char* LifeTimeLinearText = R"(������� �־������� LifeTime ����)";
    HelpText->SetText(LifeTimeLinearText);
    HelpText->SetIsHelpMode(true);

    m_IsLifeTimeLinearFromCenterEdit->SetCheck(0, true);

    m_LifeTimeMinEdit = Tree->AddWidget<CIMGUIInputFloat>("Life T Min", 150.f);
    m_LifeTimeMinEdit->SetCallBack(this, &CEffectEditor::OnLifeTimeMinEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(250.f);

    m_LifeTimeMaxEdit = Tree->AddWidget<CIMGUIInputFloat>("Life T Max", 150.f);
    m_LifeTimeMaxEdit->SetCallBack(this, &CEffectEditor::OnLifeTimeMaxEdit);

    // Speed Start End
    Tree = AddWidget<CIMGUITree>("Speed Start, End");

    m_SpeedStartEdit = Tree->AddWidget<CIMGUIInputFloat>("Speed Start", 150.f);
    m_SpeedStartEdit->SetCallBack(this, &CEffectEditor::OnSpeedStartEdit);

    Line = Tree->AddWidget<CIMGUISameLine>("Line");
    Line->SetOffsetX(250.f);

    m_SpeedEndEdit = Tree->AddWidget<CIMGUIInputFloat>("Speed End", 150.f);
    m_SpeedEndEdit->SetCallBack(this, &CEffectEditor::OnSpeedEndEdit);

    m_SpeedChangeType = Tree->AddWidget<CIMGUIComboBox>("SpeedChangeType", 200.f);
    m_SpeedChangeType->SetSelectCallback<CEffectEditor>(this, &CEffectEditor::OnSpeedChangeTypeEdit);

    m_SpeedChangeType->AddItem("Select Type");
    for (int i = 0; i < (int)ParticleSpeedChangeMethod::Max - 1; ++i)
    {
        m_SpeedChangeType->AddItem(ParticleSpeedChangeType[i]);
    }
    m_SpeedChangeType->SetSelectIndex(0);

    // Color Min, Max
    Tree = AddWidget<CIMGUITree>("Color Min, Max");

    m_ColorMinEdit = Tree->AddWidget<CIMGUIColor4>("Color Min", 150.f);
    m_ColorMinEdit->SetCallBack(this, &CEffectEditor::OnColorMinEdit);

    m_ColorMaxEdit = Tree->AddWidget<CIMGUIColor4>("Color Max", 150.f);
    m_ColorMaxEdit->SetCallBack(this, &CEffectEditor::OnColorMaxEdit);

    // Alpha Min, Max
    Tree = AddWidget<CIMGUITree>("Alpha Start, End");

    m_AlphaBlendEnableButton = Tree->AddWidget<CIMGUIButton>("Set Alpha Blend", 150.f, 20.f);
    m_AlphaBlendEnableButton->SetClickCallback<CEffectEditor>(this, &CEffectEditor::OnSetAlphaBlendToMaterialCallback);

    m_AlphaStartEdit = Tree->AddWidget<CIMGUIInputFloat>("Alpha Start", 150.f);
    m_AlphaStartEdit->SetCallBack(this, &CEffectEditor::OnAlphaStartEdit);

    m_AlphaEndEdit = Tree->AddWidget<CIMGUIInputFloat>("Alpha End", 150.f);
    m_AlphaEndEdit->SetCallBack(this, &CEffectEditor::OnAlphaEndEdit);

    m_IsAlphaLinearFromCenter = Tree->AddWidget<CIMGUICheckBox>("Alpha Linear", 30.f);
    m_IsAlphaLinearFromCenter->AddCheckInfo("Alpha Linear");
    m_IsAlphaLinearFromCenter->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnAlphaLinearFromCenter);

    // Move Dir, Angle
    Tree = AddWidget<CIMGUITree>("Move Angle, Dir");

    m_SpecialMoveDirType = Tree->AddWidget<CIMGUIComboBox>("Shape", 150.f);
    m_SpecialMoveDirType->SetSelectCallback<CEffectEditor>(this, &CEffectEditor::OnClickSpecialMoveDirType);

    m_SpecialMoveDirType->AddItem("Select Dir");
    for (int i = 0; i < (int)ParticleSpecialMoveDir::Max; ++i)
    {
        m_SpecialMoveDirType->AddItem(ParticleMoveDirType[i]);
    }
    m_SpecialMoveDirType->SetSelectIndex(0);

    m_MoveDirEdit = Tree->AddWidget<CIMGUIInputFloat3>("Move Dir", 150.f);
    m_MoveDirEdit->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnMoveDirEdit);

    m_MoveAngleEdit = Tree->AddWidget<CIMGUIInputFloat3>("Move Angle", 150.f);
    m_MoveAngleEdit->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnMoveAngleEdit);

    // m_ApplyNoiseTextureSampling
    Tree = AddWidget<CIMGUITree>("Pixel Shader");

    // ���Ῠ ���� UV Clipping
    m_UVClippingAccordingToDir = Tree->AddWidget<CIMGUICheckBox>("Linear UV Clip", 80.f);
    m_UVClippingAccordingToDir->AddCheckInfo("Linear UV Clip");
    m_UVClippingAccordingToDir->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnIsUVClippingReflectingMoveDirEdit);

    m_UVClippingDirReverse = Tree->AddWidget<CIMGUICheckBox>("UV Dir Rev", 80.f);
    m_UVClippingDirReverse->AddCheckInfo("UV Dir Rev");
    m_UVClippingDirReverse->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnUVClippingDirReverse);

    m_ApplyNoiseTextureSampling = Tree->AddWidget<CIMGUICheckBox>("Noise Texture", 80.f);
    m_ApplyNoiseTextureSampling->AddCheckInfo("Noise Texture");
    m_ApplyNoiseTextureSampling->SetCallBackLabel<CEffectEditor>(this, &CEffectEditor::OnApplyNoiseTextureSamplingEdit);

    m_NoiseTextureApplyRatio = Tree->AddWidget<CIMGUIInputFloat>("Noise Start Ratio", 150.f, 20.f);
    m_NoiseTextureApplyRatio->SetCallBack<CEffectEditor>(this, &CEffectEditor::OnSetNoiseTextureApplyRatio);

    // GameObject ����
    SetGameObjectReady();

	return true;
}

void CEffectEditor::OnSaveParticleObjectButton()
{
}

void CEffectEditor::OnLoadParticleObjectButton()
{
}

void CEffectEditor::OnRestartParticleComponentButton()
{
    // ���� ���۵� ���ߴٸ� ���� X
    if (!m_ParticleObject)
        return;

    if (!m_ParticleClass)
        return;

    // ������ �����ϴ� Particle Object �� ������� �Ѵ�
    if (m_ParticleObject)
    {
        m_ParticleObject->Destroy();
    }

    BackUpParticleObjectInfo();

    m_ParticleObject = CSceneManager::GetInst()->GetScene()->CreateGameObject<C3DParticleObject>("Particle Effect Base Ground");
    
    // Init Spawn Count ������ �ݿ��Ѵ�.
    // �Ʒ� SetParticleToParticleComponent ������ ��������� �Ѵ�. m_PArticle Class �� �ش� ������ �����صΰ�
    // �̸� Particle Component �� �ݿ��ϴ� �����̴�.
    m_ParticleClass->SetSpawnCountMax(m_SpawnCountMaxEdit->GetVal());

    SetParticleToParticleComponent(dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent()), m_ParticleClass);

    ReflectBackUpParticleObjectInfo();

    // IMGUI�� Particle ���� �ݿ��ϰ� �ϱ� 
    SetIMGUIReflectParticle(m_ParticleClass);

    // IMGUI�� Paritlc Object ���� �ݿ��ϰ� �ϱ� 
    SetIMGUIReflectObjectCamera();
}

void CEffectEditor::OnIsUVMoveEnableEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetUVMoveEnable(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetUVMoveEnable(Enable);
}

void CEffectEditor::OnUVRowNEdit(int Num)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetUVRowN(Num);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetUVRowN(Num);
}

void CEffectEditor::OnUVColNEdit(int Num)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetUVColN(Num);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetUVColN(Num);
}

void CEffectEditor::OnClickParticlePreset(int Index, const char* PresetName)
{
    // Select Index ����
    // m_CurAnimComboBox->SetSelectIndex(Index);

    if (Index == 0)
        return;

    switch (Index - 1)
    {
    case (int)ParticlePreset::Ripple :
    {
        OnRipplePreset();
    }
    break;
    case (int)ParticlePreset::Ring:
    {
        OnRingPreset();
    }
    break;
    case (int)ParticlePreset::RingWall:
    {
        OnRingWallPreset();
    }
    break;
    case (int)ParticlePreset::FireWide:
    {
        OnFireWidePreset();
    }
    break;
    case (int)ParticlePreset::Spark:
    {
        OnSparkPreset();
    }
    break;
    case (int)ParticlePreset::SparkBounce:
    {
        OnSparkBouncePreset();
    }
    break;
    case (int)ParticlePreset::SimpleMeteor:
    {
        OnSimpleMeteorPreset();
    }
    break;
    case (int)ParticlePreset::XZSpreadDust:
    {
        OnXZSpreadDustPreset();
    }
    break;
    case (int)ParticlePreset::FireTorch:
    {
        OnFireTorchPreset();
    }
    break;
    case (int)ParticlePreset::FireGenerateMoment:
    {
        OnFireGenerateMomentPreset();
    }
    break;
    case (int)ParticlePreset::BloodPartice:
    {
        OnBloodEachParticlePreset();
    }
    break;
    case (int)ParticlePreset::XYBloodSpreadSpiral:
    {
        OnXYBloodSpreadPreset();
    }
    break;
    case (int)ParticlePreset::XZSpreadGrass:
    {
        OnXZSpreadGrassPreset();
    }
    break;
    case (int)ParticlePreset::FireSmokeUp:
    {
        OnFireSmokeUpPreset();
    }
    break;
    case (int)ParticlePreset::FireCracker:
    {
        OnFireCrackerPreset();
    }
    break;
    case (int)ParticlePreset::SummonEffect:
    {
        OnSummonEffectPreset();
    }
    break;
    case (int)ParticlePreset::BettyAttackSpreadDust:
    {
        OnBettyAttackSpreadDustPreset();
    }
    break;
    case (int)ParticlePreset::PlayerDust:
    {
        OnPlayerDustPreset();
    }
    break;
    default:
        break;
    } 

}

void CEffectEditor::OnLoadParticleMaterialCallback()
{
    // Load �ϰ�
    const PathInfo* MaterialPathInfo = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

    // Bin//Material//ParticleMaterial ������ �ִ��� Ȯ���ϰ� ������ش�.
    CEngineUtil::CheckAndMakeDirectory(MaterialPathInfo);

    TCHAR FilePath[MAX_PATH] = {};

    OPENFILENAME OpenFile = {};
    OpenFile.lStructSize = sizeof(OPENFILENAME);
    OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
    OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Material File\0*.mtrl");
    OpenFile.lpstrFile = FilePath;
    OpenFile.nMaxFile = MAX_PATH;
    OpenFile.lpstrInitialDir = MaterialPathInfo->Path;

    if (GetOpenFileName(&OpenFile) != 0)
    {
        char	Ext[_MAX_EXT] = {};

        char FilePathMultibyte[MAX_PATH] = {};
        char FileName[MAX_PATH] = {};

        int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
        WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);

        _splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, FileName, MAX_PATH, Ext, _MAX_EXT);

        _strupr_s(Ext);

        // ���� Load�ϴ� Directory�� Bin/Material/ParticleMaterial ���� Ȯ���ϱ� => �ƴ϶��, Load
        std::string PathInfoBeforeFileName;
        CEngineUtil::GetPathInfoBeforeFileName(FilePathMultibyte, PathInfoBeforeFileName);

         // � Material Folder �� ���� ���������� Load �� �� �ֵ��� �����Ѵ�.
          //if (strcmp(MaterialPathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
          //{
          //    MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Material �� ���, �ݵ�� Bin/Material/ParticleMaterial �κ��� Load �ؾ� �Ѵ�."), NULL, MB_OK);
          //    return;
          //}

        // Ȯ���� .anim �� �ƴ϶�� return;
        if (strcmp(Ext, ".MTRL") != 0)
        {
            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .mtrl"), NULL, MB_OK);
            return;
        }

        // ���� �̸���, Material �� �����ϴ� Key ������ Ȱ���� ���̴�.
        CMaterial* LoadedMaterial = CResourceManager::GetInst()->LoadMaterialFullPathMultibyte(FilePathMultibyte);

        if (!LoadedMaterial)
        {
            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Load Failure"), NULL, MB_OK);
            return;
        }
  
        std::string MaterialFileName = LoadedMaterial->GetName();

        if (MaterialFileName.find(".mtrl") == std::string::npos)
            MaterialFileName.append(".mtrl");

        // Hard Disk �κ���, File�� Drop���� ��쿡��, FileName�� �����Ѵ�.
        m_LoadedMaterialFileName->SetText(MaterialFileName.c_str());

        // Particle �� �����ϰ�
        ApplyNewMaterial(LoadedMaterial);

        // ResourceDisplay Window �� �ִ� Texture ��ϵ� Resource Window �� �߰��ؼ� �����ֱ�
        CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
        CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
        CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedRenderStateResources();
        CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedShaderResources();
    }
}

void CEffectEditor::OnEditBaseGroundSize(float Size)
{
    m_GroundTextureScale->SetValue(Size);
    m_BaseGroundObject->SetWorldScale(Size, Size, 1.f);
}

void CEffectEditor::OnIsBounceEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetBounceEnable(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(Enable);
}

void CEffectEditor::OnEditBounceResistance(float Resist)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetBounceResistance(Resist);
   dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceResist(Resist);
}



void CEffectEditor::OnClickParticleShape(int Index, const char* PresetName)
{
    if (!m_ParticleClass)
        return;

    // �ڱⰡ ���õ� Idx - 1 �� ����� �Ѵ�
    // ó������ �ƹ��͵� ���þȵǾ� �ִ� ���� -> �׷��� Idx �� 0 -> -1�� ����߸�
    // hlsl ������ -1�� �ɸ��� �ʰ� �� ���̴�.
    ParitcleShapeType Type = (ParitcleShapeType)(Index - 1);
   m_ParticleClass->SetParticleShapeType(Type);
   dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(Type);

   // Torch �� ���, Spawn Time �� ��������� �Ѵ�.
    // if (Type == ParitcleShapeType::Torch)
    // {
    //     m_ParticleClass->SetSpawnTimeMax(0.01f);
    //     dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->SetSpawnTime(0.01f);
    // }
}

void CEffectEditor::OnIsLoopGenerateRingEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetLoopGenerateRing(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(Enable);
}

void CEffectEditor::OnEditGenerateRadius(float Radius)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetGenerateRadius(Radius);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGenerateRadius(Radius);
}

void CEffectEditor::OnSpawnTimeMaxEdit(float Num)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetSpawnTimeMax(Num);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->SetSpawnTime(Num);
}

void CEffectEditor::OnDisableNewAlive(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetDisableNewAlive(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetDisableNewAlive(Enable);

    // ������ �����ص� SpawnTimeMax ������ �ٽ� �����Ѵ�.
    if (Enable == false)
    {
        OnSpawnTimeMaxEdit(m_SpawnTimeMaxEdit->GetVal());
    }
}

void CEffectEditor::OnApplySpawnTimeWhileDisableNewAlive(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    // m_ParticleClass->SetApplySpawnTimeDuringDisableNewAlive(Enable);
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetApplySpawnTimeDuringDisableNewAlive(Enable);
}

void CEffectEditor::OnIsUVClippingReflectingMoveDirEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetLinearUVClippingEnable(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLinearUVClippingEnable(Enable);
}

void CEffectEditor::OnUVClippingDirReverse(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetLinearUVClippingDirReverseEnable(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLinearUVClippingDirReverseEnable(Enable);
}

// StartMin, Max , SpawnCountMax �� ���, Particle Component ���� Particle �� ��� ���۷� ���� ������ �ٷ� ���ͼ� Post Update ���� ���
// ���� Particle �� ��� ���� ������ �ٲ��ָ� �ȴ�.
void CEffectEditor::OnStartMinEdit(const Vector3& Pos)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetStartMin(Pos);
    // m_ParticleComponent->GetParticle()->SetStartMin(Pos);
}

void CEffectEditor::OnStartMaxEdit(const Vector3& Pos)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetStartMax(Pos);
    // m_ParticleComponent->GetParticle()->SetStartMax(Pos);
}

void CEffectEditor::OnScaleMinEdit(const Vector3& Scale)
{
    if (!m_ParticleClass)
        return;

    // m_ParticleClass->SetScaleMin(m_ScaleMinEdit->GetValue());
    m_ParticleClass->SetScaleMin(Scale);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetScaleMin(Scale);
}

void CEffectEditor::OnScaleMaxEdit(const Vector3& Scale)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetScaleMax(Scale);

    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetScaleMax(Scale);
    // m_ParticleComponent->GetCBuffer()->SetScaleMax(Scale);
}

void CEffectEditor::OnLifeTimeMinEdit(float Num)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetLifeTimeMin(Num);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeMin(Num);
    // m_ParticleComponent->GetCBuffer()->SetLifeTimeMin(Num);
}

void CEffectEditor::OnLifeTimeMaxEdit(float Num)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetLifeTimeMax(Num);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeMax(Num);
    // m_ParticleComponent->GetCBuffer()->SetLifeTimeMax(Num);
}

void CEffectEditor::OnSpeedStartEdit(float Num)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetSpeedStart(Num);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpeedStart(Num);
    // m_ParticleComponent->GetCBuffer()->SetSpeedStart(Num);
}

void CEffectEditor::OnSpeedEndEdit(float Num)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetSpeedEnd(Num);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpeedEnd(Num);
    // m_ParticleComponent->GetCBuffer()->SetSpeedEnd(Num);
}

void CEffectEditor::OnSpeedChangeTypeEdit(int Index, const char* Type)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetSpeedChangeMethod(Index);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpeedChangeMethod(Index);
}

void CEffectEditor::OnColorMinEdit(const Vector4& Color)
{
    if (!m_ParticleClass)
        return;

    // Alpha ���� 0���� �Ѵ�.

    //m_ParticleClass->SetColorMin(Color.x, Color.y, Color.z, 1.f);
    m_ParticleClass->SetColorMin(Color);
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetColorMin(Color.x, Color.y, Color.z, 1.f);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetColorMin(Color);
    // m_ParticleComponent->GetCBuffer()->SetColorMin(Color);
}

void CEffectEditor::OnColorMaxEdit(const Vector4& Color)
{
    if (!m_ParticleClass)
        return;

    // m_ParticleClass->SetColorMax(Color.x, Color.y, Color.z, 1.f);
    m_ParticleClass->SetColorMax(Color);
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetColorMax(Color.x, Color.y, Color.z, 1.f);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetColorMax(Color);
    // m_ParticleComponent->GetCBuffer()->SetColorMax(Color);
}

void CEffectEditor::OnAlphaStartEdit(float Alpha)
{
    if (!m_ParticleClass)
        return;

    // Alpha ���� 0���� �Ѵ�.

    // m_ParticleClass->SetColorMin(Color.x, Color.y, Color.z, 1.f);
    m_ParticleClass->SetStartAlpha(Alpha);
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetColorMin(Color.x, Color.y, Color.z, 1.f);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetStartAlpha(Alpha);
}

void CEffectEditor::OnAlphaEndEdit(float Alpha)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetEndAlpha(Alpha);
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetColorMin(Color.x, Color.y, Color.z, 1.f);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetEndAlpha(Alpha);
}

void CEffectEditor::OnAlphaLinearFromCenter(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetAlphaLinearFromCenter(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetAlphaLinearFromCenter(Enable);
}

void CEffectEditor::OnSetAlphaBlendToMaterialCallback()
{
    if (!m_ParticleClass)
        return;

    if (m_ParticleClass->GetMaterial())
    {
        CRenderState* FoundRenderState = CRenderManager::GetInst()->FindRenderState("AlphaBlend");
        
        if (!FoundRenderState)
        {
            assert(false);
        }

        // Particle Class �� Material �� Render State �����ϱ� (�̷��߸� ����� ��, Render State �� ������ ä�� Material ������ ����ȴ�.)
        m_ParticleClass->GetMaterial()->SetRenderState(FoundRenderState);

        // ���� Render �Ǵ� Particle Component �󿡵� ��������� �Ѵ�.
        dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetMaterial()->SetRenderState(FoundRenderState);

        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Alpha Blend Set Success"), NULL, MB_OK);
    }
}


void CEffectEditor::OnIsMoveEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetMove(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(Enable);
    // m_ParticleComponent->GetCBuffer()->SetMove(Enable);
}

void CEffectEditor::OnIsFollowComponentPosEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetFollowRealTimeParticleComponentPos(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetFollowRealTimeParticleComponentPos(Enable);
}

void CEffectEditor::OnIsGravityEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetGravity(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(Enable);
    // m_ParticleComponent->GetCBuffer()->SetGravity(Enable);
}

// void CEffectEditor::OnIsRandomMoveEdit(const char*, bool Enable)
// {
//     if (!m_ParticleClass)
//         return;
// 
//     m_ParticleClass->SetApplyRandom(Enable); //
//     dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetApplyRandom(Enable);
//     // m_ParticleComponent->GetCBuffer()->SetApplyRandom(Enable);
// }

void CEffectEditor::OnPauseResumeToggle(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleObject->GetRootComponent()->Enable(Enable);
}

void CEffectEditor::OnMinSeperateRotAngleEdit(const Vector3& RotAngle)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetMinSeperateRotAngle(RotAngle);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMinSeperateRotAngle(RotAngle);
}

void CEffectEditor::OnMaxSeperateRotAngleEdit(const Vector3& RotAngle)
{
    if (!m_ParticleClass)
        return;

   m_ParticleClass->SetMaxSeperateRotAngle(RotAngle);
   dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMaxSeperateRotAngle(RotAngle);
}

void CEffectEditor::OnIsLinearRot(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetRotToDir(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(Enable);
}

void CEffectEditor::OnIsLifeTimeLinearFromCenter(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetLifeTimeLinearFromCenter(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(Enable);
}

void CEffectEditor::OnIsCameraRotateEdit(const char*, bool Enable)
{
    m_ParticleObject->SetCameraRotate(Enable);
}

void CEffectEditor::OnCameraRotateInvEdit(const char*, bool Enable)
{
    m_ParticleObject->SetRotateInv(Enable);
}

void CEffectEditor::OnSetCameraRotateSpeed(float Speed)
{
    m_RotateSpeedSliderBar->SetValue(Speed);

    m_ParticleObject->SetCameraRotateSpeed(Speed);
}

void CEffectEditor::OnSetCameraYAxisRotate(float Rot)
{
    m_YRotateSliderBar->SetValue(Rot);

    m_ParticleObject->SetRelativeRotationY(Rot);
}

void CEffectEditor::OnIsCameraZoomEdit(const char*, bool Enable)
{
    m_ParticleObject->SetCameraZoom(Enable);
}

void CEffectEditor::OnSetCameraZoomSpeed(float Speed)
{
    m_ZoomSpeedSliderBar->SetValue(Speed);

    m_ParticleObject->SetCameraZoomSpeed(Speed);
}

void CEffectEditor::OnSetCameraYOffset(float Offset)
{
    m_ParticleObject->SetYOffset(Offset);
}

void CEffectEditor::OnSetCameraXRot(float Rot)
{
    m_ParticleObject->SetRelativeRotationXPos(Rot);
}

void CEffectEditor::OnApplyNoiseTextureSamplingEdit(const char*, bool Enable)
{
    if (!m_ParticleClass)
        return;

    // Material �� 2��° Texture �� Noise Texture�� Ȱ���� ���̴�.
    // �׸��� Noise Texture �� Register 100 ������ �����Ǿ� �ִ�.
    // ���� 2���� Texture �� Register 100 ���� �ƴ϶��, MessageBox �� �����, 
    // True  ������ �����Ѵ�.
    if (Enable)
    {
        const std::vector<MaterialTextureInfo>& TextureInfo = m_ParticleClass->GetMaterial()->GetTextureInfo();

        if (TextureInfo.size() < 2 ||
            TextureInfo[1].Register != 100)
        {
            // �ٽ� false üũ
            m_ApplyNoiseTextureSampling->SetCheck(0, false);

            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("2��° Texture(Noise Texture) �� �������� �ʰų�, Register �� 100���� �ƴմϴ� "), NULL, MB_OK);
            return;
        }
    }


    m_ParticleClass->SetApplyNoiseTextureSamplingEnable(Enable);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetApplyNoiseTextureSamplingEnable(Enable);
}

void CEffectEditor::OnSetNoiseTextureApplyRatio(float Ratio)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetNoiseTextureApplyRatio(Ratio);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetNoiseTextureApplyRatio(Ratio);
}

void CEffectEditor::OnMoveDirEdit(const Vector3& Dir)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetMoveDir(Dir);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMoveDir(Dir);
}

void CEffectEditor::OnMoveAngleEdit(const Vector3& Angle)
{
    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetMoveAngle(Angle);

    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMoveAngle(Angle);
}

void CEffectEditor::OnClickSpecialMoveDirType(int Index, const char* Type)
{
    if (!m_ParticleClass)
        return;

    // �ڱⰡ ���õ� Idx - 1 �� ����� �Ѵ�
    // ó������ �ƹ��͵� ���þȵǾ� �ִ� ���� -> �׷��� Idx �� 0 -> -1�� ����߸�
    // hlsl ������ -1�� �ɸ��� �ʰ� �� ���̴�.
    ParticleSpecialMoveDir DirType = (ParticleSpecialMoveDir)(Index - 1);

    m_ParticleClass->SetSpecialMoveDirType(DirType);

    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(DirType);
}

void CEffectEditor::OnSaveParticleClass()
{
    if (!m_ParticleObject || !dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle())
        return;

    const PathInfo* ParticlePathInfo = CPathManager::GetInst()->FindPath(PARTICLE_PATH);

    // Bin//ParticleClass �� Path ���� �����ϴ��� Ȯ���ϱ� , ������ ����� 
    CEngineUtil::CheckAndMakeDirectory(ParticlePathInfo);

    TCHAR FiileFullPath[MAX_PATH] = {};

    OPENFILENAME OpenFile = {};
    OpenFile.lStructSize = sizeof(OPENFILENAME);
    OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
    OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Animation File\0*.anim");
    OpenFile.lpstrFile = FiileFullPath;
    OpenFile.nMaxFile = MAX_PATH;
    OpenFile.lpstrInitialDir = ParticlePathInfo->Path; // Bin//ParticleClass

    if (GetSaveFileName(&OpenFile) != 0)
    {
        char FileFullPathMultibyte[MAX_PATH] = {};
        char FileName[MAX_PATH] = {};
        char FileExt[_MAX_EXT] = {};

        int  ConvertLength = WideCharToMultiByte(CP_ACP, 0, FiileFullPath, -1, nullptr, 0, nullptr, nullptr);

        WideCharToMultiByte(CP_ACP, 0, FiileFullPath, -1, FileFullPathMultibyte, ConvertLength, nullptr, nullptr);

        _splitpath_s(FileFullPathMultibyte, nullptr, 0, nullptr, 0, FileName, MAX_PATH, FileExt, _MAX_EXT);

        _strupr_s(FileExt);

        // Ȯ���� .anim �� �ƴ϶�� return;
        if (strcmp(FileExt, ".PRTC") != 0)
        {
            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .prtc"), NULL, MB_OK);
            return;
        }

        // ���� �����ϴ� Directory�� Bin/ParticleClass ���� Ȯ���ϱ� => �ƴ϶��, Save ����
        // std::string PathInfoBeforeFileName;
        // CEngineUtil::GetPathInfoBeforeFileName(FileFullPathMultibyte, PathInfoBeforeFileName);
        // 
        // if (strcmp(ParticlePathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
        // {
        //     MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Class �� ���, �ݵ�� Bin/ParticleClass �� ����"), NULL, MB_OK);
        //     return;
        // }

        // �ش� PARTICLE_PATH ���� �ߺ��� �̸��� �ִ��� Ȯ���ϱ�
        // .prtc Ȯ���� ���̱�
        char CheckFileName[MAX_PATH] = {};
        strcpy_s(CheckFileName, FileName);
        strcat_s(CheckFileName, ".prtc");

        bool IsSameFileNameExist = CEditorUtil::IsFileExistInDir(PARTICLE_PATH, CheckFileName);

        if (IsSameFileNameExist)
        {
            if (MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("���� �̸��� .prtc ������ �����մϴ�. �����Ͻðڽ��ϱ�?"), NULL, MB_YESNO) != IDYES)
                return;
        }

        // Particle Manager �󿡼� ���� �̸����� ����Ǿ��ִ� Particle �� Key ���� ���� ������ ���� �̸����� ���������ش�.
        const std::string& PrevParticleName = m_ParticleClass->GetName();

        CResourceManager::GetInst()->GetParticleManager()->ChangeParticleKeyName(PrevParticleName, FileName);

        // Resource Display Window �ٽ� Update
        CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedParticleResources();
        
        // ���� �����ϴ� ���� �̸����� PArticle Class �� �̸� �����ϱ� 
        m_ParticleClass->SetName(FileName);

        // �ش� Particle File �����ϱ� 
        m_ParticleClass->SaveFile(FileFullPathMultibyte);

        // IMGUI �ѹ� �� Update
        SetIMGUIReflectParticle(m_ParticleClass);
    }
}

void CEffectEditor::OnLoadParticleClass()
{
   //  if (!m_ParticleObject || !dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle())
   //      return;

    // Ȥ�� �𸣴� Particle Material ���� ���� ������ ������ش�.
    const PathInfo* ParticlePathInfo = CPathManager::GetInst()->FindPath(PARTICLE_PATH);
    CEngineUtil::CheckAndMakeDirectory(ParticlePathInfo);

    TCHAR FilePath[MAX_PATH] = {};

    OPENFILENAME OpenFile = {};
    OpenFile.lStructSize = sizeof(OPENFILENAME);
    OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
    OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Particle File\0*.prtc");
    OpenFile.lpstrFile = FilePath;
    OpenFile.nMaxFile = MAX_PATH;
    OpenFile.lpstrInitialDir = ParticlePathInfo->Path;

    if (GetOpenFileName(&OpenFile) != 0)
    {
        // ��� ����
        char	Ext[_MAX_EXT] = {};

        char FilePathMultibyte[MAX_PATH] = {};
        char FileName[MAX_PATH] = {};

        int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
        WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);

        _splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, FileName, MAX_PATH, Ext, _MAX_EXT);

        // ���� Load�Ǵ� Directory�� Bin/ParticleClass ���� Ȯ���ϱ� => �ƴ϶��, Load ����
        std::string PathInfoBeforeFileName;
        CEngineUtil::GetPathInfoBeforeFileName(FilePathMultibyte, PathInfoBeforeFileName);

        // Particle Folder ���� Load �� �� �ֵ��� �����ؾ� �Ѵ�.
        // if (strcmp(ParticlePathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
        // {
        //     MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Class �� ���, �ݵ�� Bin/ParticleClass ���� Load �ؾ� �Ѵ�."), NULL, MB_OK);
        //     return;
        // }

        _strupr_s(Ext);

        // Ȯ���� .anim �� �ƴ϶�� return;
        if (strcmp(Ext, ".PRTC") != 0)
        {
            MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Ȯ���ڰ� .prtc �̾�� �մϴ�."), NULL, MB_OK);
            return;
        }

        // Edit State ó�� 
        m_StartEdit = true;

        // Particle ���� Load �ϱ�
        // SAFE_DELETE(m_ParticleClass); --> Particle Manager ���� �˾Ƽ� �������� ���̴�.
        m_ParticleClass = CResourceManager::GetInst()->CreateParticleEmpty<CParticle>();
        m_ParticleClass->LoadFile(FilePathMultibyte);

        // ���� Particle Object �� ī�޶� ���� ���� ���
        BackUpParticleObjectInfo();

         // �ʿ��� Object ��� ���� => Particle Object �� ����
        SetGameObjectReady();

        // Particle Object Enable ó��
        m_ParticleObject->Enable(true);
        m_ParticleObject->GetRootComponent()->Enable(true);

        // ��� ���� �ݿ�
        ReflectBackUpParticleObjectInfo();

        // Particle Component �� Particle ����
        SetParticleToParticleComponent(dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent()), m_ParticleClass);

        // IMGUI Update
        SetIMGUIReflectParticle(m_ParticleClass);
        SetIMGUIReflectObjectCamera();

        // Particle Manager �� Map �� �߰��ϱ�
        CResourceManager::GetInst()->GetParticleManager()->AddParticle(m_ParticleClass);

        // Resource Display Window �����ϱ�
        CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedParticleResources();
        CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();

        // Particle Preset �� Select 0 ���� �����ѵ�
        m_ParticlePreset->SetSelectIndex(0);
    }
}

void CEffectEditor::OnSetBasicParticleMaterialSetting(CSceneComponent* Com)
{
    // �⺻ Particle Setting, ���� Component �� Particle Setting �ϱ�
    // 1) Particle Material ����
    CSceneManager::GetInst()->GetScene()->GetResource()->CreateMaterial<CMaterial>("BasicParticleMaterial");
    m_ParticleMaterial = CSceneManager::GetInst()->GetScene()->GetResource()->FindMaterial("BasicParticleMaterial");
    m_ParticleMaterial->AddTexture(0, (int)Buffer_Shader_Type::Pixel, "Bubble", TEXT("Particle/Bubbles99px.png"));
    m_ParticleMaterial->SetShader("ParticleRenderShader");
    m_ParticleMaterial->SetRenderState("AlphaBlend");
    m_ParticleMaterial->SetRenderState("NoCull");

    // 2) Particle ����
    CSceneManager::GetInst()->GetScene()->GetResource()->CreateParticle("BasicParticle");

    m_ParticleClass = CSceneManager::GetInst()->GetScene()->GetResource()->FindParticle("BasicParticle");

    if (!m_ParticleClass)
        return;

    m_ParticleClass->SetMaterial(m_ParticleMaterial);

    SetBasicDefaultParticleInfos(m_ParticleClass);

    // �ش� ������ Particle �� �����ϱ�
    SetParticleToParticleComponent(dynamic_cast<CParticleComponent*>(Com), "BasicParticle");

    // �⺻ Z Pos ����
    Com->SetWorldPos(Com->GetWorldPos().x, Com->GetWorldPos().y, 10.f);

    // Resource Window Display
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedParticleResources();
}

void CEffectEditor::OnReflectCurrentParticleSetting()
{
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->Set2D(false);
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetSpawnCountMax(m_SpawnCountMaxEdit->GetVal());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetSpawnTime(m_SpawnTimeMaxEdit->GetVal());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetLifeTimeMin(m_LifeTimeMinEdit->GetVal());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetLifeTimeMax(m_LifeTimeMaxEdit->GetVal());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetScaleMin(m_ScaleMinEdit->GetValue());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetScaleMax(m_ScaleMaxEdit->GetValue());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetSpeedStart(m_SpeedStartEdit->GetVal());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetSpeedEnd(m_SpeedEndEdit->GetVal());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetMoveDir(m_MoveDirEdit->GetValue());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetStartMin(m_StartMinEdit->GetValue());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetStartMax(m_StartMaxEdit->GetValue());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetColorMin(m_ColorMinEdit->GetRGBA());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetColorMax(m_ColorMaxEdit->GetRGBA());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetMoveAngle(m_MoveAngleEdit->GetValue());
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetGravity(m_IsGravityEdit->GetCheck(0));
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetMove(m_IsMoveEdit->GetCheck(0));
    // dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetParticle()->SetApplyRandom(m_IsRandomMoveEdit->GetCheck(0));
}
void CEffectEditor::SetGameObjectReady()
{
    // SkyObject
   // m_SkyObject = CSceneManager::GetInst()->GetScene()->CreateGameObject<CSkyObject>("Particle Effect Editor Sky");
   // m_SkyObject->GetRootComponent()->SetLayerName("ParticleEditorLayer");
   // m_SkyObject->SetScene(CSceneManager::GetInst()->GetScene());
   // m_SkyObject->Init();

    // Particle Object ����
    if (m_ParticleObject)
    {
        m_ParticleObject->Destroy();
        m_ParticleObject = nullptr;
    }

    m_ParticleObject = CSceneManager::GetInst()->GetScene()->CreateGameObject<C3DParticleObject>("Particle Effect Base Ground");

    // Scn ����� ����X
    m_ParticleObject->ExcludeFromSceneSave();
    // Scene Change �ÿ� �ı� X
    m_ParticleObject->SetNoDestroyOnSceneChange(true);

    // Callback Function ����
    m_ParticleObject->SetCameraRotateCallback<CEffectEditor>(this, &CEffectEditor::OnSetCameraYAxisRotate);

    // ó������ Enable False �� �Ͽ� ������ �ʰ� �Ѵ�.
    m_ParticleObject->Enable(false);
    m_ParticleObject->GetRootComponent()->Enable(false);

    // ���� Base Ground Object �� �����.
    if (m_BaseGroundObject)
    {
        m_BaseGroundObject->Destroy();
        m_BaseGroundObject = nullptr;
    }

    if (!m_BaseGroundObject)
    {
        m_BaseGroundObject = CSceneManager::GetInst()->GetScene()->CreateGameObject<CGameObject>("Particle Effect Base Ground");
        m_BaseGroundObject->CreateComponent<CSpriteComponent>("Simple Sprite");
        m_BaseGroundObject->GetRootComponent()->SetLayerName("ParticleEditorLayer");
        m_BaseGroundObject->SetPivot(0.5, 0.5, 0.5);
        m_BaseGroundObject->SetWorldScale(300.f, 300.f, 1.f);
        m_BaseGroundObject->AddWorldRotationX(90.f);
        m_BaseGroundObject->AddWorldPos(0.f, -30.f, 0.f);

        // Scene Change �ÿ� �ı� X
        m_BaseGroundObject->ExcludeFromSceneSave();
        m_BaseGroundObject->SetNoDestroyOnSceneChange(true);

        CSpriteComponent* BaseGroundComponent = dynamic_cast<CSpriteComponent*>(m_BaseGroundObject->GetRootComponent());
        BaseGroundComponent->SetMaterial(CResourceManager::GetInst()->FindMaterial("ParticleEditorBaseGround"));

        // BaseGround Object �� ũ�⸦ IMGUI �� �ݿ��ϱ�
        const Vector3& WorldScale = m_BaseGroundObject->GetWorldScale();
        m_GroundTextureScale->SetValue(WorldScale.x);
    }
}

void CEffectEditor::SetBasicDefaultParticleInfos(CParticle* Particle)
{
    // �ݵ�� 3D �� �����Ѵ�.
    // Particle->Set2D(false);

    // Spawn Count
    Particle->SetSpawnCountMax(1000);

    // Init Spawn Count Max �����ϱ�
    Particle->SetSpawnCountMax(m_SpawnCountMaxEdit->GetVal());

    // Spawn Time
    Particle->SetSpawnTimeMax(0.05f);

    // Life Time
    Particle->SetLifeTimeMin(5.f);
    Particle->SetLifeTimeMax(10.f);

    // Scale
    Particle->SetScaleMin(Vector3(20.f, 20.f, 1.f));
    Particle->SetScaleMax(Vector3(50.f, 50.f, 1.f));

    // Speed
    Particle->SetSpeedStart(10.f);
    Particle->SetSpeedEnd(30.f);

    // Start Min,Max
    Particle->SetStartMin(Vector3(-30.f, -30.f, 0.f));
    Particle->SetStartMax(Vector3(30.f, 30.f, 0.f));

    // Color Min, Max
    Particle->SetColorMin(Vector4(0.2f, 0.1f, 0.8f, 1.f));
    Particle->SetColorMax(Vector4(0.6f, 0.8f, 0.8f, 1.f));

    // Alpha
    Particle->SetStartAlpha(1.f);
    Particle->SetEndAlpha(1.f);

    // Move Dir
    Particle->SetMoveDir(Vector3(0.f, 1.f, 0.f)); 
    // Move Angle
    Particle->SetMoveAngle(Vector3(0.f, 0.f, 30.f));

    // IsGravity
    // IsMove
    // IsRandomMove
    // IsPauseResume -> ������ Enable true �� ���� �����Ѵ�.
    Particle->SetGravity(true);
    Particle->SetMove(true);
    Particle->Enable(true);

    // Bounce
    Particle->SetBounceEnable(false);
    Particle->SetBounceResistance(0.98f);
}

void CEffectEditor::SetStartEditing()
{
    if (m_StartEdit)
        return;

    m_StartEdit = true;

    // �̹� Particle Object �� ������� �ִٸ� => Particle Editing �� ������ �����̹Ƿ�
    m_ParticleObject->Enable(true);
    m_ParticleObject->GetRootComponent()->Enable(true);

    OnSetBasicParticleMaterialSetting(m_ParticleObject->GetRootComponent());

    SetIMGUIReflectParticle(m_ParticleClass);

    SetIMGUIReflectObjectCamera();

}

void CEffectEditor::OnDropMaterialToParticle(const std::string& InputName)
{

    if (!m_ParticleClass)
    {
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("No Particle Set"), NULL, MB_OK);
        
        // �ٽ� ���� ���� ������� �Ѵ�.
        m_CurrentMaterialName->ResetText();

        return;
    }

    // Key �� ���·� Resource Manager ���� Mateirla �� ���� ã�´�.
    CMaterial* FoundMaterial = CResourceManager::GetInst()->FindMaterial(InputName);

    if (FoundMaterial)
    {
        ApplyNewMaterial(FoundMaterial);
        return;
    }

    // �� ���� �ϵ� ��ũ���� ã�´�.
    // Drop �� Input �ȿ� �ִ� Texture ���� �ҷ����� 
    // Texture File �� File ����, Texture �� �����ϴ� Key ������ ����� ���̴�.
    std::string MaterialKeyName;

    // Bin//Material//ParticleClass ��� �������� ã�� ���̴�.
    // std::optional<std::string> FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(MATERIAL_PATH, InputName, MaterialKeyName);
    std::optional<std::string> FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(MATERIAL_PARTICLE_PATH, InputName, MaterialKeyName);

    // ã�� ���ߴٸ� 
    if (!FoundResult.has_value())
    {
        // New Texture Load Failure Message Box
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Load Failure From Bin/Material/ParticleMaterial"), NULL, MB_OK);

        // �ٽ� ���� ���� ������� �Ѵ�.
        m_CurrentMaterialName->ResetText();

        return;
    }

    // Ȯ���� Ȯ��
    std::string ExtractFileName;
    std::string ExtractFileExt;

    CEditorUtil::ExtractFileNameAndExtFromPath(FoundResult.value(), ExtractFileName, ExtractFileExt);

    // �빮��ȭ
    std::transform(ExtractFileExt.begin(), ExtractFileExt.end(), ExtractFileExt.begin(), ::toupper);
    
    if (ExtractFileExt != ".MTRL")
    {
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Ȯ���ڰ� .mtrl�� �ƴմϴ�."), NULL, MB_OK);

        // �ٽ� ���� ���� ������� �Ѵ�.
        m_CurrentMaterialName->ResetText();

        return;
    }

    // Load �õ�
    FoundMaterial = CResourceManager::GetInst()->LoadMaterialFullPathMultibyte(FoundResult.value().c_str());
    
    if (!FoundMaterial)
    {
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Load Process Failure"), NULL, MB_OK);

        // �ٽ� ���� ���� ������� �Ѵ�.
        m_CurrentMaterialName->ResetText();

        return;
    }

    // Hard Disk �κ���, File�� Drop���� ��쿡��, FileName�� �����Ѵ�.
    m_LoadedMaterialFileName->SetText((ExtractFileName + ".mtrl").c_str());
    
    ApplyNewMaterial(FoundMaterial);
}

void CEffectEditor::ApplyNewMaterial(CMaterial* FoundMaterial)
{
    if (!m_ParticleClass)
    {
        // ����� ���õǾ��ٴ� Message
        MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle�� Setting ���� �ʾҽ��ϴ�."), NULL, MB_OK);

        // Ȥ�ó� Material Loaded FileName �̸��� �ش� �Լ��� ������ ���� ���õǾ��� ���� �����Ƿ� 
        // �ٽ� �ǵ��� ���´�.
        m_LoadedMaterialFileName->ResetText();

        return;
    }

    if (!FoundMaterial)
        return;

    m_CurrentMaterialName->SetText(FoundMaterial->GetName().c_str());

    m_ParticleClass->SetMaterial(FoundMaterial);
    m_ParticleMaterial = FoundMaterial;

    // �ش� Material �� Texture �� �ҷ��ͼ� Image �� �����ϱ� => ù��° Texture �� �������ش�.
    if (FoundMaterial->GetTextureInfo().size() > 0)
        m_ParticleTexture->SetTexture(FoundMaterial->GetTexture());

    // Particle �� Material ������ Particle Component �� �ٽ� �ݿ��Ѵ�.
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->SetMaterial(m_ParticleClass->CloneMaterial());

    // ����� ���õǾ��ٴ� Message
    MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Set SuccessFully"), NULL, MB_OK);

    // Resource Window Display Update
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
    CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
}

void CEffectEditor::BackUpParticleObjectInfo()
{
    if (!m_ParticleObject)
        return;

    m_BackUpStruct.IsCameraRotate = m_ParticleObject->IsCameraRotate();
    m_BackUpStruct.IsCameraRotateInv = m_ParticleObject->IsCameraRotateInv();
    m_BackUpStruct.IsCameraRotateSpeed = m_ParticleObject->GetCameraRotateSpeed();
    m_BackUpStruct.IsCameraZoom = m_ParticleObject->IsCameraZoom();
    m_BackUpStruct.CameraZoomSpeed = m_ParticleObject->GetCameraZoomSpeed();


    m_BackUpStruct.CameraYOffset = m_ParticleObject->GetCameraYOffset();
    m_BackUpStruct.CameraXRot = m_ParticleObject->GetCameraRelativeRotation().x;
    m_BackUpStruct.TargetDist = m_ParticleObject->GetArmComponent()->GetTargetDistance();
    m_BackUpStruct.WorldRotY = m_ParticleObject->GetCameraYOffset();
}

void CEffectEditor::ReflectBackUpParticleObjectInfo()
{
    if (!m_ParticleObject)
        return;

    m_ParticleObject->SetCameraRotate(m_BackUpStruct.IsCameraRotate);
    m_ParticleObject->SetRotateInv(m_BackUpStruct.IsCameraRotateInv);
    m_ParticleObject->SetCameraRotateSpeed(m_BackUpStruct.IsCameraRotateSpeed);
    m_ParticleObject->SetCameraZoom(m_BackUpStruct.IsCameraZoom);
    m_ParticleObject->SetCameraZoomSpeed(m_BackUpStruct.CameraZoomSpeed);

    m_ParticleObject->GetArmComponent()->SetOffset(0.f, m_BackUpStruct.CameraYOffset, 0.f);
    m_ParticleObject->GetArmComponent()->SetRelativeRotation(m_BackUpStruct.CameraXRot, 0.f, 0.f);
    m_ParticleObject->GetArmComponent()->SetTargetDistance(m_BackUpStruct.TargetDist);
    m_ParticleObject->GetArmComponent()->SetRelativeRotationY(m_BackUpStruct.WorldRotY);
}

void CEffectEditor::SetParticleToParticleComponent(CParticleComponent* Component,  const char* ParticleName)
{
    if (!Component)
        return;

    Component->SetParticle(ParticleName);
}

void CEffectEditor::SetParticleToParticleComponent(CParticleComponent* Component, CParticle* Particle)
{
    if (!Component)
        return;

    Component->SetParticle(Particle);
}

void CEffectEditor::SetIMGUIReflectParticle(CParticle* Particle)
{
    // Particle �̸� ����
    m_CurrentParticleName->SetText(Particle->GetName().c_str());

    // Material �̸� ����
    if (Particle->GetMaterial())
    {
        std::string MaterialName = Particle->GetMaterial()->GetName();
        m_CurrentMaterialName->SetText(MaterialName.c_str());

        // Material File Name ����
        if (MaterialName.find(".mtrl") == std::string::npos)
            MaterialName.append(".mtrl");
        
        m_LoadedMaterialFileName->SetText(MaterialName.c_str());

        // Texture ���� ����
        if (Particle->GetMaterial()->GetTextureInfo().size() > 0)
            m_ParticleTexture->SetTexture(Particle->GetMaterial()->GetTexture());
    }

    // �ݵ�� 3D �� �����Ѵ�.
    // Particle->Set2D(false);

    // Radius
    m_GenerateRadius->SetValue(Particle->GetGenerateRadius());

    // UV Move 
    m_IsUVMoveEnableEdit->SetCheck(0, Particle->GetUVMoveEnable());
    m_UVRowN->SetVal(Particle->GetUVRowN());
    m_UVColN->SetVal(Particle->GetUVColN());

    // Spawn Count, Time
    m_SpawnCountMaxEdit->SetVal(Particle->GetSpawnCountMax());
    m_SpawnTimeMaxEdit->SetVal(Particle->GetSpawnTimeMax());

    // Disable New Alive
    m_DisableNewAliveEdit->SetCheck(0, Particle->IsDisableNewAlive());
    // m_ApplyDuringDisableNewAliveEdit->SetCheck(0, Particle->IsApplySpawnTimeDuringDisableNewAlive());

    // LifeTime
    m_LifeTimeMinEdit->SetVal(Particle->GetLifeTimeMin());
    m_LifeTimeMaxEdit->SetVal(Particle->GetLifeTimeMax());
    m_IsLifeTimeLinearFromCenterEdit->SetCheck(0, Particle->IsLifeTimeLinearFromCenter());

    m_ScaleMinEdit->SetVal(Particle->GetScaleMin());
    m_ScaleMaxEdit->SetVal(Particle->GetScaleMax());

    m_SpeedStartEdit->SetVal(Particle->GetSpeedStart());
    m_SpeedEndEdit->SetVal(Particle->GetSpeedEnd());
    m_SpeedChangeType->SetSelectIndex(Particle->GetSpeedChangeMethod());

    m_StartMinEdit->SetVal(Particle->GetStartMin());
    m_StartMaxEdit->SetVal(Particle->GetStartMax());

    m_ColorMinEdit->SetRGBA(Particle->GetColorMin());
    m_ColorMaxEdit->SetRGBA(Particle->GetColorMax());

    m_AlphaStartEdit->SetVal(Particle->GetStartAlpha());
    m_AlphaEndEdit->SetVal(Particle->GetEndAlpha());
    m_IsAlphaLinearFromCenter->SetCheck(0, Particle->IsAlphaLinearFromCenter());

    // Move Dir, Angle
    m_MoveDirEdit->SetVal(Particle->GetMoveDir());
    m_MoveAngleEdit->SetVal(Particle->GetMoveAngle());

    // Particle Special Move Dir Type ���� ����
    m_SpecialMoveDirType->SetSelectIndex(Particle->GetSpecialMoveDirType() + 1);

    // Movement
    m_IsGravityEdit->SetCheck(0, Particle->GetGravity());
    m_IsMoveEdit->SetCheck(0, Particle->GetMove());
    m_IsFollowComponentWorldPosEdit->SetCheck(0, Particle->IsFollowRealTimeParticleComponentPos());
    // m_IsPauseResumeToggle->SetCheck(0, true); ��ǻ� ���� ���� ���� ���� �ڵ�

    // Bounce
    m_IsBounce->SetCheck(0, Particle->IsBounceEnable() == 1 ? true : false);
    m_BounceResistance->SetValue(Particle->GetBounceResistance());

    // Particle Shape (���� Idx �� +1 �� �����ϱ�)
    m_ParticleShapeType->SetSelectIndex(Particle->GetParticleShapeType() + 1);

    // Generate Ring
    m_IsLoopGenerateRing->SetCheck(0, Particle->IsLoopGenerateRing());

    // Min, Max Rot Angle
    m_MinSeperateRotAngleEdit->SetVal(Particle->GetMinSeperateRotAngle());
    m_MaxSeperateRotAngleEdit->SetVal(Particle->GetMaxSeperateRotAngle());

    // Linaer Rotate
    m_RotateAccordingToDir->SetCheck(0, Particle->IsSeperateLinearRotate());

    // UV Clip To Dir
    m_UVClippingAccordingToDir->SetCheck(0, Particle->IsLinearUVClippingEnabled());
    m_UVClippingDirReverse->SetCheck(0, Particle->IsLinearUVClippingDirReversed());

    // Noise Texture
    m_ApplyNoiseTextureSampling->SetCheck(0, Particle->IsNoiseTextureSamplingApplied());
    m_NoiseTextureApplyRatio->SetVal(Particle->GetNoiseTextureApplyRatio());
}

void CEffectEditor::SetIMGUIReflectObjectCamera()
{
    // ���ο� Particle Object �� ������ ���õǾ��� Camera ���� ������ �ݿ����ش�.

    m_IsRotateEdit->SetCheck(0, m_ParticleObject->IsCameraRotate());
    m_IsRotateInv->SetCheck(0, m_ParticleObject->IsCameraRotateInv());
    m_RotateSpeedSliderBar->SetValue(m_ParticleObject->GetCameraRotateSpeed());
    m_IsZoomEdit->SetCheck(0, m_ParticleObject->IsCameraZoom());
    m_ZoomSpeedSliderBar->SetValue(m_ParticleObject->GetCameraZoomSpeed());

    m_CameraYOffsetBar->SetValue(m_ParticleObject->GetCameraYOffset());
    m_CameraXRotSlideBar->SetValue(m_ParticleObject->GetCameraRelativeRotation().x);
}

void CEffectEditor::OnRipplePreset()
{
    if (!m_ParticleClass)
        return;

    // Move X
    m_ParticleClass->SetMove(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(false);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Particle Shape X
    m_ParticleClass->SetParticleShapeType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(-1);

    // Rotation Angle 90, 0, 0
    OnMinSeperateRotAngleEdit(Vector3(90.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(90.f, 0.f, 0.f));

    // Alpha
    OnAlphaStartEdit(1.f);
    OnAlphaEndEdit(0.2f);
    OnSetAlphaBlendToMaterialCallback();

    // Scale
    OnScaleMinEdit(Vector3(10.f, 10.f, 1.f));
    OnScaleMaxEdit(Vector3(50.f, 50.f, 1.f));

    // Start Min, Max
    OnStartMinEdit(Vector3(-0.001f, -0.001f, -0.001f));
    OnStartMaxEdit(Vector3(0.001f, 0.001f, 0.001f));

    // Spawn Time
    OnSpawnTimeMaxEdit(0.5f);

    // Life Time
    OnLifeTimeMinEdit(5.f);
    OnLifeTimeMaxEdit(5.f);

    // Life Time Linear X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Disable New Alive X
    m_ParticleClass->SetDisableNewAlive(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetDisableNewAlive(false);

    // Move Dir
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnRingPreset()
{
    if (!m_ParticleClass)
        return;

    // Move X
    m_ParticleClass->SetMove(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(false);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaStartEdit(1.f);
    OnAlphaEndEdit(0.3f);
    OnSetAlphaBlendToMaterialCallback();

    // Scale
    OnScaleMinEdit(Vector3(10.f, 10.f, 1.f)); //
    OnScaleMaxEdit(Vector3(20.f, 20.f, 1.f));

    // SpawnTime
    OnSpawnTimeMaxEdit(0.05f);

    // Life Time
    OnLifeTimeMinEdit(1.f);
    OnLifeTimeMaxEdit(2.f);

    // Life Time Linear X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);


    // Radiuse
    OnEditGenerateRadius(40.f);

    // Ring Loop O
    m_ParticleClass->SetLoopGenerateRing(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(true);
    
    // Particle Shape
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::YUpDirRing);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::YUpDirRing);

    // Disable New Alive X
    m_ParticleClass->SetDisableNewAlive(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetDisableNewAlive(false);

    // Special Move X
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnRingWallPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaStartEdit(1.f);
    OnAlphaEndEdit(0.7f);
    OnSetAlphaBlendToMaterialCallback();

    // Random Dir
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Scale
    OnScaleMinEdit(Vector3(15.f, 15.f, 1.f)); 
    OnScaleMaxEdit(Vector3(10.f, 10.f, 1.f));

    // Speed
    OnSpeedStartEdit(20.f);
    OnSpeedEndEdit(20.f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.02f);

    // Life Time
    OnLifeTimeMinEdit(5.f);
    OnLifeTimeMaxEdit(5.f);
    
    // Life Time Linear X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Radiuse
    OnEditGenerateRadius(40.f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Disable New Alive X
    m_ParticleClass->SetDisableNewAlive(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetDisableNewAlive(false);

    // Ring Loop O
    m_ParticleClass->SetLoopGenerateRing(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(true);

    // Particle Shape
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::YUpDirRing);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::YUpDirRing);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnFireWidePreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);


    // Alpha
    OnAlphaEndEdit(1.f);
    OnAlphaStartEdit(0.6f);
    OnSetAlphaBlendToMaterialCallback();

    // Special move Dir X
      m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Radiuse
    OnEditGenerateRadius(80.f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.02f);

    // Scale
    OnScaleMinEdit(Vector3(20.f, 20.f, 1.f));
    OnScaleMaxEdit(Vector3(20.f, 20.f, 1.f));

    // Life Time
    OnLifeTimeMinEdit(1.f);
    OnLifeTimeMaxEdit(10.f);

    // Life Time Linaer
    m_ParticleClass->SetLifeTimeLinearFromCenter(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(true);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Particle Shape
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::Circle);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::Circle);

    // Special Move X
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnSparkPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(true);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaEndEdit(1.f);
    OnAlphaStartEdit(0.8f);
    OnSetAlphaBlendToMaterialCallback();

    // Random Dir O
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.02f);
    
    // Life Time
    OnLifeTimeMinEdit(0.5f);
    OnLifeTimeMaxEdit(2.f);

    // Life Time Linear X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Speed
    OnSpeedStartEdit(50.f);
    OnSpeedEndEdit(80.f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    //Particle Shape X
    m_ParticleClass->SetParticleShapeType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(-1);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnSparkBouncePreset()
{
    if (!m_ParticleClass)
        return;

    OnLifeTimeMaxEdit(3.f);

    OnSparkPreset();

    m_ParticleClass->SetBounceEnable(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(true);

    m_ParticleClass->SetBounceResistance(0.7f);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceResist(0.7f);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnSimpleMeteorPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Alpha
    OnAlphaEndEdit(0.2f);
    OnAlphaStartEdit(2.f);
    OnSetAlphaBlendToMaterialCallback();

    // Special Move X
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Radius
    OnEditGenerateRadius(6.f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.02f);

    // Life Time
    OnLifeTimeMinEdit(0.5f);
    OnLifeTimeMaxEdit(2.f);

    // Speed
    OnSpeedStartEdit(10.f);
    OnSpeedEndEdit(30.f);

    // Life Time Linaer
    m_ParticleClass->SetLifeTimeLinearFromCenter(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(true);

    // Diable X
    m_ParticleClass->SetDisableNewAlive(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetDisableNewAlive(false);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 15.f));

    // Circle Type
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::Circle);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::Circle);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}


void CEffectEditor::OnFireTorchPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bound Flase
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaStartEdit(2.f);
    OnAlphaEndEdit(1.4f);
    OnSetAlphaBlendToMaterialCallback();

    // Circle Type
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::Circle);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::Circle);

    // Ring Loop X
    m_ParticleClass->SetLoopGenerateRing(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(false);

    // Special Move X
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Radius
    OnEditGenerateRadius(4.2f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.001f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(30.f, 0.f, 0.f));

    // Scale,
    OnScaleMinEdit(Vector3(10.f, 10.f, 1.f));
    OnScaleMaxEdit(Vector3(10.f, 20.f, 1.f));

    // Speed
    OnSpeedStartEdit(5.f);
    OnSpeedEndEdit(10.f);

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(0.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(1.f);
    OnLifeTimeMaxEdit(2.f);

    // Diable X
    m_ParticleClass->SetDisableNewAlive(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetDisableNewAlive(false);


    // Speed
    OnSpeedStartEdit(5.f);
    OnSpeedEndEdit(10.f);

    // Life Time Linaer
    m_ParticleClass->SetLifeTimeLinearFromCenter(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(true);

    // Alpha Linaer
    m_ParticleClass->SetAlphaLinearFromCenter(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetAlphaLinearFromCenter(true);

    // Rot To Dir X
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);

    // Move Dir Type X
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnFireGenerateMomentPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bound Flase
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaStartEdit(1.5f);
    OnAlphaEndEdit(0.8f);
    OnSetAlphaBlendToMaterialCallback();

    // Circle Type
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::Circle);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::Circle);

    // Ring Loop X
    m_ParticleClass->SetLoopGenerateRing(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(false);

    // Random Dir
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Radius
    OnEditGenerateRadius(25.9f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.005f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Scale,
    OnScaleMinEdit(Vector3(40.f, 40.f, 1.f));
    OnScaleMaxEdit(Vector3(20.f, 20.f, 1.f));

    // Speed
    OnSpeedStartEdit(10.f);
    OnSpeedEndEdit(30.f);

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(0.05f);
    OnLifeTimeMaxEdit(2.f);

    // Diable X
    m_ParticleClass->SetDisableNewAlive(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetDisableNewAlive(false);


    // Speed
    OnSpeedStartEdit(0.01f);
    OnSpeedEndEdit(30.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Alpha Linaer
    m_ParticleClass->SetAlphaLinearFromCenter(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetAlphaLinearFromCenter(true);

    // Rot To Dir X
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);

    // Move Dir Type XZSpread
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnBloodEachParticlePreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity O
    m_ParticleClass->SetGravity(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(true);

    // Bound Flase
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaStartEdit(1.f);
    OnAlphaEndEdit(1.f);
    OnSetAlphaBlendToMaterialCallback();

    // Shapw X
    m_ParticleClass->SetParticleShapeType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(-1);

    // Ring Loop X
    m_ParticleClass->SetLoopGenerateRing(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(false);

    // Random Dir
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Radius
    OnEditGenerateRadius(25.9f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.01f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 0.f, -1.f));

    // Speed
    OnSpeedStartEdit(30.f);
    OnSpeedEndEdit(80.f);

    // Start , End Pos
    OnStartMinEdit(Vector3(0.f, 0.f, 0.f));
    OnStartMaxEdit(Vector3(0.f, 0.f, 0.f));

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(0.5f);
    OnLifeTimeMaxEdit(1.f);

    // Speed
    OnSpeedStartEdit(1.f);
    OnSpeedEndEdit(100.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Alpha Linaer
    m_ParticleClass->SetAlphaLinearFromCenter(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetAlphaLinearFromCenter(true);

    // Rot To Dir X
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);

    // Move Dir Type 
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnXYBloodSpreadPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bound X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaStartEdit(1.f);
    OnAlphaEndEdit(1.f);
    OnSetAlphaBlendToMaterialCallback();

    // Shapw X
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::ZMinusDirRing);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::ZMinusDirRing);

    // Ring Loop X
    m_ParticleClass->SetLoopGenerateRing(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(false);

    // Radius 
    OnEditGenerateRadius(15.9f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.02f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 0.f, -1.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 150.f));

    // Scale,
    OnScaleMinEdit(Vector3(10.f, 10.f, 1.f));
    OnScaleMaxEdit(Vector3(10.f, 50.f, 1.f));

    // Start , End Pos
    OnStartMinEdit(Vector3(0.f, 0.f, 0.f));
    OnStartMaxEdit(Vector3(0.f, 0.f, 0.f));

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(0.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(0.2f);
    OnLifeTimeMaxEdit(1.f);

    // Speed
    OnSpeedStartEdit(5.f);
    OnSpeedEndEdit(20.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Alpha Linaer X
    m_ParticleClass->SetAlphaLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetAlphaLinearFromCenter(false);

    // Rot To Dir True
    m_ParticleClass->SetRotToDir(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(true);

    // Move Dir Type 
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XYSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XYSpread);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}


// XZ Spread Circle
void CEffectEditor::OnXZSpreadGrassPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bound X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnAlphaStartEdit(1.f);
    OnAlphaEndEdit(0.3f);
    OnSetAlphaBlendToMaterialCallback();

    // Shapw X
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::YUpDirRing);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::YUpDirRing);

    // Ring Loop X
    m_ParticleClass->SetLoopGenerateRing(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLoopGenerateRing(false);

    // Radius 
    OnEditGenerateRadius(15.9f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.01f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 0.3f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Scale,
    OnScaleMinEdit(Vector3(1.f, 1.f, 1.f));
    OnScaleMaxEdit(Vector3(15.f, 1.f, 1.f));

    // Start , End Pos
    OnStartMinEdit(Vector3(0.f, 0.f, 0.f));
    OnStartMaxEdit(Vector3(0.f, 0.f, 0.f));

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(90.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(90.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(0.05f);
    OnLifeTimeMaxEdit(0.7f);

    // Speed
    OnSpeedStartEdit(1.f);
    OnSpeedEndEdit(5.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Alpha Linaer X
    m_ParticleClass->SetAlphaLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetAlphaLinearFromCenter(false);

    // Rot To Dir True
    m_ParticleClass->SetRotToDir(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(true);


    // Move Dir Type 
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnFireSmokeUpPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // UV Move False
    m_ParticleClass->SetUVMoveEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetUVMoveEnable(false);

    // Alpha
    OnAlphaEndEdit(1.1f);
    OnAlphaStartEdit(0.2f);
    OnSetAlphaBlendToMaterialCallback();

    // Particle Shape X
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.05f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 30.f));

    // Scale,
    OnScaleMinEdit(Vector3(20.f, 20.f, 1.f));
    OnScaleMaxEdit(Vector3(1.f, 1.f, 1.f));

    // Speed
    OnSpeedStartEdit(10.f);
    OnSpeedEndEdit(30.f);

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(3.f);
    OnLifeTimeMaxEdit(6.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Shape Type X
    m_ParticleClass->SetParticleShapeType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(-1);

    // Rot To Dir True
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);


    // Move Dir Type
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnFireCrackerPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnSetAlphaBlendToMaterialCallback();

    // Particle Shape X
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::Circle);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::Circle);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.05f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Scale,
    OnScaleMinEdit(Vector3(5.f, 5.f, 1.f));
    OnScaleMaxEdit(Vector3(1.f, 1.f, 1.f));

    // Speed
    OnSpeedStartEdit(180.f);
    OnSpeedEndEdit(0.f);

    // Particle Speed Change
    m_ParticleClass->SetSpeedChangeMethod(ParticleSpeedChangeMethod::Log);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpeedChangeMethod(ParticleSpeedChangeMethod::Log);

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(0.01f);
    OnLifeTimeMaxEdit(3.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Rot To Dir True
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);

    // Move Dir Type
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnSummonEffectPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bounce X
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // Alpha
    OnSetAlphaBlendToMaterialCallback();

    // Particle Shape 
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::YUpDirRing);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::YUpDirRing);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.01f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Scale,
    OnScaleMinEdit(Vector3(5.f, 5.f, 1.f));
    OnScaleMaxEdit(Vector3(1.f, 1.f, 1.f));

    // Speed
    OnSpeedStartEdit(1.f);
    OnSpeedEndEdit(100.f);

    // Particle Speed Change
    m_ParticleClass->SetSpeedChangeMethod(ParticleSpeedChangeMethod::Exponential);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpeedChangeMethod(ParticleSpeedChangeMethod::Exponential);

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(1.f);
    OnLifeTimeMaxEdit(7.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Rot To Dir True
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);

    // Move Dir Type
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnBettyAttackSpreadDustPreset()
{
    if (!m_ParticleClass)
        return;

    OnXZSpreadDustPreset();

    // Speed
    OnSpeedStartEdit(50.f);
    OnSpeedEndEdit(20.f);

    // Speed Method Type
    m_ParticleClass->SetSpeedChangeMethod(ParticleSpeedChangeMethod::Log);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpeedChangeMethod(ParticleSpeedChangeMethod::Log);

    // Color
    OnColorMinEdit(Vector4(250.f, 248.f, 235.f, 255.f));
    OnColorMinEdit(Vector4(155.f, 204.f, 255.f, 255.f));

    // Alpha
    OnAlphaEndEdit(1.1f);
    OnAlphaStartEdit(1.f);

    // Noise Texture ����� ȿ��
    m_ParticleClass->SetApplyNoiseTextureSamplingEnable(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetApplyNoiseTextureSamplingEnable(true);

    // Linaer UV Destroy X
    m_ParticleClass->SetLinearUVClippingEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLinearUVClippingEnable(false);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnPlayerDustPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bound Flase
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // UV Move False
    m_ParticleClass->SetUVMoveEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetUVMoveEnable(false);

    // Alpha
    OnAlphaEndEdit(1.1f);
    OnAlphaStartEdit(0.5f);
    OnSetAlphaBlendToMaterialCallback();

    // Alpha Linaer X
    m_ParticleClass->SetAlphaLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetAlphaLinearFromCenter(false);

    // SpawnTime
    OnSpawnTimeMaxEdit(1.f);

    // Move Dir
    OnMoveDirEdit(Vector3(1.f, 0.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Scale,
    OnScaleMinEdit(Vector3(10.f, 10.f, 1.f));
    OnScaleMaxEdit(Vector3(30.f, 30.f, 1.f));

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(0.5f);
    OnLifeTimeMaxEdit(0.5f);

    // Speed
    OnSpeedStartEdit(20.f);
    OnSpeedEndEdit(20.f);

    // Start Min, Max
    OnStartMinEdit(Vector3(0.f, 0.f, 0.f));
    OnStartMaxEdit(Vector3(0.f, 0.f, 0.f));

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Circle Type
    m_ParticleClass->SetParticleShapeType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(-1);

    // Rot To Dir X
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);

    // Move Dir Type X
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Noise Texture True
    m_ParticleClass->SetApplyNoiseTextureSamplingEnable(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetApplyNoiseTextureSamplingEnable(true);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}

void CEffectEditor::OnXZSpreadDustPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Bound Flase
    m_ParticleClass->SetBounceEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetBounceEnable(false);

    // UV Move False
    m_ParticleClass->SetUVMoveEnable(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetUVMoveEnable(false);

    // Alpha
    OnAlphaEndEdit(1.1f);
    OnAlphaStartEdit(0.2f);
    OnSetAlphaBlendToMaterialCallback();

    // Random Dir
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Radius
    OnEditGenerateRadius(1.2f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.02f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 0.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Scale,
    OnScaleMinEdit(Vector3(50.f, 50.f, 1.f));
    OnScaleMaxEdit(Vector3(5.f, 5.f, 1.f));

    // Speed
    OnSpeedStartEdit(50.f);
    OnSpeedEndEdit(50.f);

    // Rotation Angle
    OnMinSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));
    OnMaxSeperateRotAngleEdit(Vector3(45.f, 0.f, 0.f));

    // Life Time
    OnLifeTimeMinEdit(0.5f);
    OnLifeTimeMaxEdit(2.f);

    // Speed
    OnSpeedStartEdit(10.f);
    OnSpeedEndEdit(30.f);

    // Life Time Linaer X
    m_ParticleClass->SetLifeTimeLinearFromCenter(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(false);

    // Circle Type
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::YUpDirRing);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::YUpDirRing);

    // Rot To Dir True
    m_ParticleClass->SetRotToDir(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetRotToDir(false);

    // Move Dir Type
    m_ParticleClass->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(ParticleSpecialMoveDir::XZSpread);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}



/*
void CEffectEditor::OnTorchPreset()
{
    if (!m_ParticleClass)
        return;

    // Move O
    m_ParticleClass->SetMove(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetMove(true);

    // Gravity X
    m_ParticleClass->SetGravity(false);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetGravity(false);

    // Alpha
    OnAlphaStartEdit(1.f);
    OnAlphaEndEdit(0.7f);
    OnSetAlphaBlendToMaterialCallback();

    // Random Dir
    m_ParticleClass->SetSpecialMoveDirType(-1);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetSpecialMoveDirType(-1);

    // Radiuse
    OnEditGenerateRadius(80.f);

    // SpawnTime
    OnSpawnTimeMaxEdit(0.02f);

    // Scale
    OnScaleMinEdit(Vector3(20.f, 20.f, 1.f));
    OnScaleMaxEdit(Vector3(20.f, 20.f, 1.f));

    // Life Time
    OnLifeTimeMinEdit(2.f);
    OnLifeTimeMaxEdit(10.f);

    // Move Dir
    OnMoveDirEdit(Vector3(0.f, 1.f, 0.f));

    // Move Angle
    OnMoveAngleEdit(Vector3(0.f, 0.f, 0.f));

    // Life Time Linaer
    m_ParticleClass->SetLifeTimeLinearFromCenter(true);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetLifeTimeLinearFromCenter(true);

    // Particle Shape
    m_ParticleClass->SetParticleShapeType(ParitcleShapeType::Torch);
    dynamic_cast<CParticleComponent*>(m_ParticleObject->GetRootComponent())->GetCBuffer()->SetParticleShapeType(ParitcleShapeType::Torch);

    // IMGUI Update
    SetIMGUIReflectParticle(m_ParticleClass);
}
*/