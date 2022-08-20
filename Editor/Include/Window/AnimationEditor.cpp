// IMGUI
#include "IMGUIButton.h"
#include "IMGUIComboBox.h"
#include "IMGUICheckBox.h"
#include "IMGUIDummy.h"
#include "IMGUIImage.h"
#include "IMGUILabel.h"
#include "IMGUIListBox.h"
#include "IMGUISameLine.h"
#include "IMGUIProgressBar.h"
#include "IMGUIChild.h"
#include "IMGUIText.h"
#include "IMGUITable.h"
#include "IMGUITextInput.h"
#include "IMGUISliderFloat.h"
#include "IMGUISliderInt.h"
// Window
#include "AnimationEditor.h"
#include "AnimationDisplayWindow.h"
#include "../EditorManager.h"
#include "../Object/Anim3DObject.h"
#include "../AnimationInstanceConvertThread.h"
#include "../EditorUtil.h"

// Engine
#include "EngineUtil.h"
#include "PathManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Animation/AnimationSequenceInstance.h"
#include "Engine.h"
#include "Device.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Resource/Excel/ExcelManager.h"
// Library
#include <charconv>

CAnimationEditor::CAnimationEditor() :
	m_RenderTargetSet(false)
{
}

CAnimationEditor::~CAnimationEditor()
{
	SAFE_DELETE(m_DummyAnimation);
}

bool CAnimationEditor::Init()
{
	if (!CIMGUIWindow::Init())
		return false;

	// Anim Name
	// CIMGUILabel* Label = AddWidget<CIMGUILabel>("Anim Name", 100.f, 30.f);
	// Label->SetColor(0, 0, 255);
	// Label->SetAlign(0.5f, 0.0f);

	m_CurAnimComboBox = AddWidget<CIMGUIComboBox>("Anim List Combo Box", 300.f, 30.f);
	m_CurAnimComboBox->SetHideName(true);
	m_CurAnimComboBox->SetSelectCallback<CAnimationEditor>(this, &CAnimationEditor::OnClickAnimationSequence);
	// m_CurAnimComboBox->SetSelectIndexCallback<CAnimationEditor>(this, &CAnimationEditor::OnSetAnimationComboBoxCallback);

	// ���� Render Target
	// m_AnimationRenderTarget = AddWidget<CIMGUIImage>("Render Target", 400.f, 400.f);
	// m_AnimationRenderTarget->SetRenderTargetImage(true);
	// m_AnimationRenderTarget->SetBorderColor(10, 10, 255);
	// m_AnimationRenderTarget->SetTableTitle("Render Target");

	// Clip Info
	m_AnimInfoTable = AddWidget<CIMGUITable>("AnimTable", 600.f, 200.f);
	m_AnimInfoTable->SetTableTitle("Animation Info");

	// Frame Slider 
	m_FrameSlider = AddWidget<CIMGUISliderInt>("Frame Slider", 110.f, 30.f);
	m_FrameSlider->SetCallBack<CAnimationEditor>(this, &CAnimationEditor::OnAnimationSliderIntCallback);

	m_FrameInput = AddWidget<CIMGUITextInput>("Frame Input", 110.f, 30.f);
	m_FrameInput->SetTextType(ImGuiText_Type::Int);
	m_FrameInput->SetCallback<CAnimationEditor>(this, &CAnimationEditor::OnAnimationFrameInputCallback);

	// Frame Edit
	m_StartFrameEditInput = AddWidget<CIMGUITextInput>("Edit Start Frame", 110.f, 30.f);
	m_StartFrameEditInput->SetTextType(ImGuiText_Type::Int);

	m_EndFrameEditInput = AddWidget<CIMGUITextInput>("Edit End Frame", 110.f, 30.f);
	m_EndFrameEditInput->SetTextType(ImGuiText_Type::Int);

	m_StartEndFrameEditBtn = AddWidget<CIMGUIButton>("Edit Frame Save", 110.f, 20.f);
	m_StartEndFrameEditBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnEditStartEndFrame);


	// Edit  -------------------------------------------------------
	// Play Scale ����
	CIMGUIDummy* Dummy = AddWidget<CIMGUIDummy>("Dummy", 200.f, 30.f);

	m_PlayScaleInput = AddWidget<CIMGUITextInput>("Play Scale Input", 50.f, 30.f);
	m_PlayScaleInput->SetHideName(true);
	m_PlayScaleInput->SetTextType(ImGuiText_Type::Float);

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(60.f);

	m_PlayScaleEditBtn = AddWidget<CIMGUIButton>("Edit Scale", 100.f, 20.f);
	m_PlayScaleEditBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnEditAnimPlayScale);

	// Play Time ����
	m_PlayTimeInput = AddWidget<CIMGUITextInput>("Play Time Input", 50.f, 30.f);
	m_PlayTimeInput->SetHideName(true);
	m_PlayTimeInput->SetTextType(ImGuiText_Type::Float);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(60.f);

	// PlayTime Edit
	m_PlayTimeEditBtn = AddWidget<CIMGUIButton>("Edit Time", 100.f, 20.f);
	m_PlayTimeEditBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnEditAnimPlayTime);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(165.f);

	// Sequence ���� Frame �� �ݿ��ؼ� PlayTime �� ���������ִ� �Լ� 
	m_SetOriginalPlayTimeBtn = AddWidget<CIMGUIButton>("Set Origin Time", 100.f, 20.f);
	m_SetOriginalPlayTimeBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnSetOriginalAnimPlayTime);

	// ���� üũ �ڽ��� 
	// m_DeltaTimeCheckBtn = AddWidget<CIMGUICheckBox>("Engine Play", 90.f, 30.f);
	// m_DeltaTimeCheckBtn->AddCheckInfo("Engine Play");
	// m_DeltaTimeCheckBtn->SetCallBackLabel<CAnimationEditor>(this, &CAnimationEditor::OnSetPlayEngineDeltaTime);
	// // m_DeltaTimeCheckBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnSetPlayEngineDeltaTime);
	// 
	// Line = AddWidget<CIMGUISameLine>("Line");
	// Line->SetOffsetX(110.f);

	m_AnimationCheckBtn = AddWidget<CIMGUICheckBox>("Object Play", 90.f, 30.f);
	m_AnimationCheckBtn->AddCheckInfo("Anim Play");
	m_AnimationCheckBtn->SetCallBackLabel<CAnimationEditor>(this, &CAnimationEditor::OnPlayAnimation);
	// m_AnimationCheckBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnPlayAnimation);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(110);

	m_LoopEnableBtn = AddWidget<CIMGUICheckBox>("Loop", 90.f, 30.f);
	m_LoopEnableBtn->AddCheckInfo("Loop");
	m_LoopEnableBtn->SetCallBackLabel<CAnimationEditor>(this, &CAnimationEditor::OnLoopAnimation);

	// Camera
	m_RotationCheckBtn = AddWidget<CIMGUICheckBox>("Camera Rotation", 90.f, 30.f);
	m_RotationCheckBtn->AddCheckInfo("Rotate");
	m_RotationCheckBtn->SetCallBackLabel<CAnimationEditor>(this, &CAnimationEditor::OnRotateAnimationCamera);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(110.f);

	m_ZoomEnableBtn = AddWidget<CIMGUICheckBox>("Camera Zoom In", 90.f, 30.f);
	m_ZoomEnableBtn->AddCheckInfo("Zoom");
	m_ZoomEnableBtn->SetCallBackLabel<CAnimationEditor>(this, &CAnimationEditor::OnZoomAnimationCamera);

	// Sequence Make Widgets ----------------------------------------------------------------------------------------------------------------
	CIMGUIText* HelpText = AddWidget<CIMGUIText>("Anim Seq Load Btn Help Text", 90.f, 30.f);
	const char* AddSeqHelpText = R"(ex)  'ZedIdle' -- > pair('ZedIdle', 'ZedIdle.sqc') ���·� 
	SceneResource, ResourceManager�� m_mapSequence �� ����")";
	HelpText->SetText(AddSeqHelpText);
	HelpText->SetIsHelpMode(true);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	HelpText = AddWidget<CIMGUIText>("Anim Seq Load Btn Help Text", 90.f, 30.f);
	const char* ResourceAnimKeyHelpText = R"(ex) 'Idle' --> m_Animation->AddAnimation('ZedIdle', 'Idle') ? 
ZedIdle Key�� ResourceManager �� mapSequence �� ����� Animation Sequence �� 
'Idle' �̶�� �̸��� Key������ AnimationInstance �� ���� �߰�)";
	HelpText->SetText(ResourceAnimKeyHelpText);
	HelpText->SetIsHelpMode(true);

	// Seq Name Inputs
	m_NewAnimSeqName = AddWidget<CIMGUITextInput>("Sequence Name", 90.f, 30.f);
	m_NewAnimSeqName->SetHideName(true);
	m_NewAnimSeqName->SetHintText("Map Key");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	m_NewAnimSeqDataKeyName = AddWidget<CIMGUITextInput>("Sequence  Key", 90.f, 30.f);
	m_NewAnimSeqDataKeyName->SetHideName(true);
	m_NewAnimSeqDataKeyName->SetHintText("Anim Key");

	HelpText = AddWidget<CIMGUIText>("Anim Key Name Edit Help", 90.f, 30.f);
	const char* EditKeyHelpText = R"(ex) 'EditIdle' --> ������ m_Animation->AddAnimation('ZedIdle', 'Idle') ���� ���� ������� m_mapAnimationSequence['Idle'] = 'ZedIdle' �� 
m_mapAnimationSequence['EditIdle'] = 'ZedIdle' �� Key �� ���� )";
	HelpText->SetText(EditKeyHelpText);
	HelpText->SetIsHelpMode(true);

	// Animation Instance Key Name ���� ���
	m_EditAnimSeqDataKeyName = AddWidget<CIMGUITextInput>("Edit Sequence Key", 90.f, 30.f);
	m_EditAnimSeqDataKeyName->SetHideName(true);
	m_EditAnimSeqDataKeyName->SetHintText("Edit Key");

	// Line = AddWidget<CIMGUISameLine>("Line");
	// Line->SetOffsetX(290.f);
	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	m_EditAnimKeyBtn = AddWidget<CIMGUIButton>("Edit Key", 90.f, 20.f);
	m_EditAnimKeyBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnEditAnimSequenceKey);

	// Sequence Related Btns
	HelpText = AddWidget<CIMGUIText>("Anim Seq Load Btn Help Text", 90.f, 30.f);
	HelpText->SetText("���� ��ο� .msh , .bne, .fbm(����) �� �����ؾ� �Ѵ�. \n ex) Idle1.sqc �� Add �Ϸ���, Idle1.msh, Idle1.bne, Idle1.fbm�� ���� ��� ���� �����ؾ� �Ѵ�.");
	HelpText->SetIsHelpMode(true);

	m_AnimSequenceAddBtn = AddWidget<CIMGUIButton>("Add Seq", 90.f, 20.f);
	m_AnimSequenceAddBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnAddAnimationSequence);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	m_DeleteAnimSequenceBtn = AddWidget<CIMGUIButton>("Delete Seq", 90.f, 20.f);
	m_DeleteAnimSequenceBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnDeleteAnimationSequenceData);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(195.f);

	m_DeleteAnimObject = AddWidget<CIMGUIButton>("Delete Obj", 90.f, 20.f);
	m_DeleteAnimObject->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnDeleteAnimation3DObject);

	// Save, Load
	// Animation Related Btns
	HelpText = AddWidget<CIMGUIText>("Instance Save Btn Help Text", 90.f, 30.f);
	HelpText->SetText("Bin\\Animation ������ .anim ���� Ȯ���ڷ� �����ؾ� �Ѵ�.");
	HelpText->SetIsHelpMode(true);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	// Animation Related Btns
	HelpText = AddWidget<CIMGUIText>("Instance Load Btn Help Text", 90.f, 30.f);
	const char* InstLoadHelpText = R"(.anim ������ Load �Ϸ���, MESH_PATH (Bin//Mesh) ��ο� ���� .msh , .bne, .fbm(����) �� �����ؾ� �Ѵ�. 
