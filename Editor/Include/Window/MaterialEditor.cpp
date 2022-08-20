// IMGUI
#include "IMGUIButton.h"
#include "IMGUIComboBox.h"
#include "IMGUIImage.h"
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
#include "IMGUIInputFloat.h"
#include "IMGUIInputInt.h"
#include "IMGUIColor3.h"
#include "IMGUITextInput.h"
#include "IMGUISliderFloat.h"
#include "IMGUISliderInt.h"
#include "IMGUITableElemList.h"
// Window
#include "../EditorUtil.h"
#include "MaterialEditor.h"
#include "../EditorManager.h"
#include "ResourceDisplayWindow.h"
// Engine
#include "Resource/Material/Material.h"
#include "Resource/ResourceManager.h"
#include "PathManager.h"
#include "Render/RenderManager.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "Render/RenderState.h"

CMaterialEditor::CMaterialEditor()
{
}

CMaterialEditor::~CMaterialEditor()
{
}

bool CMaterialEditor::Init()
{
	// class CIMGUIText* m_SelectedMaterialName;  // 현재 선택된 Material 이름
	// class CIMGUITextInput* m_NewMaterialName; // 동시에 파일 이름으로 저장될 것이다.
	
	// New Material
	m_NewMaterialName = AddWidget<CIMGUITextInput>("New Mtrl Name", 150.f, 20.f);
	m_NewMaterialName->SetHideName(true);
	m_NewMaterialName->SetHintText("New Mtrl Name");

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	m_CreateMaterialBtn = AddWidget<CIMGUIButton>("Create", 90.f, 20.f);
	m_CreateMaterialBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnCreateMaterialCallback);

	// 현재 선택된 Material 정보
	CIMGUIDummy* Dummy = AddWidget<CIMGUIDummy>("Dummy", 150.f, 20.f);

	m_SelectedMaterialName = AddWidget<CIMGUITextInput>("Current Mtrl Name", 150.f, 20.f);
	m_SelectedMaterialName->SetDropCallBack(this, &CMaterialEditor::OnDropAndCreateMaterialCallback);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(290.f);

	CIMGUIText* HelpText = AddWidget<CIMGUIText>("SelectMaterialHelpText", 90.f, 30.f);
	const char* SelectMaterialHelpText = R"(ex)  현재 선택된 Material 이름을 보여준다. 
ResourceWindow로부터 Drag,Drop 을 통해서, Material 을  볼수 있다.)";
	HelpText->SetText(SelectMaterialHelpText);
	HelpText->SetIsHelpMode(true);

	// Material 이름 Edit 기능
	m_EditMaterialName = AddWidget<CIMGUITextInput>("Edit Mtrl Name", 150.f, 20.f);
	m_EditMaterialName->SetHideName(true);
	m_EditMaterialName->SetHintText("Edit Mtrl Name");

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	m_EditMaterialNameBtn = AddWidget<CIMGUIButton>("Edit", 90.f, 20.f);
	m_EditMaterialNameBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnEditMaterialNameCallback);

	// Image
	m_SetTexureImage = AddWidget<CIMGUIImage>("Texture Image", 150.f, 150.f);
	m_SetTexureImage->SetBorderColor(10, 10, 255);
	m_SetTexureImage->SetTableTitle("Texture Image");

	m_TextureInfoTable = AddWidget<CIMGUITableElemList>("Texture Info", 500.f, 150.f);
	m_TextureInfoTable->SetTableTitle("Texture Info");

	// Render State Info
	m_RenderStateInfoTable = AddWidget<CIMGUITable>("Render State Info", 600.f, 200.f);
	m_RenderStateInfoTable->SetTableTitle("RenderState Info");

	// Render State Input
	m_RenderStateSetInput = AddWidget<CIMGUITextInput>("Drop RenderState Name", 150.f, 20.f);
	m_RenderStateSetInput->SetHintText("Render State");
	m_RenderStateSetInput->SetDropCallBack(this, &CMaterialEditor::OnDropAndSetRenderStateToMaterial);

	m_ShaderSetInput = AddWidget<CIMGUITextInput>("Shader Name", 150.f, 20.f);
	m_ShaderSetInput->SetHintText("Current Shader");
	m_ShaderSetInput->SetDropCallBack(this, &CMaterialEditor::OnDropAndSetShaderToMaterial);

	m_SetParticleSettingBtn = AddWidget<CIMGUIButton>("Particle Setting", 150.f, 20.f);
	m_SetParticleSettingBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnSetParticleMaterialSettingCallback);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	HelpText = AddWidget<CIMGUIText>("SetParticleMaterial", 90.f, 30.f);
	const char* SetParticleMaterialText = R"(ex) Particle Material 로 사용하기 위한 기본 세팅(Shader, RenderState)을 해주는 버튼.)";
	HelpText->SetText(SetParticleMaterialText);
	HelpText->SetIsHelpMode(true);

	// m_MtrlInfoTable = AddWidget<CIMGUITable>("Mtrl Info", 200.f, 150.f);
	// m_MtrlInfoTable->SetTableTitle("Mtrl Info");

	Dummy = AddWidget<CIMGUIDummy>("Dummy", 150.f, 20.f);

	// Add Texture
	Dummy = AddWidget<CIMGUIDummy>("Dummy", 150.f, 20.f);

	// Set Texture
	m_TextureRegister = AddWidget<CIMGUIInputInt>("Register", 150.f, 20.f);
	m_TextureIndex = AddWidget<CIMGUIInputInt>("Index", 150.f, 20.f);

	m_SetTextureInput = AddWidget<CIMGUITextInput>("Set Texture", 150.f, 20.f);
	m_SetTextureInput->SetHideName(true);
	m_SetTextureInput->SetHintText("Key Name");
	m_SetTextureInput->SetDropCallBack<CMaterialEditor>(this, &CMaterialEditor::OnSetTextureBtnWithString);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	m_SetTextureBtn = AddWidget<CIMGUIButton>("Load & Set Texture", 120.f, 20.f);
	m_SetTextureBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnSetTextureBtn);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(290.f);

	HelpText = AddWidget<CIMGUIText>("SetTextureHelpText", 90.f, 30.f);
	const char* SetTextureHelpTexte = R"(ex) 1) 몇번째 Index 의 Texture 를 세팅할지 정한다.
