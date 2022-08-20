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

	// 별도 Render Target
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
	// Play Scale 조정
	CIMGUIDummy* Dummy = AddWidget<CIMGUIDummy>("Dummy", 200.f, 30.f);

	m_PlayScaleInput = AddWidget<CIMGUITextInput>("Play Scale Input", 50.f, 30.f);
	m_PlayScaleInput->SetHideName(true);
	m_PlayScaleInput->SetTextType(ImGuiText_Type::Float);

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(60.f);

	m_PlayScaleEditBtn = AddWidget<CIMGUIButton>("Edit Scale", 100.f, 20.f);
	m_PlayScaleEditBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnEditAnimPlayScale);

	// Play Time 조정
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

	// Sequence 원본 Frame 을 반영해서 PlayTime 을 재조정해주는 함수 
	m_SetOriginalPlayTimeBtn = AddWidget<CIMGUIButton>("Set Origin Time", 100.f, 20.f);
	m_SetOriginalPlayTimeBtn->SetClickCallback<CAnimationEditor>(this, &CAnimationEditor::OnSetOriginalAnimPlayTime);

	// 각종 체크 박스들 
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
	const char* AddSeqHelpText = R"(ex)  'ZedIdle' -- > pair('ZedIdle', 'ZedIdle.sqc') 형태로 
	SceneResource, ResourceManager의 m_mapSequence 에 저장")";
	HelpText->SetText(AddSeqHelpText);
	HelpText->SetIsHelpMode(true);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	HelpText = AddWidget<CIMGUIText>("Anim Seq Load Btn Help Text", 90.f, 30.f);
	const char* ResourceAnimKeyHelpText = R"(ex) 'Idle' --> m_Animation->AddAnimation('ZedIdle', 'Idle') ? 
ZedIdle Key로 ResourceManager 의 mapSequence 에 저장된 Animation Sequence 를 
'Idle' 이라는 이름의 Key값으로 AnimationInstance 에 정보 추가)";
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
	const char* EditKeyHelpText = R"(ex) 'EditIdle' --> 기존의 m_Animation->AddAnimation('ZedIdle', 'Idle') 으로 인해 만들어진 m_mapAnimationSequence['Idle'] = 'ZedIdle' 을 
m_mapAnimationSequence['EditIdle'] = 'ZedIdle' 로 Key 값 수정 )";
	HelpText->SetText(EditKeyHelpText);
	HelpText->SetIsHelpMode(true);

	// Animation Instance Key Name 수정 기능
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
	HelpText->SetText("같은 경로에 .msh , .bne, .fbm(폴더) 가 존재해야 한다. \n ex) Idle1.sqc 를 Add 하려면, Idle1.msh, Idle1.bne, Idle1.fbm이 같은 경로 내에 존재해야 한다.");
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
	HelpText->SetText("Bin\\Animation 폴더에 .anim 파일 확장자로 저장해야 한다.");
	HelpText->SetIsHelpMode(true);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	// Animation Related Btns
	HelpText = AddWidget<CIMGUIText>("Instance Load Btn Help Text", 90.f, 30.f);
	const char* InstLoadHelpText = R"(.anim 파일을 Load 하려면, MESH_PATH (Bin//Mesh) 경로에 관련 .msh , .bne, .fbm(폴더) 가 존재해야 한다. 
ex) Alistar.anim 를 Load 하려면, 
MESH_PATH 에 Alistar_Idle.sqc, Alistar_Idle.msh, Alistar_Idle.fbm 등, 
Alistar Animation 과 관련된 파일들이 하나는 존재햐야 한다)";
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

	// Table Key 값 정보 세팅
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameRange);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameLength);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameMode);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::PlayTime);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::FrameTime);
	m_AnimInfoTable->MakeKey(AnimationClipInfoKeys::PlayScale);

	// Animation 모아서 만들기
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
	HelpText->SetText("ZedIdle 이라고 하면, Bin/Animation 폴더에 ZedIdle.anim 이름으로 Animation Instance 저장");
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

	// Load 된 Excel File Name
	m_LoadedExcelFileName = AddWidget<CIMGUITextInput>("Loaded Excel Data Name", 150.f, 30.f); // 맨 첫번째 행 
	m_LoadedExcelFileName->SetHideName(true);
	m_LoadedExcelFileName->SetHintText("Loaded Excel Name");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	HelpText = AddWidget<CIMGUIText>("Loaded Excel Data", 90.f, 30.f);
	HelpText->SetText("Load 된 Excel 의 Data Name (첫번째 행)");
	HelpText->SetIsHelpMode(true);

	// Excel 내용에 기초하여 만들어낼 .anim 파일 이름
	m_ExcelSavedAnimFileName = AddWidget<CIMGUITextInput>("Saved Anim Name", 150.f, 30.f); // 맨 첫번째 행 
	m_ExcelSavedAnimFileName->SetHideName(true);
	m_ExcelSavedAnimFileName->SetHintText("Saved Anim Name");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	HelpText = AddWidget<CIMGUIText>("Excel Saved Anim Name", 90.f, 30.f);
	HelpText->SetText("만들어낼 .anim 파일의 이름 -> .anim 확장자 없이, 파일 이름만 입력하세요");
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

	// Animation Slider 증가
	if (m_Animation && m_Animation->GetCurrentAnimation())
	{
		int NextAnimationIdx = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetCurrentFrameIdx();
		OnAnimationSliderIntCallback(NextAnimationIdx);
	}

	// 별도 RenderTarget 세팅 (처음 한번만)
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
	
	// Animation Mesh Component 에서 다시 새롭게 Animation Instance 를 만들어낸다.
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

	// 3DTest Object의 Animation 정보를 가져온다.
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

		// 확장자 .anim 이 아니라면 return;
		if (strcmp(FileExt, ".CSV") != 0)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .csv"), NULL, MB_OK);
			return;
		}

		// Sub Folder 에 있을 경우에도, Load  될 수 있게 세팅하기 
		std::string LoadedExcelKey;

		bool LoadResult = CResourceManager::GetInst()->GetExcelManager()->LoadCSVFullPath(LoadedExcelKey, FilePathMultibyte);

		if (!LoadResult)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Excel Load 실패. Excel 이 열려져있는지 확인해보세요"), NULL, MB_OK);
			return;
		}

		// 기존 KeyName 의 복사본을 만든다.
		std::string PrevExcelKeyName = m_ExcelKeyName;

		m_ExcelKeyName = LoadedExcelKey;

		CExcelData* LoadedExcelData = CResourceManager::GetInst()->GetExcelManager()->FindCSV(LoadedExcelKey);

		// Load 된 이름 세팅
		std::string LoadedExcelName = LoadedExcelData->GetName();

		m_LoadedExcelFileName->SetText(LoadedExcelName.c_str());

		// 같은 Excel 을 Load 한 게 아니라면, 기존의 것은 지운다
		if (PrevExcelKeyName != LoadedExcelKey)
		{
			// Excel Data 내용 새롭게 세팅
			// Excel Manager 에서 기존 Excel 내용을 지운다.
			CResourceManager::GetInst()->GetExcelManager()->DeleteCSV(PrevExcelKeyName);
		}

		m_LoadedExcelData = LoadedExcelData;
	}
}