ex) Alistar.anim �� Load �Ϸ���, 
MESH_PATH �� Alistar_Idle.sqc, Alistar_Idle.msh, Alistar_Idle.fbm ��, 
Alistar Animation �� ���õ� ���ϵ��� �ϳ��� ������� �Ѵ�)";
	HelpText->SetText(InstLoadHelpText);
	HelpText->SetIsHelpMode(true);
	
	m_SaveAnimationInstanceBtn = AddWidget<CIMGUIButton>("Save Instance", 90.f, 20.f);
	m_SaveAnimationInstanceBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnSaveAnimationInstance);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	m_LoadAnimationInstanceBtn = AddWidget<CIMGUIButton>("Load Instance", 90.f, 20.f);
	m_LoadAnimationInstanceBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnLoadAnimationInstance);

	m_LoadedAnimFileName = AddWidget<CIMGUITextInput>("Loaded FileName", 185.f, 30.f);
	m_LoadedAnimFileName->SetHintText("Loaded FileName");

	// Table Key �� ���� ����
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameRange);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameLength);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameMode);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::PlayTime);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameTime);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::PlayScale);

	// Animation ��Ƽ� �����
	 // Anim Instance Making
	Dummy = AddWidget<CIMGUIDummy>("Dummy", 100.f, 20.f);
	CIMGUIText* Text = AddWidget<CIMGUIText>("Create Instance", 100.f, 30.f);
	Text->SetText("Create Instance");

	// Common Name
	m_SavedAnimFileName = AddWidget<CIMGUITextInput>("Saved File Name", 150.f, 20.f);
	m_SavedAnimFileName->SetHideName(true);
	m_SavedAnimFileName->SetHintText("Saved File Name");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	HelpText = AddWidget<CIMGUIText>("Anim Seq Load Btn Help Text", 90.f, 30.f);
	HelpText->SetText("ZedIdle �̶�� �ϸ�, Bin/Animation ������ ZedIdle.anim �̸����� Animation Instance ����");
	HelpText->SetIsHelpMode(true);

	m_CommonAnimSeqName = AddWidget<CIMGUITextInput>("Common Sqc Name", 150.f, 20.f);
	m_CommonAnimSeqName->SetHideName(true);
	m_CommonAnimSeqName->SetHintText("Common Sqc Name");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(165.f);

	m_SelectAnimInstanceFolderPath = AddWidget<CIMGUIButton>("Select Dir", 90.f, 20.f);
	m_SelectAnimInstanceFolderPath->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnClickSetAnimSeqSrcDirButton);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(260.f);

	m_ConvertAnimInstanceBtn = AddWidget<CIMGUIButton>("Make Inst", 90.f, 20.f);
	m_ConvertAnimInstanceBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnConvertSequencesIntoAnimationInstance);

	// Folder Path
	m_AnimSeqcSrcFolderPath = AddWidget<CIMGUITextInput>("Sqc Folder Path", 350.f, 20.f);
	m_AnimSeqcSrcFolderPath->ReadOnly(true);
	m_AnimSeqcSrcFolderPath->SetHideName(true);
	m_AnimSeqcSrcFolderPath->SetHintText("Set .sqc Folder Path");

	// Convert
	m_AnimInstanceProgressBar = AddWidget<CIMGUIProgressBar>("", 350.f, 0.f);

	m_AnimInstanceConvertLog = AddWidget<CIMGUIChild>("Log", 200.f, 200.f);
	m_AnimInstanceConvertLog->EnableBorder(true);

	// Excel Btn ----------------------------------------------------------------------------------------------------------------
	Dummy = AddWidget<CIMGUIDummy>("Dummy", 200.f, 30.f);

	Text = AddWidget<CIMGUIText>("Excel Functions", 100.f, 30.f);
	Text->SetText("Excel");

	// Load �� Excel File Name
	m_LoadedExcelFileName = AddWidget<CIMGUITextInput>("Loaded Excel Data Name", 150.f, 30.f); // �� ù��° �� 
	m_LoadedExcelFileName->SetHideName(true);
	m_LoadedExcelFileName->SetHintText("Loaded Excel Name");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	HelpText = AddWidget<CIMGUIText>("Loaded Excel Data", 90.f, 30.f);
	HelpText->SetText("Load �� Excel �� Data Name (ù��° ��)");
	HelpText->SetIsHelpMode(true);

	// Excel ���뿡 �����Ͽ� ���� .anim ���� �̸�
	m_ExcelSavedAnimFileName = AddWidget<CIMGUITextInput>("Saved Anim Name", 150.f, 30.f); // �� ù��° �� 
	m_ExcelSavedAnimFileName->SetHideName(true);
	m_ExcelSavedAnimFileName->SetHintText("Saved Anim Name");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	HelpText = AddWidget<CIMGUIText>("Excel Saved Anim Name", 90.f, 30.f);
	HelpText->SetText("���� .anim ������ �̸� -> .anim Ȯ���� ����, ���� �̸��� �Է��ϼ���");
	HelpText->SetIsHelpMode(true);

	m_LoadExcelBtn = AddWidget<CIMGUIButton>("Load Excel", 150.f, 20.f);
	m_LoadExcelBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnLoadExcel);

	m_MakeAnimInstByExcelBtn = AddWidget<CIMGUIButton>("Make Inst From Excel", 150.f, 20.f);
	m_MakeAnimInstByExcelBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnMakeAnimInstByExcel);

	return true;
}

void CAnimationEditor::Update(float DeltaTime)
{
	CIMGUIWindow::Update(DeltaTime);

	// Animation Slider ����
	if (m_Animation && m_Animation->GetCurrentAnimation())
	{
		int NextAnimationIdx = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetCurrentFrameIdx();
		OnAnimationSliderIntCallback(NextAnimationIdx);
	}

	// ���� RenderTarget ���� (ó�� �ѹ���)
	// if (!m_RenderTargetSet)
	// {
	// 	m_AnimationRenderTarget->SetTexture(CRenderManager::GetInst()->GetAnimationRenderTarget());
	// 	// m_AnimationRenderTarget->SetTexture(CResourceManager::GetInst()->FindTexture("FinalScreen"));
	// 	m_RenderTargetSet = true;
	// }
}

void CAnimationEditor::OnDeleteExisting3DObject()
{
	m_3DTestObject->Destroy();

	m_3DTestObject = nullptr;

	m_Animation = nullptr;

	if (!m_Animation)
		return;

	// Combo Box Clear
	m_CurAnimComboBox->Clear();

	// Table Claer
	m_AnimInfoTable->ClearContents();
}

void CAnimationEditor::OnClearExistingAnimationSeqInfos()
{
	if (!m_Animation)
		return;

	m_Animation->ClearAnimationSequenceFromAnimationEditor();
	
	// Animation Mesh Component ���� �ٽ� ���Ӱ� Animation Instance �� ������.
	m_Animation = dynamic_cast<CAnimationMeshComponent*>(m_3DTestObject->GetRootComponent())->CreateBasicAnimationInstance();

	// Combo Box Clear
	m_CurAnimComboBox->Clear();

	// Table Claer
	m_AnimInfoTable->ClearContents();
}

void CAnimationEditor::OnCreateSample3DObject()
{
	if (m_3DTestObject)
		return;

	m_3DTestObject = CSceneManager::GetInst()->GetScene()->CreateGameObject<CAnim3DObject>("3DObject");

	m_3DTestObject->ExcludeFromSceneSave();
	m_3DTestObject->SetNoDestroyOnSceneChange(true);

	// 3DTest Object�� Animation ������ �����´�.
	CAnimationMeshComponent* AnimComponent = dynamic_cast<CAnimationMeshComponent*>(m_3DTestObject->GetRootComponent());

	if (AnimComponent == nullptr)
	{
		assert(false);
	}

	m_Animation = AnimComponent->CreateBasicAnimationInstance();
}

void CAnimationEditor::OnPlayAnimation(const char* Lable, bool Enable)
{
	if (!m_3DTestObject)
		return;

	CAnimationMeshComponent* RootMeshComponent = dynamic_cast<CAnimationMeshComponent*>(m_3DTestObject->GetRootComponent());

	if (!RootMeshComponent)
		return;

	bool IsAnimPlay = RootMeshComponent->GetAnimationInstance()->IsPlay();

	// if (IsAnimPlay)
	if (!Enable)
		RootMeshComponent->GetAnimationInstance()->Stop();
	else
		RootMeshComponent->GetAnimationInstance()->Play();
}

void CAnimationEditor::OnRotateAnimationCamera(const char*, bool Enable)
{
	if (!m_3DTestObject)
		return;

	// if (IsAnimPlay)
	if (Enable)
		m_3DTestObject->SetCameraRot(true);
	else
		m_3DTestObject->SetCameraRot(false);
}

void CAnimationEditor::OnZoomAnimationCamera(const char*, bool Enable)
{
	if (!m_3DTestObject)
		return;

	// if (IsAnimPlay)
	if (Enable)
		m_3DTestObject->SetCameraZoom(true);
	else
		m_3DTestObject->SetCameraZoom(false);
}

void CAnimationEditor::OnLoopAnimation(const char*, bool Enable)
{
	if (!m_3DTestObject)
		return;

	CAnimationMeshComponent* RootMeshComponent = dynamic_cast<CAnimationMeshComponent*>(m_3DTestObject->GetRootComponent());

	if (!RootMeshComponent)
		return;

	RootMeshComponent->GetAnimationInstance()->SetLoop(Enable);

	// bool IsAnimPlay = RootMeshComponent->GetAnimationInstance()->IsPlay();
	// 
	// // if (IsAnimPlay)
	// if (!Enable)
	// 	RootMeshComponent->GetAnimationInstance()->Stop();
	// else
	// 	RootMeshComponent->GetAnimationInstance()->Play();
}