2) 그 다음 ResourceWindow 로부터 Key값을 Drag Drop하여 찾거나
3) FileBrowser 에서 File을 Drag Drop해서 세팅한다.
4) 혹은, 해당 버튼을 통해 하드디스크로부터 Set 할 Texture 를 선택해서 Load 한다.)";
	HelpText->SetText(SetTextureHelpTexte);
	HelpText->SetIsHelpMode(true);

	// Add Texture
	m_AddTextureInput = AddWidget<CIMGUITextInput>("Key Name", 150.f, 20.f);
	m_AddTextureInput->SetHideName(true);
	m_AddTextureInput->SetHintText("Key Name");
	m_AddTextureInput->SetDropCallBack<CMaterialEditor>(this, &CMaterialEditor::OnAddTextureBtnWithString);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	m_AddTextureBtn = AddWidget<CIMGUIButton>("Load & Add Texture", 120.f, 20.f);
	m_AddTextureBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnAddTextureBtn);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(290.f);

	HelpText = AddWidget<CIMGUIText>("AddTextureHelpText", 90.f, 30.f);
	const char* AddTextureHelpTexte = R"(ex)  
1) ResourceWindow 로부터 Key값을 Drag Drop하여 찾거나
2) FileBrowser 에서 File을 Drag Drop 해서 세팅한다.
3) 혹은, 해당 버튼을 통해 하드디스크로부터 Add 할 Texture 를 선택하여 Load 한다.)";
	HelpText->SetText(AddTextureHelpTexte);
	HelpText->SetIsHelpMode(true);

	// Load & Save Btn
	Dummy = AddWidget<CIMGUIDummy>("Dummy", 150.f, 20.f);

	HelpText = AddWidget<CIMGUIText>("SaveMtrl", 90.f, 30.f);
	const char* SaveMtrlText = R"(ex) 저장하는 파일 이름은, Material 의 이름과 동일해야 한다.)";
	HelpText->SetText(SaveMtrlText);
	HelpText->SetIsHelpMode(true);

	m_SaveMaterialBtn = AddWidget<CIMGUIButton>("Save Mtrl", 90.f, 20.f);
	m_SaveMaterialBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnSaveMaterial);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(100.f);

	m_LoadMaterialBtn = AddWidget<CIMGUIButton>("Load Mtrl", 90.f, 20.f);
	m_LoadMaterialBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnLoadMaterial);


	// class CIMGUIButton* m_CreateMaterialBtn;
	// class CIMGUIButton* m_SaveMaterialBtn;
	// class CIMGUIButton* m_LoadMaterialBtn;
	// 
	// class CIMGUIListBox* m_MadeMaterialList;
	// 
	// // Shader 를 Drag, Drop 할 수 있는 장소
	// 
	// // Texture 목록을 보여주는 장소
	// class CIMGUITableElemList* m_TextureInfoTable;
	// 
	// // Drop 해서 Set Texture 하기
	// 
	// // Drop 해서 Add Texture 하기 
	// 
	// // Drop 해서 Image 보여주는 Image Widget
	// 
	// 
	// // 남욱씨가 말한 내용들 Setting 하는 장소
	// class CIMGUICheckBox* m_OutLineCheck;
	// class CIMGUIColor4* m_OutLineColor;
	// class CIMGUIInputFloat* m_OutLineThickNess;


	return true;
}