void CAnimationEditor::OnMakeAnimInstByExcel()
{
	// Excel 이 존재해야 한다.
	if (!m_LoadedExcelData)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Excel 파일을 Load 하세요"), NULL, MB_OK);
		return;
	}

	// Animation 이 존재해야 한다.
	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// m_ExcelSavedAnimFileName 에 이름이 채워져 있어야 한다.
	if (m_ExcelSavedAnimFileName->Empty())
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT(".anim 파일 이름을 지정하세요"), NULL, MB_OK);
		return;
	}

	int OriginAnimStartFrame = 0;
	int OriginAnimEndFrame = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetFrameLength() - 1;

	// >> Excel 내에 모든 sqc 정보들을 돌면서, 범위를 벗어나는 정보인지를 확인한다. 
	const Table& ExcelTableData = m_LoadedExcelData->GetTable();

	{
		auto iter = ExcelTableData.begin();
		auto iterEnd = ExcelTableData.end();

		for (; iter != iterEnd; ++iter)
		{
			// 1번째는 startFrame
			// 2번째는 endFrame
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
				TCHAR ErrorMessage[MAX_PATH] = TEXT(": Start Frame 의 범위를 벗어납니다. Origin Start Frame : ");
				TCHAR StartFrameMessage[MAX_PATH];
				lstrcpy(StartFrameMessage, CEditorUtil::ChangeMultibyteTextToTCHAR(std::to_string(OriginAnimStartFrame).c_str()));

				lstrcat(FulllErrorMessage, ErrorMessage);
				lstrcat(FulllErrorMessage, StartFrameMessage);
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);

				return;
			}
			if (OriginAnimEndFrame < EdFrameInt)
			{
				TCHAR ErrorMessage[MAX_PATH] = TEXT(": End Frame 의 범위를 벗어납니다. Origin End Frame : ");
				TCHAR EndFrameMessage[MAX_PATH];
				lstrcpy(EndFrameMessage, CEditorUtil::ChangeMultibyteTextToTCHAR(std::to_string(OriginAnimEndFrame).c_str()));

				lstrcat(FulllErrorMessage, ErrorMessage);
				lstrcat(FulllErrorMessage, EndFrameMessage);
				MessageBox(CEngine::GetInst()->GetWindowHandle(), FulllErrorMessage, NULL, MB_OK);
				return;
			}
		}
	}

		
	// >> 현재 Load 된 sqc 가 위치한 Folder 경로에 여러개 sqc 파일들로 저장해서 만들어준다. 
	CMesh* CurrentUsedMesh = CResourceManager::GetInst()->FindMesh(m_3DTestObjectMeshName);

	// Extension 포함 이름 ex) CombinedPot.msh
	const std::string& UsedMeshFileName = CEditorUtil::FilterFileName(CurrentUsedMesh->GetFullPath());

	// 확장자 제외 파일 이름
	std::string UsedMeshFileNameOnly;
	CEngineUtil::GetFileNameOnly(UsedMeshFileName, UsedMeshFileNameOnly);

	// Full 경로 정보가 들어있다.
	auto MeshFileFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedMeshFileName);
	
	// Folder 경로
	std::string CommonFolderPath;
	CEngineUtil::GetPathInfoBeforeFileName(MeshFileFoundResult.value(), CommonFolderPath);

	// Animation Sequence 정보
	CAnimationSequence* ExistingSequeunce = m_Animation->GetCurrentAnimation()->GetAnimationSequence();

	// >> 여러개의 Sqc 들을 만든다.
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

			// 1번째는 startFrame
			// 2번째는 endFrame
			const std::string& StFrame = (*(iter->second))[0];
			int StFrameInt;
			auto StFrameIntInfo = std::from_chars(StFrame.c_str(), StFrame.c_str() + sizeof(StFrame.c_str()) - 1, StFrameInt);

			const std::string& EdFrame = (*(iter->second))[1];
			int EdFrameInt;
			auto EdFrameIntInfo = std::from_chars(EdFrame.c_str(), EdFrame.c_str() + sizeof(EdFrame.c_str()) - 1, EdFrameInt);


			TCHAR TCHARSavedSqcFullPath[MAX_PATH];

			lstrcpy(TCHARSavedSqcFullPath, CEditorUtil::ChangeMultibyteTextToTCHAR(NewlySavedSqcFullPath.c_str()));

			// 하나라도 Load 실패시 X 
			// 처음 한개만 msh, fbm, bne 파일 복사본을 만들어줄 것이다.
			if (!SaveEditedSqcFile(TCHARSavedSqcFullPath, ExistingSequeunce, StFrameInt, EdFrameInt, FirstData))
			{
				MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT(".sqc 파일 저장 실패"), NULL, MB_OK);
				return;
			}

			FirstData = false;
		}
	}

	// >> 폴더 내에 저장된 sqc 들을 하나의 anim 파일을 만들어낸다. (Save 까지)
	// 새롭게 생성된 sqc + 기존 sqc 파일 모두 UsedMeshFileNameOnly 라는 이름을 포함한다.
	std::string FolderPathWithOutSlash = std::string(CommonFolderPath.begin(), CommonFolderPath.end() - 1);
	GatherFullPathInfoOfSqcFilesInSelectedDir(FolderPathWithOutSlash, UsedMeshFileNameOnly);

	// >> 기존 sqc 는 .anim 으로 만들 .sqc 목록에서 제외해준다.
	//  m_vecAnimationSeqFilesFullPath;
	int TargetSqcFullPathSize = (int)m_vecAnimationSeqFilesFullPath.size();

	std::string ExisintSeqFileName;
	ExisintSeqFileName = UsedMeshFileNameOnly;
	if (ExisintSeqFileName.find(".sqc") == std::string::npos)
		ExisintSeqFileName.append(".sqc");

	for (int i = 0; i < TargetSqcFullPathSize; ++i)
	{
		// file 이름만을 뽑아낸다. (확장자 포함)
		const std::string& CurSqcFileName = CEditorUtil::FilterFileName(m_vecAnimationSeqFilesFullPath[i]);

		// 기존 sqc 파일 이름과 같은지 확인한다. -> 같으면 제거하고 return
		if (CurSqcFileName == ExisintSeqFileName)
		{
			m_vecAnimationSeqFilesFullPath.erase(m_vecAnimationSeqFilesFullPath.begin() + i);
			break;
		}
	}

	// >> 하나의 anim 파일을 만들어서 저장한다.
	// 로그창 클리어
	m_AnimInstanceConvertLog->ClearWidget();

	CIMGUIText* StartText = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");
	StartText->SetText("Convert Start...");


	// 모은 모든 녀석들로 Mesh Load 하고 
	size_t Size = m_vecAnimationSeqFilesFullPath.size();

	CIMGUIText* Text = nullptr;

	bool DummyAnimationMadeSuccess = true;

	for (size_t i = 0; i < Size; ++i)
	{
		// Grunt_Idle 과 같이, '_' 뒤에 Excel 에 해당하는 Lable 이 놓일 것이다.
		std::string AddedKeyName;

		AddSequenceToDummyAnimationInstance(m_vecAnimationSeqFilesFullPath[i].c_str(), AddedKeyName);

		// 해당 Key 이름을, Excel 에 저장된 Layer 이름으로 바꿔준다.
		// 이를 위해, Excel 에 저장된 Layer 중에서, 자신이 현재 포함하고 있는 Layer 정보를 세팅해줄 것이다.
		auto iterSqc = ExcelTableData.begin();
		auto iterSqcEnd = ExcelTableData.end();

		int AddedKeyNameLength = (int)AddedKeyName.length();

		// AddedKeyName 중에서 '_' 뒤의 문자를 가져온다. 
		// 단, 맨 뒤에서부터 검사한다. (따라서, str.find 가 아니라, rfind 를 사용할 것이다.)
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
			MessageBox(nullptr, TEXT("Label 이름에 '_' 이 포함되지 않게 하세요. 혹은, 기존에 만든 sqc 들을 다 지워주고 다시 시도하세요"), TEXT("실패"), MB_OK);
			break;
		}

		// Dummy Animation 을 통해 찾아야 한다.
		// Dummy Animation 상에서 KeyName 을 수정한다.
		if (!m_DummyAnimation->EditCurrentSequenceKeyName(NewLableKeyName.c_str(), AddedKeyName))
		{
			DummyAnimationMadeSuccess = false;
			assert(false);
			return;
		}
		// 뿐만 아니라, AnimationSequenceData 의 m_Name 도 수정해야 한다.