void CAnimationEditor::OnLoadExcel()
{
	TCHAR FilePath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Excel CSV\0*.csv");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(EXCEL_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char	FileExt[_MAX_EXT] = {};

		char FilePathMultibyte[MAX_PATH] = {};
		char FileName[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		_splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, FileName, MAX_PATH, FileExt, _MAX_EXT);

		_strupr_s(FileExt);

		// Ȯ���� .anim �� �ƴ϶�� return;
		if (strcmp(FileExt, ".CSV") != 0)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .csv"), NULL, MB_OK);
			return;
		}

		// Sub Folder �� ���� ��쿡��, Load  �� �� �ְ� �����ϱ� 
		std::string LoadedExcelKey;

		bool LoadResult = CResourceManager::GetInst()->GetExcelManager()->LoadCSVFullPath(LoadedExcelKey, FilePathMultibyte);

		if (!LoadResult)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Excel Load ����. Excel �� �������ִ��� Ȯ���غ�����"), NULL, MB_OK);
			return;
		}

		// ���� KeyName �� ���纻�� �����.
		std::string PrevExcelKeyName = m_ExcelKeyName;

		m_ExcelKeyName = LoadedExcelKey;

		CExcelData* LoadedExcelData = CResourceManager::GetInst()->GetExcelManager()->FindCSV(LoadedExcelKey);

		// Load �� �̸� ����
		std::string LoadedExcelName = LoadedExcelData->GetName();

		m_LoadedExcelFileName->SetText(LoadedExcelName.c_str());

		// ���� Excel �� Load �� �� �ƴ϶��, ������ ���� �����
		if (PrevExcelKeyName != LoadedExcelKey)
		{
			// Excel Data ���� ���Ӱ� ����
			// Excel Manager ���� ���� Excel ������ �����.
			CResourceManager::GetInst()->GetExcelManager()->DeleteCSV(PrevExcelKeyName);
		}

		m_LoadedExcelData = LoadedExcelData;
	}
}

void CAnimationEditor::OnMakeAnimInstByExcel()
{
	// Excel �� �����ؾ� �Ѵ�.
	if (!m_LoadedExcelData)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Excel ������ Load �ϼ���"), NULL, MB_OK);
		return;
	}

	// Animation �� �����ؾ� �Ѵ�.
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// m_ExcelSavedAnimFileName �� �̸��� ä���� �־�� �Ѵ�.
	if (m_ExcelSavedAnimFileName->Empty())
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT(".anim ���� �̸��� �����ϼ���"), NULL, MB_OK);
		return;
	}

	int OriginAnimStartFrame = 0;
	int OriginAnimEndFrame = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetFrameLength() - 1;

	// >> Excel ���� ��� sqc �������� ���鼭, ������ ����� ���������� Ȯ���Ѵ�. 
	const Table& ExcelTableData = m_LoadedExcelData->GetTable();

	{
		auto iter = ExcelTableData.begin();
		auto iterEnd = ExcelTableData.end();

		for (; iter != iterEnd; ++iter)
		{
			// 1��°�� startFrame
			// 2��°�� endFrame
			const std::string& StFrame = (*(iter->second))[0];
			int StFrameInt;
			auto StFrameIntInfo = std::from_chars(StFrame.c_str(), StFrame.c_str() + sizeof(StFrame.c_str()) - 1, StFrameInt);

			const std::string& EdFrame = (*(iter->second))[1];
			int EdFrameInt;
			auto EdFrameIntInfo = std::from_chars(EdFrame.c_str(), EdFrame.c_str() + sizeof(EdFrame.c_str()) - 1, EdFrameInt);

			const std::string& DataLable = iter->first;

			TCHAR FulllErrorMessage[MAX_PATH] = {};
			TCHAR TCHARDataLable[MAX_PATH] = {};
			lstrcpy(TCHARDataLable, CEditorUtil::ChangeMultibyteTextToTCHAR(DataLable.c_str()));
			lstrcpy(FulllErrorMessage, TCHARDataLable);

			if (OriginAnimStartFrame > StFrameInt)
			{
				TCHAR ErrorMessage[MAX_PATH] = TEXT(": Start Frame �� ������ ����ϴ�. Origin Start Frame : ");
				TCHAR StartFrameMessage[MAX_PATH];
				lstrcpy(StartFrameMessage, CEditorUtil::ChangeMultibyteTextToTCHAR(std::to_string(OriginAnimStartFrame).c_str()));

				lstrcat(FulllErrorMessage, ErrorMessage);
				lstrcat(FulllErrorMessage, StartFrameMessage);
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);

				return;
			}
			if (OriginAnimEndFrame < EdFrameInt)
			{
				TCHAR ErrorMessage[MAX_PATH] = TEXT(": End Frame �� ������ ����ϴ�. Origin End Frame : ");
				TCHAR EndFrameMessage[MAX_PATH];
				lstrcpy(EndFrameMessage, CEditorUtil::ChangeMultibyteTextToTCHAR(std::to_string(OriginAnimEndFrame).c_str()));

				lstrcat(FulllErrorMessage, ErrorMessage);
				lstrcat(FulllErrorMessage, EndFrameMessage);
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);
				return;
			}
		}
	}

		
	// >> ���� Load �� sqc �� ��ġ�� Folder ��ο� ������ sqc ���ϵ�� �����ؼ� ������ش�. 
	CMesh* CurrentUsedMesh = CResourceManager::GetInst()->FindMesh(m_3DTestObjectMeshName);

	// Extension ���� �̸� ex) CombinedPot.msh
	const std::string& UsedMeshFileName = CEditorUtil::FilterFileName(CurrentUsedMesh->GetFullPath());

	// Ȯ���� ���� ���� �̸�
	std::string UsedMeshFileNameOnly;
	CEngineUtil::GetFileNameOnly(UsedMeshFileName, UsedMeshFileNameOnly);

	// Full ��� ������ ����ִ�.
	auto MeshFileFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedMeshFileName);
	
	// Folder ���
	std::string CommonFolderPath;
	CEngineUtil::GetPathInfoBeforeFileName(MeshFileFoundResult.value(), CommonFolderPath);

	// Animation Sequence ����
	CAnimationSequence* ExistingSequeunce = m_Animation->GetCurrentAnimation()->GetAnimationSequence();

	// >> �������� Sqc ���� �����.
	{
		auto iter = ExcelTableData.begin();
		auto iterEnd = ExcelTableData.end();

		int FirstData = true;

		for (; iter != iterEnd; ++iter)
		{
			const std::string& DataLable = iter->first;
			std::string NewlySavedSqcFullPath;

			NewlySavedSqcFullPath.reserve(CommonFolderPath.length() * 2);

			NewlySavedSqcFullPath = CommonFolderPath;
			NewlySavedSqcFullPath.append(UsedMeshFileNameOnly);
			NewlySavedSqcFullPath.append("_" + DataLable);
			NewlySavedSqcFullPath.append(".sqc");

			// 1��°�� startFrame
			// 2��°�� endFrame
			const std::string& StFrame = (*(iter->second))[0];
			int StFrameInt;
			auto StFrameIntInfo = std::from_chars(StFrame.c_str(), StFrame.c_str() + sizeof(StFrame.c_str()) - 1, StFrameInt);

			const std::string& EdFrame = (*(iter->second))[1];
			int EdFrameInt;
			auto EdFrameIntInfo = std::from_chars(EdFrame.c_str(), EdFrame.c_str() + sizeof(EdFrame.c_str()) - 1, EdFrameInt);


			TCHAR TCHARSavedSqcFullPath[MAX_PATH];

			lstrcpy(TCHARSavedSqcFullPath, CEditorUtil::ChangeMultibyteTextToTCHAR(NewlySavedSqcFullPath.c_str()));

			// �ϳ��� Load ���н� X 
			// ó�� �Ѱ��� msh, fbm, bne ���� ���纻�� ������� ���̴�.
			if (!SaveEditedSqcFile(TCHARSavedSqcFullPath, ExistingSequeunce, StFrameInt, EdFrameInt, FirstData))
			{
				MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT(".sqc ���� ���� ����"), NULL, MB_OK);
				return;
			}

			FirstData = false;
		}
	}

	// >> ���� ���� ����� sqc ���� �ϳ��� anim ������ ������. (Save ����)
	// ���Ӱ� ������ sqc + ���� sqc ���� ��� UsedMeshFileNameOnly ��� �̸��� �����Ѵ�.
	std::string FolderPathWithOutSlash = std::string(CommonFolderPath.begin(), CommonFolderPath.end() - 1);
	GatherFullPathInfoOfSqcFilesInSelectedDir(FolderPathWithOutSlash, UsedMeshFileNameOnly);

	// >> ���� sqc �� .anim ���� ���� .sqc ��Ͽ��� �������ش�.
	//  m_vecAnimationSeqFilesFullPath;
	int TargetSqcFullPathSize = (int)m_vecAnimationSeqFilesFullPath.size();

	std::string ExisintSeqFileName;
	ExisintSeqFileName = UsedMeshFileNameOnly;
	if (ExisintSeqFileName.find(".sqc") == std::string::npos)
		ExisintSeqFileName.append(".sqc");

	for (int i = 0; i < TargetSqcFullPathSize; ++i)
	{
		// file �̸����� �̾Ƴ���. (Ȯ���� ����)
		const std::string& CurSqcFileName = CEditorUtil::FilterFileName(m_vecAnimationSeqFilesFullPath[i]);

		// ���� sqc ���� �̸��� ������ Ȯ���Ѵ�. -> ������ �����ϰ� return
		if (CurSqcFileName == ExisintSeqFileName)
		{
			m_vecAnimationSeqFilesFullPath.erase(m_vecAnimationSeqFilesFullPath.begin() + i);
			break;
		}
	}

	// >> �ϳ��� anim ������ ���� �����Ѵ�.
	// �α�â Ŭ����
	m_AnimInstanceConvertLog->ClearWidget();

	CIMGUIText* StartText = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");
	StartText->SetText("Convert Start...");


	// ���� ��� �༮��� Mesh Load �ϰ� 
	size_t Size = m_vecAnimationSeqFilesFullPath.size();

	CIMGUIText* Text = nullptr;

	bool DummyAnimationMadeSuccess = true;

	for (size_t i = 0; i < Size; ++i)
	{
		// Grunt_Idle �� ����, '_' �ڿ� Excel �� �ش��ϴ� Lable �� ���� ���̴�.
		std::string AddedKeyName;

		AddSequenceToDummyAnimationInstance(m_vecAnimationSeqFilesFullPath[i].c_str(), AddedKeyName);

		// �ش� Key �̸���, Excel �� ����� Layer �̸����� �ٲ��ش�.
		// �̸� ����, Excel �� ����� Layer �߿���, �ڽ��� ���� �����ϰ� �ִ� Layer ������ �������� ���̴�.
		auto iterSqc = ExcelTableData.begin();
		auto iterSqcEnd = ExcelTableData.end();

		int AddedKeyNameLength = (int)AddedKeyName.length();

		// AddedKeyName �߿��� '_' ���� ���ڸ� �����´�. 
		// ��, �� �ڿ������� �˻��Ѵ�. (����, str.find �� �ƴ϶�, rfind �� ����� ���̴�.)
		std::string_view AddedKeyNameAfterLowDash = AddedKeyName;
		AddedKeyNameAfterLowDash = AddedKeyNameAfterLowDash.substr(AddedKeyNameAfterLowDash.rfind('_') + 1, AddedKeyNameAfterLowDash.length());

		std::string NewLableKeyName;

		bool LabelFound = false;

		for (; iterSqc != iterSqcEnd; ++iterSqc)
		{
			// ex) Idle
			const std::string& DataLable = iterSqc->first;

			if (AddedKeyNameAfterLowDash.data() == DataLable)
			{
				LabelFound = true;
				NewLableKeyName = DataLable;
				break;
			}
		}

		if (!LabelFound)
		{
			m_AnimInstanceConvertLog->ClearWidget();
			DummyAnimationMadeSuccess = false;
			MessageBox(nullptr, TEXT("Label �̸��� '_' �� ���Ե��� �ʰ� �ϼ���. Ȥ��, ������ ���� sqc ���� �� �����ְ� �ٽ� �õ��ϼ���"), TEXT("����"), MB_OK);
			break;
		}

		// Dummy Animation �� ���� ã�ƾ� �Ѵ�.
		// Dummy Animation �󿡼� KeyName �� �����Ѵ�.
		if (!m_DummyAnimation->EditCurrentSequenceKeyName(NewLableKeyName.c_str(), AddedKeyName))
		{
			DummyAnimationMadeSuccess = false;
			assert(false);
			return;
		}
		// �Ӹ� �ƴ϶�, AnimationSequenceData �� m_Name �� �����ؾ� �Ѵ�.
// �׸��� ���� �����ϴ� AnimationSequenceData �� m_DummyAnimation �� ����Ǿ� �ִ� �༮�̾�� �Ѵ�.
		CAnimationSequenceData* AnimSeqData = m_DummyAnimation->FindAnimation(NewLableKeyName);
		AnimSeqData->SetName(NewLableKeyName);

		AnimSeqData->SetOriginalFramePlayTime();
		// File �̸� Log ��Ͽ� �߰�
		Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");

		Text->SetText(CEditorUtil::FilterFileName(m_vecAnimationSeqFilesFullPath[i]).c_str());

	}

	if (DummyAnimationMadeSuccess == false)
	{
		m_AnimInstanceConvertLog->ClearWidget();
		SAFE_DELETE(m_DummyAnimation);
		MessageBox(nullptr, TEXT("Instance Create ����"), TEXT("����"), MB_OK);
		return;
	}

	m_AnimInstanceProgressBar->SetPercent(100.f);

	Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("OK");
	Text->SetText("Complete!");

	// Dummy Animation Intance �� �����Ѵ�.

	const PathInfo* Path = CPathManager::GetInst()->FindPath(ANIMATION_PATH);

	std::string SavedFileNameMultibyte;
	SavedFileNameMultibyte.reserve(strlen(Path->PathMultibyte) * 2);

	// Ȯ���� ����
	std::string SaveAnimFileName;
	SaveAnimFileName.reserve(strlen(m_ExcelSavedAnimFileName->GetTextUTF8()) * 2);

	if (Path)
		SavedFileNameMultibyte = Path->PathMultibyte;

	SaveAnimFileName = m_ExcelSavedAnimFileName->GetTextUTF8();
	
	// .anim Ȯ���� �ٿ��ֱ�
	if (SaveAnimFileName.find(".anim") == std::string::npos)
		SaveAnimFileName.append(".anim");

	SavedFileNameMultibyte.append(SaveAnimFileName);

	m_DummyAnimation->SetSavedFileName(SaveAnimFileName.c_str());
	m_DummyAnimation->SaveAnimationFullPath(SavedFileNameMultibyte.c_str());

	MessageBox(nullptr, TEXT("Instance Create �Ϸ�"), TEXT("�Ϸ�"), MB_OK);

	SAFE_DELETE(m_DummyAnimation);
}