void CMaterialEditor::OnCreateMaterialCallback()
{
	if (m_NewMaterialName->Empty())
		return;

	CMaterial* NewMaterial = CResourceManager::GetInst()->CreateMaterial<CMaterial>(m_NewMaterialName->GetTextUTF8());

	// 이미 선택된 Material 과 같다면 X
	if (m_SelectedMaterial && m_SelectedMaterial == NewMaterial)
	{
		return;
	}

	m_NewMaterialName->ClearText();

	m_SelectedMaterial = NewMaterial;

	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	// 보여지는 Texture 정보 갱신
	m_SetTexureImage->SetTexture(nullptr);

	// Resource Info Window 정보 갱신하기
	CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
}

void CMaterialEditor::OnDropAndCreateMaterialCallback(const std::string& MaterialName)
{
	// 중복 방지
	if (m_SelectedMaterial && m_SelectedMaterial->GetName() == MaterialName)
		return;

	// 들어온 Material Name

	// 1) Key 값으로 이용해서 Material Manager 에서 세팅
	CMaterial* FoundMaterial = CResourceManager::GetInst()->FindMaterial(MaterialName);

	if (FoundMaterial)
	{
		m_SelectedMaterial = FoundMaterial;

		RefreshMaterialDisplayInfo(m_SelectedMaterial);

		return;
	}

	// 2) 없으면, 해당 하드디스크 파일 이름을 이용해서 Bin//Material//ParticleMaterial 폴더에 있는 File 을 찾아서
	// 해당 Material 파일을 Material Manager 에 Load 하고
	// m_SeleteMaterial 에 세팅한다.
	char MaterialLoadFullPathMultibyte[MAX_PATH] = {};
	// TCHAR MaterialLoadFullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

	//  Bin//Material//ParticleMaterial 해당 폴더가 없다면 만들어준다.
	CEngineUtil::CheckAndMakeDirectory(Info);

	if (Info)
		strcpy_s(MaterialLoadFullPathMultibyte, Info->PathMultibyte);

	strcat_s(MaterialLoadFullPathMultibyte, MaterialName.c_str());

	CMaterial* LoadedMaterial = CResourceManager::GetInst()->LoadMaterialFullPathMultibyte(MaterialLoadFullPathMultibyte);

	// 이것이 nullptr 이라는 의미는, Load 가 실패했다는 의미이다.
	if (!LoadedMaterial)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Load Failure From Bin/Material/ParticleMaterial."), NULL, MB_OK);

		m_SelectedMaterialName->ResetText();

		return;
	}

	m_SelectedMaterial = LoadedMaterial;

	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();

	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Set Successfully"), TEXT("Success"), NULL);
}

void CMaterialEditor::OnDropAndSetShaderToMaterial(const std::string& DropShaderName)
{
	// 현재 Material 에 Shader Name 을 세팅한다.
	// Shader Manager 에서 Key 값만 찾아서 세팅해주고
	// 없으면 X
	if (!m_SelectedMaterial)
		return;

	CGraphicShader* FoundShader = dynamic_cast<CGraphicShader*>(CResourceManager::GetInst()->FindShader(DropShaderName));
	
	if (!FoundShader)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("No Shader Exist To According Shader Key"), NULL, MB_OK);
		m_ShaderSetInput->ResetText();
		return;
	}

	m_SelectedMaterial->SetShader(FoundShader);

	// Shader Setting 이후, Refresh
	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Shader Successfully Set"), TEXT("Success"), NULL);
}