// 그리고 현재 수정하는 AnimationSequenceData 는 m_DummyAnimation 에 저장되어 있는 녀석이어야 한다.
		CAnimationSequenceData* AnimSeqData = m_DummyAnimation->FindAnimation(NewLableKeyName);
		AnimSeqData->SetName(NewLableKeyName);

		AnimSeqData->SetOriginalFramePlayTime();
		// File 이름 Log 목록에 추가
		Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");

		Text->SetText(CEditorUtil::FilterFileName(m_vecAnimationSeqFilesFullPath[i]).c_str());

	}

	if (DummyAnimationMadeSuccess == false)
	{
		m_AnimInstanceConvertLog->ClearWidget();
		SAFE_DELETE(m_DummyAnimation);
		MessageBox(nullptr, TEXT("Instance Create 실패"), TEXT("실패"), MB_OK);
		return;
	}

	m_AnimInstanceProgressBar->SetPercent(100.f);

	Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("OK");
	Text->SetText("Complete!");

	// Dummy Animation Intance 를 저장한다.

	const PathInfo* Path = CPathManager::GetInst()->FindPath(ANIMATION_PATH);

	std::string SavedFileNameMultibyte;
	SavedFileNameMultibyte.reserve(strlen(Path->PathMultibyte) * 2);

	// 확장자 포함
	std::string SaveAnimFileName;
	SaveAnimFileName.reserve(strlen(m_ExcelSavedAnimFileName->GetTextUTF8()) * 2);

	if (Path)
		SavedFileNameMultibyte = Path->PathMultibyte;

	SaveAnimFileName = m_ExcelSavedAnimFileName->GetTextUTF8();
	
	// .anim 확장자 붙여주기
	if (SaveAnimFileName.find(".anim") == std::string::npos)
		SaveAnimFileName.append(".anim");

	SavedFileNameMultibyte.append(SaveAnimFileName);

	m_DummyAnimation->SetSavedFileName(SaveAnimFileName.c_str());
	m_DummyAnimation->SaveAnimationFullPath(SavedFileNameMultibyte.c_str());

	MessageBox(nullptr, TEXT("Instance Create 완료"), TEXT("완료"), MB_OK);

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

		// 확장자 .anim 이 아니라면 return;
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

		// 확장자 .anim 이 아니라면 return;
		if (strcmp(Ext, ".ANIM") != 0)
			return;

		if (!m_Animation)
		{
			// ex) singed_spell2.sqc 를 선택했다면
			// 같은 폴더 목록 내에서 singed_spell2.msh / singed_spell2.bne 를 Load 하여 세팅한다.
			// singed_spell2.msh -> singed_spell2_mesh 라는 이름으로
			// singed_spell2.bne -> singed_spell2_skeleton 이라는 이름으로
			OnCreateSample3DObject();
		}
		else
		{
			// 기존 Animation List에 보여지던 , 즉, 현재 Animation에 Added 되었던 모든 Sequence 정보를 지워준다
			OnDeleteExisting3DObject();

			OnCreateSample3DObject();
		}

		// TODO : Animation Instance 는 그냥 지워줘 버리면 안되나 ?
		m_Animation->LoadAnimationFullPath(FilePathMultibyte);

		// CurrentAnimation 이 없다면, 
		if (!m_Animation->GetCurrentAnimation())
		{
			// .anim File 이 정상적으로 Load 되지 않았다는 의미이다.
			// - 그러면 기존에 생성해둔 3DObject 를 지워서 Animation Mesh Component 가 그려지지 않게 해야 한다.
			OnDeleteExisting3DObject();
			return;
		}

		std::pair<bool, std::string> LoadResult = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(m_Animation);

		if (LoadResult.first == false)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Mesh, Texture, Skeleton Load Failure"), NULL, MB_OK);

			// 만약 Load 를 실패했다면, 
			OnDeleteExisting3DObject();

			return;
		}

		// 해당 이름을 3d Test Object 에서 가져와서 사용할 것이다.
		m_3DTestObjectMeshName = LoadResult.second;

		// Set Material, Mesh Info
		SetMeshMaterialReadyForAnimation();

		// ComboBox 에 모든 Seq 내용을 추가해준다
		m_CurAnimComboBox->Clear();

		int Size = m_Animation->GetAnimationCount();
		std::vector<std::string> vecSeqNames;
		vecSeqNames.reserve(Size);

		m_Animation->GatherSequenceNames(vecSeqNames);

		for (int i = 0; i < Size; i++)
		{
			m_CurAnimComboBox->AddItem(vecSeqNames[i]);
		}

		// Current Animation 정보로 세팅한다.
		int CurAnimIdx = m_Animation->GetCurrentAnimationOrder();

		if (CurAnimIdx == -1)
		{
			assert(false);
			return;
		}

		m_CurAnimComboBox->SetSelectIndex(CurAnimIdx);

		// 현재 Select 된 Animation 의 최소, 최대 Frame 으로 Frame Slider 범위 세팅
		m_FrameSlider->SetMin(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetStartFrame());
		m_FrameSlider->SetMax(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetEndFrame());

		// Table 정보 세팅
		OnRefreshAnimationClipTable(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

		// 현재 Scene에 모든 Sequence 내용을 추가한다.
		// m_Animation->AddAnimationSequenceToSceneResource();
		OnRefreshFrameSliderInfo(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

		// 현재 Scene의 정보를 m_Scene으로 지정해준다
		m_Animation->SetScene(CSceneManager::GetInst()->GetScene());

		// Refresh CheckBox Info
		OnRefreshCheckBoxInfo();

		// Animation Play Scale, Time
		OnRefreshScaleAndTimeInputInfo();

		// Animation을 시작한다..
		m_Animation->Play();

		// Load 한 Animation File 의 FileName 을 세팅한다.
		// .anim 확장자가 만일 없다면, .anim 확장자를 붙여서 Text 를 Set
		std::string LoadedName = m_Animation->GetSavedFileName();
		
		if (LoadedName == "") // Loaded FileName 이 없다면 안된다.
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

	// 범위를 넘어가면 조정해준다.
	int InputFrame = m_FrameInput->GetValueInt();
	int StFrame = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetStartFrame();
	int EdFrame = m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetEndFrame();

	if (InputFrame < StFrame)
		InputFrame = EdFrame;
	else if (InputFrame > EdFrame)
		InputFrame = StFrame;

	m_FrameInput->SetInt(InputFrame);

	// Input 의 Frame 으로 Animation Frame 정보를 세팅해준다.
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
	// 만약 Mesh Load 과정에서 필요한 Texture가 없다면 
	// ex) FBX Convert 이후, singed_spell2.sqc 가 있다면, 같은 경로내에 singed_spell2.fbm 이라는 디렉토리가 존재해야 한다.
	// 만약 해당 Folder 가 존재하지 않는다면, Mesh를 Load 하더라도 Texture 가 없다고 뜰 것이다
	char TextFolderExt[10] = ".fbm";
	char TextFolderFullPath[MAX_PATH] = {};

	// RESOURCE_MESH_PATH 에 있는 정보로부터 불러올 것이다.
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

	// ex) singed_spell2.sqc 를 선택했다면
	// 같은 폴더 목록 내에서 singed_spell2.msh / singed_spell2.bne 를 Load 하여 세팅한다.
	// singed_spell2.msh -> singed_spell2_mesh 라는 이름으로
	// singed_spell2.bne -> singed_spell2_skeleton 이라는 이름으로

	// Load Mesh
	std::string LoadedMeshName = SqcFileName;
	// LoadedMeshName.append("_mesh");

	// 해당 이름을 3d Test Object 에서 가져와서 사용할 것이다.
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

	// Mesh 에 해당 Skeleton 세팅
	CSceneManager::GetInst()->GetScene()->GetResource()->SetMeshSkeleton(LoadedMeshName, LoadedBneName); //


	return true;
}

void CAnimationEditor::SetMeshMaterialReadyForAnimation()
{
	m_3DTestObject->SetMeshAndMaterialInfo();

	m_Animation->Start();

	// 3DTest Object의 Animation 정보를 가져온다.
	CAnimationMeshComponent* AnimComponent = dynamic_cast<CAnimationMeshComponent*>(m_3DTestObject->GetRootComponent());

	if (AnimComponent == nullptr)
		assert(false);

	// Animation Display Window 에서 Scale 값 세팅해준다.
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

	// New Key 값과 중복된 Key 값의 Animation 이 있는지를 검사한다.
	if (m_Animation->FindAnimation(m_EditAnimSeqDataKeyName->GetTextUTF8()))
	{
		MessageBox(nullptr, TEXT("Sequence Exist"), TEXT("Same Named Seq File Already Exist"), MB_OK);
		return;
	}

	// New Key 값
	// EditCurrentSequenceKeyName
	if (!m_Animation->EditCurrentSequenceKeyName(m_EditAnimSeqDataKeyName->GetTextUTF8(), m_CurAnimComboBox->GetSelectItem()))
	{
		assert(false);
		return;
	}

	// 바꿔준 이름의 Animation 을  Current Animation 으로 세팅한다.
	m_Animation->SetCurrentAnimation(m_EditAnimSeqDataKeyName->GetTextUTF8());

	// Combo Box 내용 Refresh
	OnRefreshAnimationComboBox();

	// 새롭게 바뀐 SequenceKey Name 의 이름으로, ComboBox 세팅해두기
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

	// 현재 Current Animation 에 해당하는 Sequence 를 현재 Scene 에서 지워준다.
	CAnimationSequence* DeleteSeq = CResourceManager::GetInst()->FindAnimationSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetName());
	
	CSceneManager::GetInst()->GetScene()->GetResource()->DeleteSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

	m_Animation->DeleteCurrentAnimation();

	// ResourceManager 에 남아있는 Sqc 의 ref Cnt 가 1 이라면, 더이상 해당 sqc 를 사용하는 다른 Obj 가 없다는 것
	// 따라서 지워준다.
	CResourceManager::GetInst()->ReleaseAnimationSequence3D(DeleteSeq);


	// Combo Box 내용 Refresh
	// 비어있을 경우, ComboBox 안의 내용 ""로 세팅하는 기능까지 
	OnRefreshAnimationComboBox();

	if (!m_Animation->GetCurrentAnimation())
	{
		// 다 지웠다는 메세지
		MessageBox(nullptr, TEXT("모든 Sqc 파일이 Delete 되었습니다."), TEXT("Anim Seq Delete."), MB_OK);

		// Animation Info Table 깨끗하게
		m_AnimInfoTable->ClearContents();

		// 3D Object ? Animation Mesh Component 도 지워준다. 
		OnDeleteExisting3DObject();

		// Render Target 을 비워준다.
		CRenderManager::GetInst()->GetAnimationRenderTarget()->ClearTarget();

		// 혹시나 Load 된 .anim 파일 정보가 있다면 지워준다
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

	// 모든 Sequence 정보를 지워준다.
	while (m_Animation->GetCurrentAnimation())
	{
		// 현재 Current Animation 에 해당하는 Sequence 를 현재 Scene 에서 지워준다.
		CAnimationSequence* DeleteSeq = CResourceManager::GetInst()->FindAnimationSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence()->GetName());

		CSceneManager::GetInst()->GetScene()->GetResource()->DeleteSequence(m_Animation->GetCurrentAnimation()->GetAnimationSequence());

		m_Animation->DeleteCurrentAnimation();

		// ResourceManager 에 남아있는 Sqc 의 ref Cnt 가 1 이라면, 더이상 해당 sqc 를 사용하는 다른 Obj 가 없다는 것
		// 따라서 지워준다.
		CResourceManager::GetInst()->ReleaseAnimationSequence3D(DeleteSeq);
	}

	// Combo Box 내용 Refresh
	// 비어있을 경우, ComboBox 안의 내용 ""로 세팅하는 기능까지 
	OnRefreshAnimationComboBox();

	// 아래 함수가 호출되기 위해서는 m_Animation 이 더이상 CurrentAnimation을 가지고 있어서는 안된다.
	// 다 지웠다는 메세지
	MessageBox(nullptr, TEXT("모든 Sqc 파일이 Delete 되었습니다."), TEXT("Anim Seq Delete."), MB_OK);

	// Animation Info Table 깨끗하게
	m_AnimInfoTable->ClearContents();

	// 3D Object ? Animation Mesh Component 도 지워준다. 
	OnDeleteExisting3DObject();

	// Render Target 을 비워준다.
	CRenderManager::GetInst()->GetAnimationRenderTarget()->ClearTarget();

	// 혹시나 Load 된 .anim 파일 정보가 있다면 지워준다
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
	// Add 할 Animation 의 이름을 입력해야 한다
	if (m_NewAnimSeqName->Empty() || m_NewAnimSeqDataKeyName->Empty())
		return;

	// 현재 Seq Key 를 Key로 사용하고 있는 SceneResource 내의 Sequence 를 조사한다.
	// 그래야만, 내가 Add한 Key로, 불러오고자 하는 .sqc 파일을 제대로 불러온다.
	// 그렇지 않다면, 이미 SceneResource 에 존재하는 .sqc 를 불러오게 될 것이다.
	if (CSceneManager::GetInst()->GetScene()->GetResource()->FindAnimationSequence(m_NewAnimSeqName->GetTextUTF8()))
	{
		MessageBox(nullptr, TEXT("다른 Sqc Key 활용하세요"), TEXT("다른 Sqc Key 활용하세요"), MB_OK);
		return;
	}

	TCHAR LoadFilePath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = LoadFilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("모든파일\0*.*\0*.Scene File\0*.scn");
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
		
		// 확장자 .sqc 이 아니라면 return;
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
		
		// 이름 중복 X --> Key 이름 중복되는 Sequence 는 추가 X 
		// ex) AnimationInstance --> ("ZedIdle", "Idle"); --> "ZedIdle" 이라는 Key 를 지닌 또 다른 Sqc 파일을 로드하면 안된다.
		if (m_Animation->FindAnimation(m_NewAnimSeqDataKeyName->GetTextUTF8()))
			return;
		
		// Animation Seq Load
		std::string FSequenceName;
		
		// Load 한 파일 본연의 이름을 가져와서 세팅한다.
		bool ResultLoadSeq = CSceneManager::GetInst()->GetScene()->GetResource()->LoadAnimationSequenceFullPathMultibyte(true,
			m_NewAnimSeqName->GetTextUTF8(), FilePathMultibyte);
		
		if (!ResultLoadSeq)
			return;
		
		CAnimationSequence* LoadedSequence = CSceneManager::GetInst()->GetScene()->GetResource()->FindAnimationSequence(m_NewAnimSeqName->GetTextUTF8());
		
		LoadedSequence->SetAnimationFullPathMultibyte(FilePathMultibyte);
		LoadedSequence->SetAnimationFileNameMultibyte(SqcFileName);
		
		// 현재 
		m_Animation->AddAnimation(m_NewAnimSeqName->GetTextUTF8(), m_NewAnimSeqDataKeyName->GetTextUTF8());
		
		// Combo Box 정보 갱신
		OnRefreshAnimationClipTable(LoadedSequence);

		OnRefreshFrameSliderInfo(LoadedSequence);

		OnRefreshAnimationComboBox();
		
		OnRefreshScaleAndTimeInputInfo();

		OnRefreshCheckBoxInfo();
		
		// Frame Slider 의 최대 최소 값 세팅하기
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

	// 클릭한 Animation 으로 Current Animation 세팅
	m_Animation->SetCurrentAnimation(Name);

	// Table 정보 갱신
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

	// 맨 처음 Sequence를 추가하는 상황이라면
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
		// 처음 Animation 을 추가하는 상황이라면
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
	// Animation 객체 정보가 있어야 하고
	if (!m_3DTestObject)
		return;

	if (!m_Animation || !m_Animation->GetCurrentAnimation())
		return;

	// Frame 범위가 벗어나면 안된다. (Message Box)
	CAnimationSequence* ExistingSequence = m_Animation->GetCurrentAnimation()->GetAnimationSequence();

	if (m_StartFrameEditInput->GetValueInt() < 0 || m_EndFrameEditInput->GetValueInt() >= ExistingSequence->GetFrameLength())
	{
		MessageBox(nullptr, TEXT("Frame 범위를 벗어남."), TEXT("Frame 범위를 벗어남."), MB_OK);
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

	// 확장자 .sqc 이 아니라면 return;
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
		// bne, fbm, msh 파일을 복사해주지 않을 것이라면, 그냥 return true 하고 끝
		if (!MakeCopy)
			return true;

		// 현재 Sqc 가 사용하는 Mesh, Bne, Fbm 폴더 내용을 현재 저장한 폴더 경로에 그대로 복사하여 넣어주고
		// 해당 파일들의 이름은, 현재 저장한 Sqc 파일들과 동일하게 세팅한다.
		std::string SavedFolderPath;
		CEngineUtil::GetPathInfoBeforeFileName(FileFullPathMultibyte, SavedFolderPath);

		// (과정)
		// m_3DTestObjectMeshName 에 현재 사용하는 Mesh의 Name 이 세팅되어 있다.
		// 해당 Mesh Name 으로 Mesh 를 찾는다.
		// 각 Mesh 에는 저장될 당시의 FullPath 정보가 들어있다.
		// 해당 FullPath 정보를 이용해서, Mesh가 사용하는 FileName 을 가져와서
		// FileName.msh, FileName.bne, FileName.fbm 폴더를 찾아낸다.
		// 해당 파일들을, 새로 저장한 폴더에 복사하고
		// 복사한 파일들의 이름을 바꿔준다.
		CMesh* CurrentUsedMesh = CResourceManager::GetInst()->FindMesh(m_3DTestObjectMeshName);

		// Extension 포함 이름 ex) CombinedPot.msh
		const std::string& UsedMeshFileName = CEditorUtil::FilterFileName(CurrentUsedMesh->GetFullPath());
		auto MeshFileFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedMeshFileName);

		// Extension 제거
		std::string FileNameOnly;
		CEngineUtil::GetFileNameOnly(UsedMeshFileName, FileNameOnly);

		// Unicode 버전
		TCHAR TCHARCopyFileNameOnly[MAX_PATH];
		lstrcpy(TCHARCopyFileNameOnly, CEditorUtil::ChangeMultibyteTextToTCHAR(FileNameOnly.c_str()));

		// 복사할 .bne 파일, .fbm 폴더 이름을 세팅한다.
		std::string UsedBoneFileName = FileNameOnly;
		UsedBoneFileName.append(".bne");

		std::string UsedTextureFolderName = FileNameOnly;
		UsedTextureFolderName.append(".fbm");

		// (혹시나 하는 예외처리)
		// 해당 .msh, .bne, .fbn 이 경로내에 존재하는지 확인하기 => 나중에 필요하면 하기 
		// MESH_PATH 경로 안에 존재해야 한다.
		// 먼저 .bne 확인
		auto BoneFileFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedBoneFileName);

		TCHAR BneFulllErrorMessage[MAX_PATH] = {};
		TCHAR BneErrorMessage[MAX_PATH] = TEXT(".bne Does Not Exist in Bin//Mesh");
		lstrcpy(BneFulllErrorMessage, TCHARCopyFileNameOnly);
		lstrcat(BneFulllErrorMessage, BneErrorMessage);

		// 해당 경로에 Mesh 파일이 존재하지 않는다면 Return
		if (!BoneFileFoundResult.has_value())
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), BneFulllErrorMessage, NULL, MB_OK);
			TotalSaveResult = false;
			return false;
		}

		// 그 다음 Texture Folder 확인
		auto TextureFoundResult = CEngineUtil::CheckAndExtractFullPathOfTargetFile(MESH_PATH, UsedTextureFolderName);

		TCHAR FbmFulllErrorMessage[MAX_PATH] = {};
		TCHAR FbmErrorMessage[MAX_PATH] = TEXT(".fbm Does Not Exist in Bin//Mesh");
		lstrcpy(FbmFulllErrorMessage, TCHARCopyFileNameOnly);
		lstrcat(FbmFulllErrorMessage, FbmErrorMessage);

		// 해당 경로에 Mesh 파일이 존재하지 않는다면 Return
		if (!TextureFoundResult.has_value())
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), FbmFulllErrorMessage, NULL, MB_OK);
			TotalSaveResult = false;
			return false; 
		}

		// 복사
		std::string CopyMeshPath = SavedFolderPath;
		CopyMeshPath.append(UsedMeshFileName);

		std::string CopyBonePath = SavedFolderPath;
		CopyBonePath.append(UsedBoneFileName);

		std::string CopyTexturePath = SavedFolderPath;
		CopyTexturePath.append(UsedTextureFolderName);

		// 최종 복사된 경로 정보
		std::string FinalCopyMeshPath;
		std::string FinalCopyBonePath;
		std::string FinalCopyTexturePath;

		CEngineUtil::CopyFileToOtherDirectory(MeshFileFoundResult.value(), CopyMeshPath, FinalCopyMeshPath);
		CEngineUtil::CopyFileToOtherDirectory(BoneFileFoundResult.value(), CopyBonePath, FinalCopyBonePath);
		// 안에 있는 Texture 내용들도 Copy 시킬 것이다.
		CEngineUtil::CopyFileToOtherDirectory(TextureFoundResult.value(), CopyTexturePath, FinalCopyTexturePath, true);

		// 이름 수정
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
			MessageBox(nullptr, TEXT("Seq Edit & Save 실패 !!!."), TEXT("Seq Edit & Save 실패."), MB_OK);
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
	Bi.lpszTitle = TEXT("하나의 Instance 로 만들 Seq 파일들이 있는 폴더 선택");
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
		
		// Common Name 영역이 비어있으면 안된다
		if (m_CommonAnimSeqName->Empty())
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Common Sqc 이름을 입력하세요"), NULL, MB_OK);
			return;
		}

		GatherFullPathInfoOfSqcFilesInSelectedDir(SelectedDirPath, m_CommonAnimSeqName->GetTextUTF8());
	}
}