void CAnimationEditor::OnSaveAnimationInstance()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	TCHAR FiileFullPath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Animation File\0*.anim");
	OpenFile.lpstrFile = FiileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(ANIMATION_PATH)->Path;

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
		if (strcmp(FileExt, ".ANIM") != 0)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .anim"), NULL, MB_OK);
			return ;
		}

		m_Animation->SetSavedFileName(FileName);

		m_Animation->SaveAnimationFullPath(FileFullPathMultibyte);
	}
}

void CAnimationEditor::OnLoadAnimationInstance()
{
	TCHAR FilePath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Animation File\0*.anim");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(ANIMATION_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char	Ext[_MAX_EXT] = {};

		char FilePathMultibyte[MAX_PATH] = {};
		char FileName[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		_splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, FileName, MAX_PATH, Ext, _MAX_EXT);

		_strupr_s(Ext);

		// Ȯ���� .anim �� �ƴ϶�� return;
		if (strcmp(Ext, ".ANIM") != 0)
			return;

		if (!m_Animation)
		{
			// ex) singed_spell2.sqc �� �����ߴٸ�
			// ���� ���� ��� ������ singed_spell2.msh / singed_spell2.bne �� Load �Ͽ� �����Ѵ�.
			// singed_spell2.msh -> singed_spell2_mesh ��� �̸�����
			// singed_spell2.bne -> singed_spell2_skeleton �̶�� �̸�����
			OnCreateSample3DObject();
		}
		else
		{
			// ���� Animation List�� �������� , ��, ���� Animation�� Added �Ǿ��� ��� Sequence ������ �����ش�
			OnDeleteExisting3DObject();

			OnCreateSample3DObject();
		}

		// TODO : Animation Instance �� �׳� ������ ������ �ȵǳ� ?
		m_Animation->LoadAnimationFullPath(FilePathMultibyte);

		// CurrentAnimation �� ���ٸ�, 
		if (!m_Animation->GetCurrentAnimation())
		{
			// .anim File �� ���������� Load ���� �ʾҴٴ� �ǹ��̴�.
			// - �׷��� ������ �����ص� 3DObject �� ������ Animation Mesh Component �� �׷����� �ʰ� �ؾ� �Ѵ�.
			OnDeleteExisting3DObject();
			return;
		}

		std::pair<bool, std::string> LoadResult = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(m_Animation);

		if (LoadResult.first == false)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Mesh, Texture, Skeleton Load Failure"), NULL, MB_OK);

			// ���� Load �� �����ߴٸ�, 
			OnDeleteExisting3DObject();

			return;
		}

		// �ش� �̸��� 3d Test Object ���� �����ͼ� ����� ���̴�.
		m_3DTestObjectMeshName = LoadResult.second;

		// Set Material, Mesh Info
		SetMeshMaterialReadyForAnimation();

		// ComboBox �� ��� Seq ������ �߰����ش�
		m_CurAnimComboBox->Clear();

		int Size = m_Animation->GetAnimationCount();
		std::vector<std::string> vecSeqNames;
		vecSeqNames.reserve(Size);

		m_Animation->GatherSequenceNames(vecSeqNames);

		for (int i = 0; i < Size; i++)
		{
			m_CurAnimComboBox->AddItem(vecSeqNames[i]);
		}

		// Current Animation ������ �����Ѵ�.
		int CurAnimIdx = m_Animation->GetCurrentAnimationOrder();

		if (CurAnimIdx == -1)
		{
			assert(false);
			return;
		}

		m_CurAnimComboBox->SetSelectIndex(CurAnimIdx);

		// ���� Select �� Animation �� �ּ�, �ִ� Frame ���� Frame Slider ���� ����
		m_FrameSlider->SetMin(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetStartFrame());
		m_FrameSlider->SetMax(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetEndFrame());

		// Table ���� ����
		OnRefreshAnimationClipTable(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

		// ���� Scene�� ��� Sequence ������ �߰��Ѵ�.
		// m_Animation->AddAnimationSequenceToSceneResource();
		OnRefreshFrameSliderInfo(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

		// ���� Scene�� ������ m_Scene���� �������ش�
		m_Animation->SetScene(CSceneManager::GetInst()->GetScene());

		// Refresh CheckBox Info
		OnRefreshCheckBoxInfo();

		// Animation Play Scale, Time
		OnRefreshScaleAndTimeInputInfo();

		// Animation�� �����Ѵ�..
		m_Animation->Play();

		// Load �� Animation File �� FileName �� �����Ѵ�.
		// .anim Ȯ���ڰ� ���� ���ٸ�, .anim Ȯ���ڸ� �ٿ��� Text �� Set
		std::string LoadedName = m_Animation->GetSavedFileName();
		
		if (LoadedName == "") // Loaded FileName �� ���ٸ� �ȵȴ�.
			assert(false);

		if (LoadedName.find(".anim") == std::string::npos)
			LoadedName.append(".anim");

		m_LoadedAnimFileName->SetText(LoadedName.c_str());
	}
}

void CAnimationEditor::OnAnimationFrameInputCallback()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// ������ �Ѿ�� �������ش�.
	int InputFrame = m_FrameInput->GetValueInt();
	int StFrame = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetStartFrame();
	int EdFrame = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetEndFrame();

	if (InputFrame < StFrame)
		InputFrame = EdFrame;
	else if (InputFrame > EdFrame)
		InputFrame = StFrame;

	m_FrameInput->SetInt(InputFrame);

	// Input �� Frame ���� Animation Frame ������ �������ش�.
	// m_Animation->SetCurrentAnimationFrameIdx(InputFrame);
	m_Animation->SetEditorStopTargetFrame(InputFrame);

}

void CAnimationEditor::OnAnimationSliderIntCallback(int CurrentFrame)
{
	m_FrameSlider->SetValue(CurrentFrame);
}

void CAnimationEditor::OnApplyAnimationSlider(CAnimationSequence* Sequence)
{
	if (!Sequence)
		return;

	m_FrameSlider->SetMin(Sequence->GetStartFrame());
	m_FrameSlider->SetMax(Sequence->GetEndFrame());
}