void CMaterialEditor::OnDropAndSetRenderStateToMaterial(const std::string& DropRenderStateName)
{
	const char* PrevRenderStateName = m_RenderStateSetInput->GetTextUTF8();

	// 현재 Material 에 Render State Name 을 세팅한다.
	// Shader Manager 에서 Key 값만 찾아서 세팅해주고
	// 없으면 X
	if (!m_SelectedMaterial)
		return;

	CRenderState* FoundRenderState = CRenderManager::GetInst()->FindRenderState(DropRenderStateName);

	if (!FoundRenderState)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("No RenderState Exist To According Sampler Key"), NULL, MB_OK);
		m_RenderStateSetInput->SetText(PrevRenderStateName);
		return;
	}

	// Render State Setting 이후 Refresh
	m_SelectedMaterial->SetRenderState(FoundRenderState);

	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("RenderState Successfully Set"), TEXT("Success"), NULL);
}

void CMaterialEditor::OnEditMaterialNameCallback()
{
	if (!m_SelectedMaterial || m_EditMaterialName->Empty())
		return;

	// 이름 수정
	m_SelectedMaterial->SetName(m_EditMaterialName->GetTextUTF8());

	// IMGUI Update
	m_SelectedMaterialName->SetText(m_EditMaterialName->GetTextUTF8());

	m_EditMaterialName->ClearText();
}

void CMaterialEditor::OnSetParticleMaterialSettingCallback()
{
	if (!m_SelectedMaterial)
		return;

	// Alpha Blend State 세팅
	CRenderState* AlphaBlendState = CRenderManager::GetInst()->FindRenderState("AlphaBlend");
	if (!AlphaBlendState)
	{
		assert(false);
		return;
	}
	m_SelectedMaterial->SetRenderState(AlphaBlendState);

	// No Cull Mode 세팅
	CRenderState* NoCullModeState = CRenderManager::GetInst()->FindRenderState("NoCull");

	if (!NoCullModeState)
	{
		assert(false);
		return;
	}
	m_SelectedMaterial->SetRenderState(NoCullModeState);

	// Particle Render Shader 세팅
	CGraphicShader* FoundShader = dynamic_cast<CGraphicShader*>(CResourceManager::GetInst()->FindShader("ParticleRenderShader"));

	if (!FoundShader)
	{
		assert(false);
		return;
	}
	m_SelectedMaterial->SetShader(FoundShader);

	// IMGUI Update
	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Material Setting Set"), NULL, MB_OK);
}

void CMaterialEditor::OnSetTextureBtn()
{
	if (!m_SelectedMaterial)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("No Material Set"), NULL, MB_OK);
		return;
	}

	// OnSetTextureBtnWithString(m_SetTextureInput->GetTextUTF8());

	// 실제 Texture 를 Load 하여 Setting 하는 기능 마련하기 
	TCHAR FileFullPath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	// OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Material File\0*.mtrl");
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(TEXTURE_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		int SetTextureIndex = m_TextureIndex->GetVal();

		if (SetTextureIndex >= m_SelectedMaterial->GetTextureInfo().size())
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Target Texture Index Exceed Texture Size"), NULL, MB_OK);
			return;
		}

		// FullPath 추출
		char FileFullPathMultibyteCopy[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FileFullPathMultibyteCopy, ConvertLength, 0, 0);

		// 파일 이름 추출
		std::string FileName;
		std::string FileExt;
		CEditorUtil::ExtractFileNameAndExtFromPath(FileFullPathMultibyteCopy, FileName, FileExt);

		// 파일 이름을 Key값으로 하여, 이미 Engine에 Load 되어 있는지 확인한다.
		CTexture* TargetTexture = CResourceManager::GetInst()->FindTexture(FileName);

		// Texture 와 Register 정보를 둘다 세팅한다.
		if (TargetTexture)
		{
			m_SelectedMaterial->SetTextureInfoResource(SetTextureIndex, TargetTexture);
			m_SelectedMaterial->SetTextureInfoRegister(SetTextureIndex, m_TextureRegister->GetVal());

			TextureSetSuccessCallback(TargetTexture);

			return;
		}

		// 그게 아니라면, Resource Manager 를 통해 Load 하고 Texture 를 세팅한다.
		CResourceManager::GetInst()->LoadTextureFullPathMultibyte(FileName, FileFullPathMultibyteCopy);

		TargetTexture = CResourceManager::GetInst()->FindTexture(FileName);
		
		m_SelectedMaterial->SetTextureInfoResource(SetTextureIndex, TargetTexture);
		m_SelectedMaterial->SetTextureInfoRegister(SetTextureIndex, m_TextureRegister->GetVal());

		TextureSetSuccessCallback(TargetTexture);

		return;
	}
}