// m_ConvertAnimInstanceBtn 의 Callback
void CAnimationEditor::OnConvertSequencesIntoAnimationInstance()
{
	//m_SavedAnimFileName : Instance 를 만든 이후, .anim 파일로 저장할 때의 파일 이름
	if (m_AnimSeqcSrcFolderPath->Empty() || m_SavedAnimFileName->Empty())
		return;

	// 현재 m_SavedAnimFileName 이, Bin/Animation 에 이미 존재하는지 확인 + 중복 방지
	// return 값이 true 라면, 생성 X (중복된 이름의 .anim 파일이 이미 존재한다는 의미이기 때문이다)
	bool DuplicateResult = CheckSavedFileNameDuplicated();

	if (DuplicateResult)
	{
		MessageBox(nullptr, TEXT("해당 Folder 에 같은 이름의 .anim File 존재"), TEXT(".anim File Duplicate"), MB_OK);
		return;
	}

	// Animation Instance 를 만들어낼 .sqc 파일 목록들이 있는지 검사한다.
	if (m_vecAnimationSeqFilesFullPath.size() <= 0)
	{
		MessageBox(nullptr, TEXT("해당 Folder 에 해당 Common 이름을 포함한 .sqc 파일 존재 X"), TEXT("Common Name Error"), MB_OK);
		return;
	}

	// 다시 한번, 현재 모아둔 m_vecAnimationSeqFilesFullPath 에서, Common Name 이 포함되어 있는지 한번 더 검사한다.
	// 첫번째 File 로만 검사할 것이다.
	bool CheckCommonNameResult = CheckGatheredSeqFilesIncludeCommonName();

	if (!CheckCommonNameResult)
	{
		MessageBox(nullptr, TEXT("Folder 경로와, Common 이름을 확인하세요. (중간에 Common 이름을 변경했는지 확인)"), TEXT("Common Name Error"), MB_OK);
		return;
	}

	// 로그창 클리어
	m_AnimInstanceConvertLog->ClearWidget();

	CIMGUIText* StartText = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");
	StartText->SetText("Convert Start...");


	// 모은 모든 녀석들로 Mesh Load 하고 
	size_t Size = m_vecAnimationSeqFilesFullPath.size();

	CIMGUIText* Text = nullptr;

	for (size_t i = 0; i < Size; ++i)
	{
		std::string AddedKeyName;
		AddSequenceToDummyAnimationInstance(m_vecAnimationSeqFilesFullPath[i].c_str(), AddedKeyName);

		// File 이름 Log 목록에 추가
		Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("Text");

		Text->SetText(CEditorUtil::FilterFileName(m_vecAnimationSeqFilesFullPath[i]).c_str());
	}

	m_AnimInstanceProgressBar->SetPercent(100.f);

	Text = m_AnimInstanceConvertLog->AddWidget<CIMGUIText>("OK");
	Text->SetText("Complete!");
	MessageBox(nullptr, TEXT("Instance Create 완료"), TEXT("완료"), MB_OK);

	// Dummy Animation Intance 를 저장한다.
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
		MessageBox(nullptr, TEXT("Instance Create 완료"), TEXT("완료"), MB_OK);

		// Dummy Animation Intance 를 저장한다.
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

	// 확장자 .sqc 이 아니라면 return;
	if (strcmp(SqcExt, ".SQC") != 0)
		return;

	// FileFullPath 에서 File 이름으로 Key 값을 지정해줄 것이다.
	// 중복 방지 
	if (m_DummyAnimation->FindAnimation(SqcFileName))
		return;

	// Animation Seq Load
	std::string FSequenceName;

	// Load 한 파일 본연의 이름을 가져와서 세팅한다.
	bool ResultLoadSeq = CSceneManager::GetInst()->GetScene()->GetResource()->LoadAnimationSequenceFullPathMultibyte(true,
		SqcFileName, FileFullPath);

	if (!ResultLoadSeq)
		return;

	CAnimationSequence* LoadedSequence = CSceneManager::GetInst()->GetScene()->GetResource()->FindAnimationSequence(SqcFileName);

	LoadedSequence->SetAnimationFullPathMultibyte(FileFullPath);
	LoadedSequence->SetAnimationFileNameMultibyte(SqcFileName);

	// ex) File 이름이 Single_Idle.sqc 라면
	// Resource Manager, Animation Instance 에 모두
	// 동일하게 Single_Idle 이라는 Key 값으로 해당 정보를 세팅할 것이다.
	m_DummyAnimation->AddAnimation(SqcFileName, SqcFileName);

	AddedKeyName = SqcFileName;
}