bool CAnimationEditor::LoadElementsForSqcLoading(const char* SqcFileName)
{
	// ���� Mesh Load �������� �ʿ��� Texture�� ���ٸ� 
	// ex) FBX Convert ����, singed_spell2.sqc �� �ִٸ�, ���� ��γ��� singed_spell2.fbm �̶�� ���丮�� �����ؾ� �Ѵ�.
	// ���� �ش� Folder �� �������� �ʴ´ٸ�, Mesh�� Load �ϴ��� Texture �� ���ٰ� �� ���̴�
	char TextFolderExt[10] = ".fbm";
	char TextFolderFullPath[MAX_PATH] = {};

	// RESOURCE_MESH_PATH �� �ִ� �����κ��� �ҷ��� ���̴�.
	char MeshFileFullPath[MAX_PATH] = {};
	
	const PathInfo* Path = CPathManager::GetInst()->FindPath(MESH_PATH);
	
	if (Path)
		strcpy_s(MeshFileFullPath, Path->PathMultibyte);

	strcpy_s(TextFolderFullPath, MeshFileFullPath);
	strcat_s(TextFolderFullPath, SqcFileName);
	strcat_s(TextFolderFullPath, TextFolderExt);

	std::filesystem::path MeshTextureFolderPath(TextFolderFullPath);

	if (!std::filesystem::exists(MeshTextureFolderPath))
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT(".fbm Folder Does Not Exist"), NULL, MB_OK);
		return false;
	}

	// ex) singed_spell2.sqc �� �����ߴٸ�
	// ���� ���� ��� ������ singed_spell2.msh / singed_spell2.bne �� Load �Ͽ� �����Ѵ�.
	// singed_spell2.msh -> singed_spell2_mesh ��� �̸�����
	// singed_spell2.bne -> singed_spell2_skeleton �̶�� �̸�����

	// Load Mesh
	std::string LoadedMeshName = SqcFileName;
	// LoadedMeshName.append("_mesh");

	// �ش� �̸��� 3d Test Object ���� �����ͼ� ����� ���̴�.
	m_3DTestObjectMeshName = LoadedMeshName;

	char MeshExt[10] = ".msh";

	char MshFileName[MAX_PATH] = {};
	TCHAR MshTCHARFileName[MAX_PATH] = {};

	strcpy_s(MshFileName, SqcFileName);
	strcat_s(MshFileName, MeshExt);

	int ConvertLength = MultiByteToWideChar(CP_ACP, 0, MshFileName, -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, MshFileName, -1, MshTCHARFileName, ConvertLength);

	if (!CSceneManager::GetInst()->GetScene()->GetResource()->LoadMesh(Mesh_Type::Animation, LoadedMeshName,
		MshTCHARFileName, MESH_PATH))
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT(".msh Load Failure"), NULL, MB_OK);
		return false;
	}

	// Bne (Skeleton) Load
	char BneExt[10] = ".bne";

	std::string LoadedBneName = SqcFileName;
	LoadedBneName.append("_skeleton");

	char BneFileName[MAX_PATH] = {};
	TCHAR BneTCHARFileName[MAX_PATH] = {};

	strcpy_s(BneFileName, SqcFileName);
	strcat_s(BneFileName, BneExt);

	ConvertLength = MultiByteToWideChar(CP_ACP, 0, BneFileName, -1, 0, 0);
	MultiByteToWideChar(CP_ACP, 0, BneFileName, -1, BneTCHARFileName, ConvertLength);

	if (!CSceneManager::GetInst()->GetScene()->GetResource()->LoadSkeleton(LoadedBneName, BneTCHARFileName, MESH_PATH))
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT(".bne Load Failure"), NULL, MB_OK);
		return false;
	}

	// Mesh �� �ش� Skeleton ����
	CSceneManager::GetInst()->GetScene()->GetResource()->SetMeshSkeleton(LoadedMeshName, LoadedBneName); //


	return true;
}

void CAnimationEditor::SetMeshMaterialReadyForAnimation()
{
	m_3DTestObject->SetMeshAndMaterialInfo();

	m_Animation->Start();

	// 3DTest Object�� Animation ������ �����´�.
	CAnimationMeshComponent* AnimComponent = dynamic_cast<CAnimationMeshComponent*>(m_3DTestObject->GetRootComponent());

	if (AnimComponent == nullptr)
		assert(false);

	// Animation Display Window ���� Scale �� �������ش�.
	CEditorManager::GetInst()->GetAnimationDisplayWindow()->SetAnimMeshScaleToIMGUI(AnimComponent->GetRelativeScale());
}

void CAnimationEditor::OnSetOriginalAnimPlayTime()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	m_Animation->GetCurrentAnimation()->SetOriginalFramePlayTime();

	// PlayTime Input Update
	// Anim Info Table Update
	OnRefreshAnimationClipTable(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

	OnRefreshScaleAndTimeInputInfo();
}

void CAnimationEditor::OnEditAnimPlayTime()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// if (m_PlayTimeInput->Empty())
	//	return;

	float SetPlayTime = m_PlayTimeInput->GetValueFloat();

	m_Animation->GetCurrentAnimation()->GetAnimationSequence()->SetPlayTime(SetPlayTime);
	m_Animation->GetCurrentAnimation()->SetPlayTime(SetPlayTime);

	OnRefreshAnimationClipTable(m_Animation->GetCurrentAnimation()->GetAnimationSequence());
}

void CAnimationEditor::OnEditAnimPlayScale()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// if (m_PlayScaleInput->Empty())
	//	return;

	m_Animation->GetCurrentAnimation()->GetAnimationSequence()->SetPlayScale(m_PlayScaleInput->GetValueFloat());
	m_Animation->GetCurrentAnimation()->SetPlayScale(m_PlayScaleInput->GetValueFloat());

	OnRefreshAnimationClipTable(m_Animation->GetCurrentAnimation()->GetAnimationSequence());
}

void CAnimationEditor::OnEditAnimSequenceKey()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// New Key ���� �ߺ��� Key ���� Animation �� �ִ����� �˻��Ѵ�.
	if (m_Animation->FindAnimation(m_EditAnimSeqDataKeyName->GetTextUTF8()))
	{
		MessageBox(nullptr, TEXT("Sequence Exist"), TEXT("Same Named Seq File Already Exist"), MB_OK);
		return;
	}

	// New Key ��
	// EditCurrentSequenceKeyName
	if (!m_Animation->EditCurrentSequenceKeyName(m_EditAnimSeqDataKeyName->GetTextUTF8(), m_CurAnimComboBox->GetSelectItem()))
	{
		assert(false);
		return;
	}

	// �ٲ��� �̸��� Animation ��  Current Animation ���� �����Ѵ�.
	m_Animation->SetCurrentAnimation(m_EditAnimSeqDataKeyName->GetTextUTF8());

	// Combo Box ���� Refresh
	OnRefreshAnimationComboBox();

	// ���Ӱ� �ٲ� SequenceKey Name �� �̸�����, ComboBox �����صα�
	int FoundIndex = m_CurAnimComboBox->FindTargetTextIndex(m_EditAnimSeqDataKeyName->GetTextUTF8());
	
	if (FoundIndex != -1)
	{
		m_CurAnimComboBox->SetSelectIndex(FoundIndex);
	}
}

void CAnimationEditor::OnDeleteAnimationSequenceData()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// ���� Current Animation �� �ش��ϴ� Sequence �� ���� Scene ���� �����ش�.
	CAnimationSequence* DeleteSeq = CResourceManager::GetInst()->FindAnimationSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetName());
	
	CSceneManager::GetInst()->GetScene()->GetResource()->DeleteSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

	m_Animation->DeleteCurrentAnimation();

	// ResourceManager �� �����ִ� Sqc �� ref Cnt �� 1 �̶��, ���̻� �ش� sqc �� ����ϴ� �ٸ� Obj �� ���ٴ� ��
	// ���� �����ش�.
	CResourceManager::GetInst()->ReleaseAnimationSequence3D(DeleteSeq);


	// Combo Box ���� Refresh
	// ������� ���, ComboBox ���� ���� ""�� �����ϴ� ��ɱ��� 
	OnRefreshAnimationComboBox();

	if (!m_Animation->GetCurrentAnimation())
	{
		// �� �����ٴ� �޼���
		MessageBox(nullptr, TEXT("��� Sqc ������ Delete �Ǿ����ϴ�."), TEXT("Anim Seq Delete."), MB_OK);

		// Animation Info Table �����ϰ�
		m_AnimInfoTable->ClearContents();

		// 3D Object ? Animation Mesh Component �� �����ش�. 
		OnDeleteExisting3DObject();

		// Render Target �� ����ش�.
		CRenderManager::GetInst()->GetAnimationRenderTarget()->ClearTarget();

		// Ȥ�ó� Load �� .anim ���� ������ �ִٸ� �����ش�
		m_LoadedAnimFileName->ClearText();

		return;
	}

	OnRefreshAnimationClipTable(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

	OnRefreshFrameSliderInfo(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

	OnRefreshScaleAndTimeInputInfo();

	OnRefreshCheckBoxInfo();
}

void CAnimationEditor::OnDeleteAnimation3DObject()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// ��� Sequence ������ �����ش�.
	while (m_Animation->GetCurrentAnimation())
	{
		// ���� Current Animation �� �ش��ϴ� Sequence �� ���� Scene ���� �����ش�.
		CAnimationSequence* DeleteSeq = CResourceManager::GetInst()->FindAnimationSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetName());

		CSceneManager::GetInst()->GetScene()->GetResource()->DeleteSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

		m_Animation->DeleteCurrentAnimation();

		// ResourceManager �� �����ִ� Sqc �� ref Cnt �� 1 �̶��, ���̻� �ش� sqc �� ����ϴ� �ٸ� Obj �� ���ٴ� ��
		// ���� �����ش�.
		CResourceManager::GetInst()->ReleaseAnimationSequence3D(DeleteSeq);
	}

	// Combo Box ���� Refresh
	// ������� ���, ComboBox ���� ���� ""�� �����ϴ� ��ɱ��� 
	OnRefreshAnimationComboBox();

	// �Ʒ� �Լ��� ȣ��Ǳ� ���ؼ��� m_Animation �� ���̻� CurrentAnimation�� ������ �־�� �ȵȴ�.
	// �� �����ٴ� �޼���
	MessageBox(nullptr, TEXT("��� Sqc ������ Delete �Ǿ����ϴ�."), TEXT("Anim Seq Delete."), MB_OK);

	// Animation Info Table �����ϰ�
	m_AnimInfoTable->ClearContents();

	// 3D Object ? Animation Mesh Component �� �����ش�. 
	OnDeleteExisting3DObject();

	// Render Target �� ����ش�.
	CRenderManager::GetInst()->GetAnimationRenderTarget()->ClearTarget();

	// Ȥ�ó� Load �� .anim ���� ������ �ִٸ� �����ش�
	m_LoadedAnimFileName->ClearText();

	return;
}

void CAnimationEditor::OnSetPlayEngineDeltaTime(const char* Lable, bool Enable)
{
	// bool IsEnginePlay = CEngine::GetInst()->IsPlay();

	// if (IsEnginePlay)
	if (!Enable)
		CEngine::GetInst()->Pause();
	else
		CEngine::GetInst()->Play();
}