void CMaterialEditor::OnSetTextureBtnWithString(const std::string& InputName)
{
	if (!m_SelectedMaterial)
		return;

	int SetTextureIndex = m_TextureIndex->GetVal();

	if (SetTextureIndex >= m_SelectedMaterial->GetTextureInfo().size())
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Target Texture Index Exceed Texture Size"), NULL, MB_OK);
		return;
	}

	// 1) 먼저 Texture Manager 에서 해당 Key로 저장된 Texture 정보가 있는지를 저장하고
	CTexture* TargetTexture = CResourceManager::GetInst()->FindTexture(InputName);

	if (TargetTexture)
	{
		m_SelectedMaterial->SetTextureInfoResource(SetTextureIndex, TargetTexture);

		TextureSetSuccessCallback(TargetTexture);
		return;
	}

	// 2) 그래도 없으면, Hard Disk 에서 불러와서 세팅한다.
	// Drop 한 Input 안에 있는 Texture 정보 불러오기 
	// Texture File 의 File 명을, Texture 를 저장하는 Key 값으로 사용할 것이다.
	std::string TextureKeyName;
	std::optional<std::string> FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(TEXTURE_PATH,
		InputName, TextureKeyName);

	// 찾지 못했다면 
	if (!FoundResult.has_value())
	{
		// Texture Path가 아니라, Particle Path 에서도 한번 찾아본다.
		FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(PARTICLE_PATH,
			InputName, TextureKeyName);

		if (!FoundResult.has_value())
			return;
	}
	// 
	// Editor Util 함수 안에, --> 특정 폴더, File 안에 이름 넣어주면, 다 돌면서, 파일 찾아주는 함수 제작
	// 그렇게 해서 Full Path, File 이름 받아오고
	// 이를 통해 ResourceManager, Texture Manager 에서 불러오기
	// 그 안에서는 있으면 그대로 리턴, 없으면 Load 로직 세팅

	bool LoadedResult = CResourceManager::GetInst()->LoadTextureFullPathMultibyte(TextureKeyName, FoundResult.value().c_str());

	if (!LoadedResult)
	{
		// New Texture Load Failure Message Box
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Texture Load Failure"), NULL, MB_OK);
		return;
	}

	TargetTexture = CResourceManager::GetInst()->FindTexture(TextureKeyName);

	m_SelectedMaterial->SetTextureInfoResource(SetTextureIndex, TargetTexture);

	TextureSetSuccessCallback(TargetTexture);
}

void CMaterialEditor::OnAddTextureBtn()
{
	if (!m_SelectedMaterial)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("No Material Set"), NULL, MB_OK);
		return;
	}

	// OnSetTextureBtnWithString(m_SetTextureInput->GetTextUTF8());

	// 실제 Texture 를 Load 하여 Setting 하는 기능 마련하기 
	TCHAR FileFullPath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	// OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Material File\0*.mtrl");
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(TEXTURE_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		// FullPath 추출
		//const char* FileFullPathMultibyte = CEditorUtil::ChangeTCHARTextToMultibyte(FileFullPath);

		char FilePathMultibyte[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);


		char FileFullPathMultibyteCopy[MAX_PATH] = {};
		strcpy_s(FileFullPathMultibyteCopy, FilePathMultibyte);

		// 파일 이름 추출
		std::string FileName;
		std::string FileExt;
		CEditorUtil::ExtractFileNameAndExtFromPath(FileFullPathMultibyteCopy, FileName, FileExt);

		// 1) 파일 이름을 Key값으로 하여, 이미 Engine에 Load 되어 있는지 확인한다.
		CTexture* TargetTexture = CResourceManager::GetInst()->FindTexture(FileName);

		if (TargetTexture)
		{
			int Reg = m_TextureRegister->GetVal();
			if (m_TextureRegister->GetVal() < 0)
			{
				Reg = 0;
			}

			m_SelectedMaterial->AddTexture(Reg, (int)Buffer_Shader_Type::Pixel, FileName, TargetTexture);

			TextureSetSuccessCallback(TargetTexture);

			return;
		}

		// 2) 그게 아니라면, Resource Manager 를 통해 Load 하고 Texture 를 세팅한다.
		bool LoadedResult = CResourceManager::GetInst()->LoadTextureFullPathMultibyte(FileName, FileFullPathMultibyteCopy);

		if (!LoadedResult)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Texture Load Failure"), NULL, MB_OK);
			return;
		}

		TargetTexture = CResourceManager::GetInst()->FindTexture(FileName);

		int Reg = m_TextureRegister->GetVal();
		if (m_TextureRegister->GetVal() < 0)
		{
			Reg = 0;
		}
		m_SelectedMaterial->AddTexture(Reg, (int)Buffer_Shader_Type::Pixel, FileName, TargetTexture);

		TextureSetSuccessCallback(TargetTexture);

		return;
	}
}