// Return 값이 false 라면, 계속 진행하여, Make Inst
// true 라면 , 중복 현상이 발생하는 것이므로, Make Inst X
bool CAnimationEditor::CheckSavedFileNameDuplicated()
{
	// SaveFileName Input 에 Text 가 비어있다면 skip
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
			// 만약에 하나라도 포함한다면, 현재 Folder 경로와 Common Name 이 잘 짝지어진 것이다.
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
				// 만약에 하나라도 포함한다면, 현재 Folder 경로와 Common Name 이 잘 짝지어진 것이다.
				if (m_vecAnimationSeqFilesFullPath[0].find(vecEachLowerSeqFilesFullPath[i]) != std::string::npos)
				{
					CommonNameIncluded = true;
					break;
				}
			}
		}

		if (!CommonNameIncluded)
		{
			// 만약에 하나라도 포함한다면, 현재 Folder 경로와 Common Name 이 잘 짝지어진 것이다.
			if (m_vecAnimationSeqFilesFullPath[0].find(AllUpper) != std::string::npos)
				CommonNameIncluded = true;
		}

		if (CommonNameIncluded)
		{
			// 만약에 하나라도 포함한다면, 현재 Folder 경로와 Common Name 이 잘 짝지어진 것이다.
			if (m_vecAnimationSeqFilesFullPath[0].find(AllLower) != std::string::npos)
				CommonNameIncluded = true;
		}

		// 현재 Folder 내 특정 이름을 포함하는 File 들을 모아놨는데
		// 해당 File 들 중, 첫번째 파일이, Common Name 을 Include 하지 않는다는 의미는
		// 중간에 Common NAme 을 바꾸는 등의 변화가 있었다는 것이다.
		if (!CommonNameIncluded)
			return false;
	}

	return true;
}