void CAnimationEditor::OnAddAnimationSequence()
{
	// Add �� Animation �� �̸��� �Է��ؾ� �Ѵ�
	if (m_NewAnimSeqName->Empty() || m_NewAnimSeqDataKeyName->Empty())
		return;

	// ���� Seq Key �� Key�� ����ϰ� �ִ� SceneResource ���� Sequence �� �����Ѵ�.
	// �׷��߸�, ���� Add�� Key��, �ҷ������� �ϴ� .sqc ������ ����� �ҷ��´�.
	// �׷��� �ʴٸ�, �̹� SceneResource �� �����ϴ� .sqc �� �ҷ����� �� ���̴�.
	if (CSceneManager::GetInst()->GetScene()->GetResource()->FindAnimationSequence(m_NewAnimSeqName->GetTextUTF8()))
	{
		MessageBox(nullptr, TEXT("�ٸ� Sqc Key Ȱ���ϼ���"), TEXT("�ٸ� Sqc Key Ȱ���ϼ���"), MB_OK);
		return;
	}

	TCHAR LoadFilePath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = LoadFilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("�������\0*.*\0*.Scene File\0*.scn");
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(MESH_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char	Ext[_MAX_EXT] = {};
		char FilePathMultibyte[MAX_PATH] = {};
		char SqcFileName[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, LoadFilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, LoadFilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		_splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, SqcFileName, MAX_PATH, Ext, _MAX_EXT);
		
		_strupr_s(Ext);
		
		// Ȯ���� .sqc �� �ƴ϶�� return;
		if (strcmp(Ext, ".SQC") != 0)
			return;
		
		if (!m_Animation)
		{
			// f (!LoadElementsForSqcLoading(SqcFileName))
			// 	return;
		
			std::pair<bool, std::string> LoadResult = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(SqcFileName);
		
			if (!LoadResult.first)
			{
				return;
			}
		
			m_3DTestObjectMeshName = LoadResult.second;
		
			// Create Object
			OnCreateSample3DObject();
		
			// Set Material, Mesh Info
			SetMeshMaterialReadyForAnimation();
		}
		
		// �̸� �ߺ� X --> Key �̸� �ߺ��Ǵ� Sequence �� �߰� X 
		// ex) AnimationInstance --> ("ZedIdle", "Idle"); --> "ZedIdle" �̶�� Key �� ���� �� �ٸ� Sqc ������ �ε��ϸ� �ȵȴ�.
		if (m_Animation->FindAnimation(m_NewAnimSeqDataKeyName->GetTextUTF8()))
			return;
		
		// Animation Seq Load
		std::string FSequenceName;
		
		// Load �� ���� ������ �̸��� �����ͼ� �����Ѵ�.
		bool ResultLoadSeq = CSceneManager::GetInst()->GetScene()->GetResource()->LoadAnimationSequenceFullPathMultibyte(true,
			m_NewAnimSeqName->GetTextUTF8(), FilePathMultibyte);
		
		if (!ResultLoadSeq)
			return;
		
		CAnimationSequence* LoadedSequence = CSceneManager::GetInst()->GetScene()->GetResource()->FindAnimationSequence(m_NewAnimSeqName->GetTextUTF8());
		
		LoadedSequence->SetAnimationFullPathMultibyte(FilePathMultibyte);
		LoadedSequence->SetAnimationFileNameMultibyte(SqcFileName);
		
		// ���� 
		m_Animation->AddAnimation(m_NewAnimSeqName->GetTextUTF8(), m_NewAnimSeqDataKeyName->GetTextUTF8());
		
		// Combo Box ���� ����
		OnRefreshAnimationClipTable(LoadedSequence);

		OnRefreshFrameSliderInfo(LoadedSequence);

		OnRefreshAnimationComboBox();
		
		OnRefreshScaleAndTimeInputInfo();

		OnRefreshCheckBoxInfo();
		
		// Frame Slider �� �ִ� �ּ� �� �����ϱ�
		m_FrameSlider->SetMin(LoadedSequence->GetStartFrame());
		m_FrameSlider->SetMax(LoadedSequence->GetEndFrame());
	}
}

void CAnimationEditor::OnClickAnimationSequence(int Index, const char* Name)
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	m_CurAnimComboBox->SetSelectIndex(Index);

	const std::string& SelectSequenceName = m_CurAnimComboBox->GetSelectItem();

	CAnimationSequenceData* SequenceData = m_Animation->FindAnimation(SelectSequenceName);

	if (!SequenceData)
	{
		return;
	}

	// Ŭ���� Animation ���� Current Animation ����
	m_Animation->SetCurrentAnimation(Name);

	// Table ���� ����
	OnRefreshAnimationClipTable(SequenceData->GetAnimationSequence());

	OnRefreshFrameSliderInfo(SequenceData->GetAnimationSequence());

	OnRefreshAnimationComboBox();

	OnRefreshScaleAndTimeInputInfo();

	OnRefreshCheckBoxInfo();
}

void CAnimationEditor::OnSetAnimationComboBoxCallback(const std::string& AnimSequenceName)
{
	if (!m_Animation)
		return;
	m_Animation->SetCurrentAnimation(AnimSequenceName);
}

void CAnimationEditor::OnRefreshAnimationClipTable(CAnimationSequence* Sequence)
{
	if (!Sequence)
	{
		return;
	}
	m_AnimInfoTable->ClearContents();

	m_AnimInfoTable->AddData(AnimationClipInfoKeys::FrameRange, Sequence->GetStartFrame());
	m_AnimInfoTable->AddData(AnimationClipInfoKeys::FrameRange, Sequence->GetEndFrame());
	m_AnimInfoTable->AddData(AnimationClipInfoKeys::FrameLength, Sequence->GetFrameLength());
	m_AnimInfoTable->AddData(AnimationClipInfoKeys::FrameMode, Sequence->GetFrameMode());
	m_AnimInfoTable->AddData(AnimationClipInfoKeys::PlayTime, Sequence->GetPlayTime());
	m_AnimInfoTable->AddData(AnimationClipInfoKeys::FrameTime, Sequence->GetFrameTime());
	m_AnimInfoTable->AddData(AnimationClipInfoKeys::PlayScale, Sequence->GetPlayScale());
}

void CAnimationEditor::OnRefreshFrameSliderInfo(CAnimationSequence* Sequence)
{
	if (!Sequence)
		return;

	m_FrameSlider->SetMin(Sequence->GetStartFrame());
	m_FrameSlider->SetMax(Sequence->GetEndFrame());
}

void CAnimationEditor::OnRefreshAnimationComboBox()
{
	if (!m_Animation)
		return;

	// �� ó�� Sequence�� �߰��ϴ� ��Ȳ�̶��
	int SeletedIdx = m_CurAnimComboBox->GetSelectIndex();

	m_CurAnimComboBox->Clear();

	std::vector<std::string> vecCurKeyNames;

	m_Animation->GatherSequenceNames(vecCurKeyNames);

	for (const auto& name : vecCurKeyNames)
		m_CurAnimComboBox->AddItem(name);

	if (m_CurAnimComboBox->Empty())
		return;

	if (SeletedIdx < m_CurAnimComboBox->GetItemCount())
	{
		// ó�� Animation �� �߰��ϴ� ��Ȳ�̶��
		if (SeletedIdx == -1)
			m_CurAnimComboBox->SetSelectIndex(0);
		else
			m_CurAnimComboBox->SetSelectIndex(SeletedIdx);
	}
	else
		m_CurAnimComboBox->SetSelectIndex(m_CurAnimComboBox->GetItemCount() - 1);


}

void CAnimationEditor::OnRefreshScaleAndTimeInputInfo()
{
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	float PlayScale = m_Animation->GetCurrentAnimation()->GetAnimationPlayScale();
	m_PlayScaleInput->SetFloat(PlayScale);

	float PlayTime = m_Animation->GetCurrentAnimation()->GetAnimationPlayTime();
	m_PlayTimeInput->SetFloat(PlayTime);
}

void CAnimationEditor::OnRefreshCheckBoxInfo()
{
	if (!m_3DTestObject)
		return;

	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// m_DeltaTimeCheckBtn->SetCheck(0, CEngine::GetInst()->IsPlay());
	m_AnimationCheckBtn->SetCheck(0, m_Animation->IsPlay());
	m_RotationCheckBtn->SetCheck(0,m_3DTestObject->IsCameraRot());
	m_ZoomEnableBtn->SetCheck(0, m_3DTestObject->IsCameraRot());
	m_LoopEnableBtn->SetCheck(0, m_Animation->GetCurrentAnimation()->IsLoop());
}

void CAnimationEditor::OnEditStartEndFrame()
{
	// Animation ��ü ������ �־�� �ϰ�
	if (!m_3DTestObject)
		return;

	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// Frame ������ ����� �ȵȴ�. (Message Box)
	CAnimationSequence* ExistingSequence = m_Animation->GetCurrentAnimation()->GetAnimationSequence();

	if (m_StartFrameEditInput->GetValueInt() < 0 || m_EndFrameEditInput->GetValueInt() >= ExistingSequence->GetFrameLength())
	{
		MessageBox(nullptr, TEXT("Frame ������ ���."), TEXT("Frame ������ ���."), MB_OK);
		return;
	}

	TCHAR FiileFullPath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Sqc Files\0*.sqc");
	OpenFile.lpstrFile = FiileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(MESH_PATH)->Path;

	if (GetSaveFileName(&OpenFile) != 0)
	{
		SaveEditedSqcFile(FiileFullPath, ExistingSequence, m_StartFrameEditInput->GetValueInt(), m_EndFrameEditInput->GetValueInt());
	}
}

CAnimationSequence* CAnimationEditor::NewlyEditSqcFile(const TCHAR* FileSavedFullPath, CAnimationSequence* ExistingSequence, int StartFrame, int EndFrame)
{
	return nullptr;
}