void CMaterialEditor::OnAddTextureBtnWithString(const std::string& InputName)
{
	if (!m_SelectedMaterial)
		return;

	// 1) 먼저 Texture Manager 에서 해당 Key로 저장된 Texture 정보가 있는지를 저장하고
	CTexture* TargetTexture = CResourceManager::GetInst()->FindTexture(InputName);

	if (TargetTexture)
	{
		int Reg = m_TextureRegister->GetVal();
		if (m_TextureRegister->GetVal() < 0)
		{
			Reg = 0;
		}
		m_SelectedMaterial->AddTexture(Reg, (int)Buffer_Shader_Type::Pixel, InputName, TargetTexture);

		TextureSetSuccessCallback(TargetTexture);

		return;
	}
	
	// 2) 만약 없다면, 그 다음, 하드디스크로 내려가서 해당 정보를 불러올 것이다.

	// m_SelectedMaterial->SetOutLineThickNess(ThickNess);
	// Drop 한 Input 안에 있는 Texture 정보 불러오기 
	// Texture File 의 File 명을, Texture 를 저장하는 Key 값으로 사용할 것이다.
	std::string TextureKeyName;

	std::optional<std::string> FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(TEXTURE_PATH,
		InputName, TextureKeyName);

	// 찾지 못했다면 
	if (!FoundResult.has_value())
		return;
	// 
	// Editor Util 함수 안에, --> 특정 폴더, File 안에 이름 넣어주면, 다 돌면서, 파일 찾아주는 함수 제작
	// 그렇게 해서 Full Path, File 이름 받아오고
	// 이를 통해 ResourceManager, Texture Manager 에서 불러오기
	// 그 안에서는 있으면 그대로 리턴, 없으면 Load 로직 세팅

	bool LoadedResult = CResourceManager::GetInst()->LoadTextureFullPathMultibyte(TextureKeyName, FoundResult.value().c_str());

	if (!LoadedResult)
	{
		// New Texture Load Failure Message Box
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Texture Load Failure"), NULL, MB_OK);
		return;
	}

	TargetTexture = CResourceManager::GetInst()->FindTexture(TextureKeyName);

	int Reg = m_TextureRegister->GetVal();
	if (m_TextureRegister->GetVal() < 0)
	{
		Reg = 0;
	}
	m_SelectedMaterial->AddTexture(Reg, (int)Buffer_Shader_Type::Pixel, InputName, TargetTexture);

	TextureSetSuccessCallback(TargetTexture);
}

void CMaterialEditor::TextureSetSuccessCallback(class CTexture* Texture)
{
	// 현재 Texture 를 IMGUI Image 에 보여주기
	m_SetTexureImage->SetTexture(Texture);
	
	RefreshMaterialDisplayInfo(m_SelectedMaterial, Texture);
	
	// ResourceDisplay Window 에 있는 Texture 목록들 Resource Window 에 추가해서 보여주기
	CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
	
	// 제대로 세팅되었다는 Message
	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Texture Set SuccessFully"), TEXT("Success"), NULL);
}