void CAnimationEditor::GatherFullPathInfoOfSqcFilesInSelectedDir(const std::string& SelectedDirPath, const std::string& CommonSqcName)
{
	strcpy_s(m_SelectedSeqSrcsDirPath, SelectedDirPath.c_str());

	// 모든 .sqc 확장자 파일 이름들을 모은다. FullPath 들을 모아둘 것이다.
	std::vector<std::string> vecSeqFilesFullPath;

	// 먼저 해당 .sqc 확장자를 지닌, FullPath 목록들을 vector 형태로 모아둬야 한다.
	// CEditorUtil::GetAllFileNamesInDir(m_SelectedSeqSrcsDirPath, vecSeqFilesFullPath, ".sqc");
	CEditorUtil::GetAllFileFullPathInDir(m_SelectedSeqSrcsDirPath, vecSeqFilesFullPath, ".sqc");

	// vecAllAnimationSeqFilesFullPath 에 특정 문자를 포함하는 (대문자, 소문자 형태 모두) .sqc 파일들을 모두 모아놓을 것이다.
	std::vector<std::string> vecAllAnimationSeqFilesFullPath;
	vecAllAnimationSeqFilesFullPath.reserve(100);

	m_vecAnimationSeqFilesFullPath.clear();
	m_vecAnimationSeqFilesFullPath.reserve(100);

	// 1. Origin 입력 내용 그대로의 파일 이름을 포함하는 파일 들을 모은다
	// ex) "zed" => 모든 "zed~~" 형태의 파일 이름들 추출

	// CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, m_CommonAnimSeqName->GetTextUTF8());
	CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, CommonSqcName.c_str());

	// 모든 .sqc 파일들을 m_vecAnimationSeqFilesFullPath 에 모아두기 
	size_t AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

	for (size_t i = 0; i < AllSeqFilesCnt; ++i)
	{
		// 중복 제거
		if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
			continue;

		m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
	}

	// 2. 각각 소문자, 대문자
	//    모두 소문자, 모두 대문자
	std::vector<std::string> vecEachUpperSeqFilesFullPath;
	std::vector<std::string> vecEachLowerSeqFilesFullPath;
	std::string AllUpper;
	std::string AllLower;

	// CEditorUtil::GetAllKindsOfTransformedStringVersions(m_CommonAnimSeqName->GetTextUTF8(), vecEachLowerSeqFilesFullPath,
	// 	vecEachUpperSeqFilesFullPath, AllUpper, AllLower);	
	CEditorUtil::GetAllKindsOfTransformedStringVersions(CommonSqcName.c_str(), vecEachLowerSeqFilesFullPath,
		vecEachUpperSeqFilesFullPath, AllUpper, AllLower);

	// 각각 대문자 목록들 모아두기
	size_t EachUpperTotSize = vecEachUpperSeqFilesFullPath.size();

	for (size_t i = 0; i < EachUpperTotSize; ++i)
	{
		// FilterSpecificNameIncludedFilePath 에서는 vecAllAnimationSeqFilesFullPath 을 비워준다.
		CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, vecEachUpperSeqFilesFullPath[i].c_str());

		AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

		for (size_t i = 0; i < AllSeqFilesCnt; ++i)
		{
			if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
				continue;

			m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
		}
	}

	// 각각 소문자 목록들 모아두기
	size_t EachLowerTotSize = vecEachLowerSeqFilesFullPath.size();

	for (size_t i = 0; i < EachLowerTotSize; ++i)
	{
		// FilterSpecificNameIncludedFilePath 에서는 vecAllAnimationSeqFilesFullPath 을 비워준다.
		CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, vecEachLowerSeqFilesFullPath[i].c_str());

		AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

		for (size_t i = 0; i < AllSeqFilesCnt; ++i)
		{
			if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
				continue;

			m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
		}
	}


	// All 대문자 목록들 모아두기
	CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, AllUpper.c_str());

	AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

	for (size_t i = 0; i < AllSeqFilesCnt; ++i)
	{
		if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
			continue;

		m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
	}

	// All 소문자 목록들 모아두기
	CEditorUtil::FilterSpecificNameIncludedFilePath(vecSeqFilesFullPath, vecAllAnimationSeqFilesFullPath, AllLower.c_str());

	AllSeqFilesCnt = vecAllAnimationSeqFilesFullPath.size();

	for (size_t i = 0; i < AllSeqFilesCnt; ++i)
	{
		if (std::find(m_vecAnimationSeqFilesFullPath.begin(), m_vecAnimationSeqFilesFullPath.end(), vecAllAnimationSeqFilesFullPath[i]) != m_vecAnimationSeqFilesFullPath.end())
			continue;

		m_vecAnimationSeqFilesFullPath.push_back(std::move(vecAllAnimationSeqFilesFullPath[i]));
	}


	// 하나도 찾아내지 못했다면.
	if (m_vecAnimationSeqFilesFullPath.size() == 0)
	{
		MessageBox(nullptr, TEXT("No .sqc Files Found"), TEXT(".sqc File 찾기 실패"), MB_OK);
		return;
	}

	m_AnimSeqcSrcFolderPath->SetText(m_SelectedSeqSrcsDirPath);
}