bool CAnimationEditor::SaveEditedSqcFile(const TCHAR* FileSavedFullPath, CAnimationSequence* ExistingSequence, 
	int StartFrame, int EndFrame, bool MakeCopy)
{
	char FileFullPathMultibyte[MAX_PATH] = {};

	char NewlySavedSqcFileName[MAX_PATH] = {};
	char FileExt[_MAX_EXT] = {};

	int  ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileSavedFullPath, -1, nullptr, 0, nullptr, nullptr);

	WideCharToMultiByte(CP_ACP, 0, FileSavedFullPath, -1, FileFullPathMultibyte, ConvertLength, nullptr, nullptr);

	_splitpath_s(FileFullPathMultibyte, nullptr, 0, nullptr, 0, NewlySavedSqcFileName, MAX_PATH, FileExt, _MAX_EXT);

	_strupr_s(FileExt);

	// Ȯ���� .sqc �� �ƴ϶�� return;
	if (strcmp(FileExt, ".SQC") != 0)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .sqc"), NULL, MB_OK);
		return false;
	}

	bool SqcSaveResult = CResourceManager::GetInst()->EditSequenceClip(ExistingSequence, NewlySavedSqcFileName,
		StartFrame, EndFrame, FileFullPathMultibyte);

	bool TotalSaveResult = true;

	if (SqcSaveResult)
	{
		// bne, fbm, msh ������ ���������� ���� ���̶��, �׳� return true �ϰ� ��
		if (!MakeCopy)
			return true;

		// ���� Sqc �� ����ϴ� Mesh, Bne, Fbm ���� ������ ���� ������ ���� ��ο� �״�� �����Ͽ� �־��ְ�
		// �ش� ���ϵ��� �̸���, ���� ������ Sqc ���ϵ�� �����ϰ� �����Ѵ�.
		std::string SavedFolderPath;
		CEngineUtil::GetPathInfoBeforeFileName(FileFullPathMultibyte, SavedFolderPath);

		// (����)
		// m_3DTestObjectMeshName �� ���� ����ϴ� Mesh�� Name �� ���õǾ� �ִ�.
		// �ش� Mesh Name ���� Mesh �� ã�´�.
		// �� Mesh ���� ����� ����� FullPath ������ ����ִ�.
		// �ش� FullPath ������ �̿��ؼ�, Mesh�� ����ϴ� FileName �� �����ͼ�
		// FileName.msh, FileName.bne, FileName.fbm ������ ã�Ƴ���.
		// �ش� ���ϵ���, ���� ������ ������ �����ϰ�
		// ������ ���ϵ��� �̸��� �ٲ��ش�.
		CMesh* CurrentUsedMesh = CResourceManager::GetInst()->FindMesh(m_3DTestObjectMeshName);

		// Extension ���� �̸� ex) CombinedPot.msh
		const std::string& UsedMeshFileName = CEditorUtil::FilterFileName(CurrentUsedMesh->GetFullPath());
		auto MeshFileFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedMeshFileName);

		// Extension ����
		std::string FileNameOnly;
		CEngineUtil::GetFileNameOnly(UsedMeshFileName, FileNameOnly);

		// Unicode ����
		TCHAR TCHARCopyFileNameOnly[MAX_PATH];
		lstrcpy(TCHARCopyFileNameOnly, CEditorUtil::ChangeMultibyteTextToTCHAR(FileNameOnly.c_str()));

		// ������ .bne ����, .fbm ���� �̸��� �����Ѵ�.
		std::string UsedBoneFileName = FileNameOnly;
		UsedBoneFileName.append(".bne");

		std::string UsedTextureFolderName = FileNameOnly;
		UsedTextureFolderName.append(".fbm");

		// (Ȥ�ó� �ϴ� ����ó��)
		// �ش� .msh, .bne, .fbn �� ��γ��� �����ϴ��� Ȯ���ϱ� => ���߿� �ʿ��ϸ� �ϱ� 
		// MESH_PATH ��� �ȿ� �����ؾ� �Ѵ�.
		// ���� .bne Ȯ��
		auto BoneFileFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedBoneFileName);

		TCHAR BneFulllErrorMessage[MAX_PATH] = {};
		TCHAR BneErrorMessage[MAX_PATH] = TEXT(".bne Does Not Exist in Bin//Mesh");
		lstrcpy(BneFulllErrorMessage, TCHARCopyFileNameOnly);
		lstrcat(BneFulllErrorMessage, BneErrorMessage);

		// �ش� ��ο� Mesh ������ �������� �ʴ´ٸ� Return
		if (!BoneFileFoundResult.has_value())
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), BneFulllErrorMessage, NULL, MB_OK);
			TotalSaveResult = false;
			return false;
		}

		// �� ���� Texture Folder Ȯ��
		auto TextureFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedTextureFolderName);

		TCHAR FbmFulllErrorMessage[MAX_PATH] = {};
		TCHAR FbmErrorMessage[MAX_PATH] = TEXT(".fbm Does Not Exist in Bin//Mesh");
		lstrcpy(FbmFulllErrorMessage, TCHARCopyFileNameOnly);
		lstrcat(FbmFulllErrorMessage, FbmErrorMessage);

		// �ش� ��ο� Mesh ������ �������� �ʴ´ٸ� Return
		if (!TextureFoundResult.has_value())
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), FbmFulllErrorMessage, NULL, MB_OK);
			TotalSaveResult = false;
			return false; 
		}

		// ����
		std::string CopyMeshPath = SavedFolderPath;
		CopyMeshPath.append(UsedMeshFileName);

		std::string CopyBonePath = SavedFolderPath;
		CopyBonePath.append(UsedBoneFileName);

		std::string CopyTexturePath = SavedFolderPath;
		CopyTexturePath.append(UsedTextureFolderName);

		// ���� ����� ��� ����
		std::string FinalCopyMeshPath;
		std::string FinalCopyBonePath;
		std::string FinalCopyTexturePath;

		CEngineUtil::CopyFileToOtherDirectory(MeshFileFoundResult.value(), CopyMeshPath, FinalCopyMeshPath);
		CEngineUtil::CopyFileToOtherDirectory(BoneFileFoundResult.value(), CopyBonePath, FinalCopyBonePath);
		// �ȿ� �ִ� Texture ����鵵 Copy ��ų ���̴�.
		CEngineUtil::CopyFileToOtherDirectory(TextureFoundResult.value(), CopyTexturePath, FinalCopyTexturePath, true);

		// �̸� ����
		std::string NewMeshFileName = NewlySavedSqcFileName;
		NewMeshFileName.append(".msh");

		std::string NewBoneFileName = NewlySavedSqcFileName;
		NewBoneFileName.append(".bne");

		std::string NewTextureFolderName = NewlySavedSqcFileName;
		NewTextureFolderName.append(".fbm");

		CEditorUtil::ChangeFileOrDirectoryName(FinalCopyMeshPath, NewMeshFileName);
		CEditorUtil::ChangeFileOrDirectoryName(FinalCopyBonePath, NewBoneFileName);
		CEditorUtil::ChangeFileOrDirectoryName(FinalCopyTexturePath, NewTextureFolderName);

		if (TotalSaveResult == false)
		{
			MessageBox(nullptr, TEXT("Seq Edit & Save ���� !!!."), TEXT("Seq Edit & Save ����."), MB_OK);
			return false;
		}

		return true;
	}

	return false;
}



void CAnimationEditor::OnClickSetAnimSeqSrcDirButton()
{
	if (m_CommonAnimSeqName->Empty())
		return;

	TCHAR Buf[MAX_PATH] = {};

	BROWSEINFO Bi = {};

	Bi.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	Bi.lpszTitle = TEXT("�ϳ��� Instance �� ���� Seq ���ϵ��� �ִ� ���� ����");
	Bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;
	Bi.lpfn = BrowseCallbackProc;
	//	Bi.lParam = (LPARAM)InitialPath;

	LPITEMIDLIST ItemIDList = ::SHBrowseForFolder(&Bi);

	if (::SHGetPathFromIDList(ItemIDList, Buf))
	{
		char SelectedDirPath[MAX_PATH] = {};

		int length = WideCharToMultiByte(CP_ACP, 0, Buf, -1, nullptr, 0, nullptr, 0);
		// WideCharToMultiByte(CP_ACP, 0, Buf, -1, m_SelectedSeqSrcsDirPath, length, nullptr, 0);

		WideCharToMultiByte(CP_ACP, 0, Buf, -1, SelectedDirPath, length, nullptr, 0);
		
		// Common Name ������ ��������� �ȵȴ�
		if (m_CommonAnimSeqName->Empty())
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Common Sqc �̸��� �Է��ϼ���"), NULL, MB_OK);
			return;
		}

		GatherFullPathInfoOfSqcFilesInSelectedDir(SelectedDirPath, m_CommonAnimSeqName->GetTextUTF8());
	}
}

// m_ConvertAnimInstanceBtn �� Callback
void CAnimationEditor::OnConvertSequencesIntoAnimationInstance()
{
	//m_SavedAnimFileName : Instance �� ���� ����, .anim ���Ϸ� ������ ���� ���� �̸�
	if (m_AnimSeqcSrcFolderPath->Empty() || m_SavedAnimFileName->Empty())
		return;

	// ���� m_SavedAnimFileName ��, Bin/Animation �� �̹� �����ϴ��� Ȯ�� + �ߺ� ����
	// return ���� true ���, ���� X (�ߺ��� �̸��� .anim ������ �̹� �����Ѵٴ� �ǹ��̱� �����̴�)
	bool DuplicateResult = CheckSavedFileNameDuplicated();

	if (DuplicateResult)
	{
		MessageBox(nullptr, TEXT("�ش� Folder �� ���� �̸��� .anim File ����"), TEXT(".anim File Duplicate"), MB_OK);
		return;
	}

	// Animation Instance �� ���� .sqc ���� ��ϵ��� �ִ��� �˻��Ѵ�.
	if (m_vecAnimationSeqFilesFullPath.size() <= 0)
	{
		MessageBox(nullptr, TEXT("�ش� Folder �� �ش� Common �̸��� ������ .sqc ���� ���� X"), TEXT("Common Name Error"), MB_OK);
		return;
	}

	// �ٽ� �ѹ�, ���� ��Ƶ� m_vecAnimationSeqFilesFullPath ����, Common Name �� ���ԵǾ� �ִ��� �ѹ� �� �˻��Ѵ�.
	// ù��° File �θ� �˻��� ���̴�.
	bool CheckCommonNameResult = CheckGatheredSeqFilesIncludeCommonName();

	if (!CheckCommonNameResult)
	{
		MessageBox(nullptr, TEXT("Folder ��ο�, Common �̸��� Ȯ���ϼ���. (�߰��� Common �̸��� �����ߴ��� Ȯ��)"), TEXT("Common Name Error"), MB_OK);
		return;
	}

	// �α�â Ŭ����
	m_AnimInstanceConvertLog->ClearWidget();

	CIMGUIText* StartText = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");
	StartText->SetText("Convert Start...");


	// ���� ��� �༮��� Mesh Load �ϰ� 
	size_t Size = m_vecAnimationSeqFilesFullPath.size();

	CIMGUIText* Text = nullptr;

	for (size_t i = 0; i < Size; ++i)
	{
		std::string AddedKeyName;
		AddSequenceToDummyAnimationInstance(m_vecAnimationSeqFilesFullPath[i].c_str(), AddedKeyName);

		// File �̸� Log ��Ͽ� �߰�
		Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");

		Text->SetText(CEditorUtil::FilterFileName(m_vecAnimationSeqFilesFullPath[i]).c_str());
	}

	m_AnimInstanceProgressBar->SetPercent(100.f);

	Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("OK");
	Text->SetText("Complete!");
	MessageBox(nullptr, TEXT("Instance Create �Ϸ�"), TEXT("�Ϸ�"), MB_OK);

	// Dummy Animation Intance �� �����Ѵ�.
	if (!m_DummyAnimation)
		return;

	const PathInfo* Path = CPathManager::GetInst()->FindPath(ANIMATION_PATH);

	char SavedFileNameMultibyte[MAX_PATH] = {};

	if (Path)
		strcpy_s(SavedFileNameMultibyte, Path->PathMultibyte);

	strcat_s(SavedFileNameMultibyte, m_SavedAnimFileName->GetTextUTF8());
	strcat_s(SavedFileNameMultibyte, ".anim");

	m_DummyAnimation->SetSavedFileName(m_SavedAnimFileName->GetTextUTF8());
	m_DummyAnimation->SaveAnimationFullPath(SavedFileNameMultibyte);

	SAFE_DELETE(m_DummyAnimation);
}

void CAnimationEditor::OnAnimInstanceConvertLoading(const LoadingMessage& msg)
{
	m_AnimInstanceProgressBar->SetPercent(msg.Percent);

	CIMGUIText* Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");

	std::string LoadingMsg = "Add Success : " + msg.Message;

	Text->SetText(LoadingMsg.c_str());

	if (msg.Complete)
	{
		Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("OK");
		Text->SetText("Complete!");
		MessageBox(nullptr, TEXT("Instance Create �Ϸ�"), TEXT("�Ϸ�"), MB_OK);

		// Dummy Animation Intance �� �����Ѵ�.
		if (!m_DummyAnimation)
			return;

		m_DummyAnimation->SaveAnimationFullPath(m_SelectedSeqSrcsDirPath);

		SAFE_DELETE(m_DummyAnimation);
	}
}