void CMaterialEditor::OnSaveMaterial()
{
	if (!m_SelectedMaterial)
		return;

	const PathInfo* MaterialPathInfo = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

	// Bin//Material//ParticleMaterial 폴더가 있는지 확인하고 만들어준다.
	CEngineUtil::CheckAndMakeDirectory(MaterialPathInfo);

	TCHAR FileFullPath[MAX_PATH] = {};

	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All Files\0*.*\0.Material File\0*.mtrl");
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = MaterialPathInfo->Path;

	if (GetSaveFileName(&OpenFile) != 0)
	{
		char FileName[MAX_PATH];
		char FileExt[MAX_PATH];

		// Initial Name => Material 의 Name 으로 설정
		TCHAR TCHARInitFilename[MAX_PATH] = {};

		char FilePathMultibyte[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		CEditorUtil::ExtractFileNameAndExtFromPath(FilePathMultibyte, FileName, FileExt);

		// 현재 저장하는 Material 의 파일 이름과, Material 의 이름이 같은지를 확인한다.
		if (strcmp(FileName, m_SelectedMaterial->GetName().c_str()) != 0)
		{
			TCHAR ErrorMessage[MAX_PATH] = {};
			lstrcpy(ErrorMessage, TEXT("FileName Has To Be Same With Material Name : "));
			lstrcat(ErrorMessage, TCHARInitFilename);
			MessageBox(CEngine::GetInst()->GetWindowHandle(), ErrorMessage, NULL, MB_OK);
			return;
		}

		// (아래의 코드는 기존 Particle Material 용도로만 사용한다는 가정하에 작성, 이제는 다른 Material 범용으로 사용하므로 아래코드 지우기)
		// 현재 저장하는 Directory가 Bin/Material/ParticleMaterial 인지 확인하기 => 아니라면, Save 방지
		std::string PathInfoBeforeFileName;
		CEngineUtil::GetPathInfoBeforeFileName(FilePathMultibyte, PathInfoBeforeFileName);

		// if (strcmp(MaterialPathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
		// {
		// 	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Material 의 경우, 반드시 Bin/Material/ParticleMaterial 에 저장"), NULL, MB_OK);
		// 	return;
		// }

		// 현재 저장하는 Path 에서, 같은 이름의 FileName이 존재하는지 확인하기
		char CheckMaterialFileName[MAX_PATH] = {};
		strcpy_s(CheckMaterialFileName, FileName);
		strcat_s(CheckMaterialFileName, FileExt);

		if (CEditorUtil::IsFileExistInDir(MATERIAL_PARTICLE_PATH, CheckMaterialFileName))
		{
			if (MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("같은 이름의 .mtrl 파일이 존재합니다. 저장하시겠습니까?"), NULL, MB_YESNO) != IDYES)
				return;
		}


		// 확장자 검사하기
		_strupr_s(FileExt);
		if (strcmp(FileExt, ".MTRL") != 0)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .mtrl"), NULL, MB_OK);
			return;
		}

		m_SelectedMaterial->SaveFullPath(FilePathMultibyte);

		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Save Success"), TEXT("Success"), NULL);
	}
}

void CMaterialEditor::OnLoadMaterial()
{
	const PathInfo* MaterialPathInfo = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

	// Bin//Material//ParticleMaterial 폴더가 있는지 확인하고 만들어준다.
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

		// 확장자 .anim 이 아니라면 return;
		if (strcmp(Ext, ".MTRL") != 0)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .mtrl"), NULL, MB_OK);
			return;
		}

		// >> (원래는 반드시 Particle 용 Material 을 사용하려고 했었다. 하지만, 이제 다른 Component 의 Material Editor 로도 사용해야 하므로)
		// >> (아래의 코드는 없앤다.)
		// 현재 Load하는 Directory가 Bin/Material/ParticleMaterial 인지 확인하기 => 아니라면, Load X
		// std::string PathInfoBeforeFileName;
		// CEngineUtil::GetPathInfoBeforeFileName(FilePathMultibyte, PathInfoBeforeFileName);
		// 
		// if (strcmp(MaterialPathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
		// {
		// 	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Material 의 경우, 반드시 Bin/Material/ParticleMaterial 로부터 Load 해야 한다."), NULL, MB_OK);
		// 	return;
		// }

		// 파일 이름을, Material 을 저장하는 Key 값으로 활용할 것이다.
		CMaterial* LoadedMaterial = CResourceManager::GetInst()->LoadMaterialFullPathMultibyte(FilePathMultibyte);

		if (!LoadedMaterial)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Load Failure"), NULL, MB_OK);
			return;
		}

		m_SelectedMaterial = LoadedMaterial;

		RefreshMaterialDisplayInfo(m_SelectedMaterial);

		// ResourceDisplay Window 에 있는 Texture 목록들 Resource Window 에 추가해서 보여주기
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedRenderStateResources();
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedShaderResources();

		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Success"), TEXT("Material Load Success"), NULL);
	}
}

void CMaterialEditor::RefreshMaterialDisplayInfo(class CMaterial* Material, class CTexture* Texture )
{
	// Material Editor =>
	if (!Material)
		return;

	// 현재 Material 이름 보여주기
	m_SelectedMaterialName->SetText(Material->GetName().c_str());

	// Edit Name Input 비워주기 
	m_EditMaterialName->ClearText();

	const std::vector<MaterialTextureInfo>& MtrlTexture = Material->GetTextureInfo();

	// 따로 세팅해주고자 하는 Texture 가 없고, 현재 Material 이 Texture 를 들고 있다면
	if (!Texture && MtrlTexture.size() > 0)
	{
		int LastTexIdx = (int)MtrlTexture.size() - 1;
		// Texture Image 맨 마지막 꺼 보여주기
		m_SetTexureImage->SetTexture(MtrlTexture[LastTexIdx].Texture);
	}
	else
	{
		m_SetTexureImage->SetTexture(Texture);
	}

	// Texture Table 에 Texture 정보들 세팅
	// - Name, Sampler Type, Texture Name, FileName + EXT, Register 정보
	m_TextureInfoTable->ClearContents();

	size_t TextureNums = MtrlTexture.size();

	for (size_t i = 0; i < TextureNums; ++i)
	{
		const MaterialTextureInfo& TextureInfo = MtrlTexture[i];

		m_TextureInfoTable->AddData(MaterialTextureInfoKeys::Index, (int)i);
		m_TextureInfoTable->AddData(MaterialTextureInfoKeys::Register, TextureInfo.Register);
		m_TextureInfoTable->AddData(MaterialTextureInfoKeys::Name, TextureInfo.Name);

		const std::vector<TextureResourceInfo*>& VecTextureInfo = TextureInfo.Texture->GetVecTextureInfo();

		size_t VecTextSize = VecTextureInfo.size();

		for (size_t tIdx = 0; tIdx < VecTextSize; ++tIdx)
		{
			char FileNameMultibyte[MAX_PATH] = {};

			int ConvertLength = WideCharToMultiByte(CP_ACP, 0, VecTextureInfo[tIdx]->FileName, -1, nullptr, 0, nullptr, nullptr);
			WideCharToMultiByte(CP_ACP, 0, VecTextureInfo[tIdx]->FileName, -1, FileNameMultibyte, ConvertLength, nullptr, nullptr);

			std::string FileNameOnly;

			CEngineUtil::GetFileNameAfterSlash(FileNameMultibyte, FileNameOnly);

			m_TextureInfoTable->AddData(MaterialTextureInfoKeys::Textures[tIdx], FileNameOnly);
		}
	}

	// Set Texture, Add Texture Input 내용 비워주기
	m_AddTextureInput->ClearText();
	m_SetTextureInput->ClearText();

	// Info Table 세팅
	// Shader Name
	CShader* Shader = Material->GetShader();

	m_ShaderSetInput->ClearText();

	if (Shader)
	{
		m_ShaderSetInput->SetText(Shader->GetName().c_str());
	}
	 
	// TODO : Material 의 고유 정보 세팅 (현재로서는 필요한 것은 없어보인다)
	// BaseColor, Ambient Color, Specular Color, EmmisiveColor
	// m_MtrlInfoTable->Clear()
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameTRange, Sequence->GetStartFrame());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameRange, Sequence->GetEndFrame());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameLength, Sequence->GetFrameLength());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameMode, Sequence->GetFrameMode());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::PlayTime, Sequence->GetPlayTime());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameTime, Sequence->GetFrameTime());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::PlayScale, Sequence->GetPlayScale());

	// Sampler 정보를 세팅한다. -> Table 정보로 세팅
	m_RenderStateInfoTable->ClearContents();

	const CSharedPtr<class CRenderState>* RenderStateArray = Material->GetRenderStateArray();

	for (int i = 0; i < (int)RenderState_Type::Max; ++i)
	{
		// if (RenderStateArray[i])
		// {
		std::string RenderStateName = "";

		if (RenderStateArray[i])
			RenderStateName = RenderStateArray[i]->GetName();

		const std::string& TableKeyName = MaterialTextureInfoKeys::RenderState[i];

		m_RenderStateInfoTable->AddData(TableKeyName, RenderStateName);
		// }
	}

	// RenderState Input 을 Clear 해준다.
	m_RenderStateSetInput->ClearText();

	// TODO : Metallic 정보 세팅
}