void CAnimationEditor::AddSequenceToDummyAnimationInstance(const char* FileFullPath, std::string& AddedKeyName)
{
	if (!m_DummyAnimation)
	{
		m_DummyAnimation = new CAnimationSequenceInstance;

		if (!m_DummyAnimation->Init())
		{
			SAFE_DELETE(m_DummyAnimation);
			return;
		}
	}

	char FileFullPathCopy[MAX_PATH] = {};

	strcpy_s(FileFullPathCopy, FileFullPath);

	char SqcFileName[MAX_PATH] = {};
	char SqcExt[_MAX_EXT] = {};

	_splitpath_s(FileFullPathCopy, nullptr, 0, nullptr, 0, SqcFileName, MAX_PATH, SqcExt, _MAX_EXT);

	_strupr_s(SqcExt);

	// Ȯ���� .sqc �� �ƴ϶�� return;
	if (strcmp(SqcExt, ".SQC") != 0)
		return;

	// FileFullPath ���� File �̸����� Key ���� �������� ���̴�.
	// �ߺ� ���� 
	if (m_DummyAnimation->FindAnimation(SqcFileName))
		return;

	// Animation Seq Load
	std::string FSequenceName;

	// Load �� ���� ������ �̸��� �����ͼ� �����Ѵ�.
	bool ResultLoadSeq = CSceneManager::GetInst()->GetScene()->GetResource()->LoadAnimationSequenceFullPathMultibyte(true,
		SqcFileName, FileFullPath);

	if (!ResultLoadSeq)
		return;

	CAnimationSequence* LoadedSequence = CSceneManager::GetInst()->GetScene()->GetResource()->FindAnimationSequence(SqcFileName);

	LoadedSequence->SetAnimationFullPathMultibyte(FileFullPath);
	LoadedSequence->SetAnimationFileNameMultibyte(SqcFileName);

	// ex) File �̸��� Single_Idle.sqc ���
	// Resource Manager, Animation Instance �� ���
	// �����ϰ� Single_Idle �̶�� Key ������ �ش� ������ ������ ���̴�.
	m_DummyAnimation->AddAnimation(SqcFileName, SqcFileName);

	AddedKeyName = SqcFileName;
}

// Return ���� false ���, ��� �����Ͽ�, Make Inst
// true ��� , �ߺ� ������ �߻��ϴ� ���̹Ƿ�, Make Inst X
bool CAnimationEditor::CheckSavedFileNameDuplicated()
{
	// SaveFileName Input �� Text �� ����ִٸ� skip
	if (m_SavedAnimFileName->Empty())
		return true;

	const PathInfo* Info = CPathManager::GetInst()->FindPath(ANIMATION_PATH);

	if (!Info)
	{
		assert(false);
	}

	std::string SavedFileName = m_SavedAnimFileName->GetTextUTF8();

	if (SavedFileName.find(".anim") == std::string::npos)
		SavedFileName.append(".anim");

	return CEditorUtil::IsFileExistInDir(ANIMATION_PATH, SavedFileName);
}

bool CAnimationEditor::CheckGatheredSeqFilesIncludeCommonName()
{
	if (m_vecAnimationSeqFilesFullPath.size() > 0)
	{
		bool CommonNameIncluded = false;

		// && m_vecAnimationSeqFilesFullPath[0].find(m_CommonAnimSeqName->GetTextUTF8()) == std::string::npos
		std::vector<std::string> vecEachUpperSeqFilesFullPath;
		std::vector<std::string> vecEachLowerSeqFilesFullPath;
		std::string AllUpper;
		std::string AllLower;

		CEditorUtil::GetAllKindsOfTransformedStringVersions(m_CommonAnimSeqName->GetTextUTF8(), vecEachLowerSeqFilesFullPath,
			vecEachUpperSeqFilesFullPath, AllUpper, AllLower);

		size_t EachUpperTotalSize = vecEachUpperSeqFilesFullPath.size();

		for (size_t i = 0; i < EachUpperTotalSize; ++i)
		{
			// ���࿡ �ϳ��� �����Ѵٸ�, ���� Folder ��ο� Common Name �� �� ¦������ ���̴�.
			if (m_vecAnimationSeqFilesFullPath[0].find(vecEachUpperSeqFilesFullPath[i]) != std::string::npos)
			{
				CommonNameIncluded = true;
				break;
			}
		}

		if (!CommonNameIncluded)
		{
			size_t EachLowerTotalSize = vecEachLowerSeqFilesFullPath.size();

			for (size_t i = 0; i < EachLowerTotalSize; ++i)
			{
				// ���࿡ �ϳ��� �����Ѵٸ�, ���� Folder ��ο� Common Name �� �� ¦������ ���̴�.
				if (m_vecAnimationSeqFilesFullPath[0].find(vecEachLowerSeqFilesFullPath[i]) != std::string::npos)
				{
					CommonNameIncluded = true;
					break;
				}
			}
		}

		if (!CommonNameIncluded)
		{
			// ���࿡ �ϳ��� �����Ѵٸ�, ���� Folder ��ο� Common Name �� �� ¦������ ���̴�.
			if (m_vecAnimationSeqFilesFullPath[0].find(AllUpper) != std::string::npos)
				CommonNameIncluded = true;
		}

		if (CommonNameIncluded)
		{
			// ���࿡ �ϳ��� �����Ѵٸ�, ���� Folder ��ο� Common Name �� �� ¦������ ���̴�.
			if (m_vecAnimationSeqFilesFullPath[0].find(AllLower) != std::string::npos)
				CommonNameIncluded = true;
		}

		// ���� Folder �� Ư�� �̸��� �����ϴ� File ���� ��Ƴ��µ�
		// �ش� File �� ��, ù��° ������, Common Name �� Include ���� �ʴ´ٴ� �ǹ̴�
		// �߰��� Common NAme �� �ٲٴ� ���� ��ȭ�� �־��ٴ� ���̴�.
		if (!CommonNameIncluded)
			return false;
	}

	return true;
}

void CAnimationEditor::GatherFullPathInfoOfSqcFilesInSelectedDir(const std::string& SelectedDirPath, const std::string& CommonSqcName)
{
	strcpy_s(m_SelectedSeqSrcsDirPath, SelectedDirPath.c_str());

	// ��� .sqc Ȯ���� ���� �̸����� ������. FullPath ���� ��Ƶ� ���̴�.
	std::vector<std::string> vecSeqFilesFullPath;

	// ���� �ش� .sqc Ȯ���ڸ� ����, FullPath ��ϵ��� vector ���·� ��Ƶ־� �Ѵ�.
	// CEditorUtil::GetAllFileNamesInDir(m_SelectedSeqSrcsDirPath, vecSeqFilesFullPath, ".sqc");
	CEditorUtil::GetAllFileFullPathInDir(m_SelectedSeqSrcsDirPath, vecSeqFilesFullPath, ".sqc");

	// vecAllAnimationSeqFilesFullPath �� Ư�� ���ڸ� �����ϴ� (�빮��, �ҹ��� ���� ���) .sqc ���ϵ��� ��� ��Ƴ��� ���̴�.
	std::vector<std::string> vecAllAnimationSeqFilesFullPath;
	vecAllAnimationSeqFilesFullPath.reserve(100);

	m_vecAnimationSeqFilesFullPath.clear();
	m_vecAnimationSeqFilesFullPath.reserve(100);

	// 1. Origin �Է� ���� �״���� ���� �̸��� �����ϴ� ���� ���� ������
	// ex) "zed" => ��� "zed~~" ������ ���� �̸��� ����

	// CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, m_CommonAnimSeqName->GetTextUTF8());
	CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, CommonSqcName.c_str());

	// ��� .sqc ���ϵ��� m_vecAnimationSeqFilesFullPath �� ��Ƶα� 
	size_t AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

	for (size_t i = 0; i < AllSeqFilesCnt; ++i)
	{
		// �ߺ� ����
		if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
			continue;

		m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
	}

	// 2. ���� �ҹ���, �빮��
	//    ��� �ҹ���, ��� �빮��
	std::vector<std::string> vecEachUpperSeqFilesFullPath;
	std::vector<std::string> vecEachLowerSeqFilesFullPath;
	std::string AllUpper;
	std::string AllLower;

	// CEditorUtil::GetAllKindsOfTransformedStringVersions(m_CommonAnimSeqName->GetTextUTF8(), vecEachLowerSeqFilesFullPath,
	// 	vecEachUpperSeqFilesFullPath, AllUpper, AllLower);	
	CEditorUtil::GetAllKindsOfTransformedStringVersions(CommonSqcName.c_str(), vecEachLowerSeqFilesFullPath,
		vecEachUpperSeqFilesFullPath, AllUpper, AllLower);

	// ���� �빮�� ��ϵ� ��Ƶα�
	size_t EachUpperTotSize = vecEachUpperSeqFilesFullPath.size();

	for (size_t i = 0; i < EachUpperTotSize; ++i)
	{
		// FilterSpecificNameIncludedFilePath ������ vecAllAnimationSeqFilesFullPath �� ����ش�.
		CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, vecEachUpperSeqFilesFullPath[i].c_str());

		AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

		for (size_t i = 0; i < AllSeqFilesCnt; ++i)
		{
			if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
				continue;

			m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
		}
	}

	// ���� �ҹ��� ��ϵ� ��Ƶα�
	size_t EachLowerTotSize = vecEachLowerSeqFilesFullPath.size();

	for (size_t i = 0; i < EachLowerTotSize; ++i)
	{
		// FilterSpecificNameIncludedFilePath ������ vecAllAnimationSeqFilesFullPath �� ����ش�.
		CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, vecEachLowerSeqFilesFullPath[i].c_str());

		AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

		for (size_t i = 0; i < AllSeqFilesCnt; ++i)
		{
			if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
				continue;

			m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
		}
	}


	// All �빮�� ��ϵ� ��Ƶα�
	CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, AllUpper.c_str());

	AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

	for (size_t i = 0; i < AllSeqFilesCnt; ++i)
	{
		if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
			continue;

		m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
	}

	// All �ҹ��� ��ϵ� ��Ƶα�
	CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, AllLower.c_str());

	AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

	for (size_t i = 0; i < AllSeqFilesCnt; ++i)
	{
		if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
			continue;

		m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
	}


	// �ϳ��� ã�Ƴ��� ���ߴٸ�.
	if (m_vecAnimationSeqFilesFullPath.size() == 0)
	{
		MessageBox(nullptr, TEXT("No .sqc Files Found"), TEXT(".sqc File ã�� ����"), MB_OK);
		return;
	}

	m_AnimSeqcSrcFolderPath->SetText(m_SelectedSeqSrcsDirPath);
}
