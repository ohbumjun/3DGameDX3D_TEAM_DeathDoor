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
	// class CIMGUIText* m_SelectedMaterialName;  // ���� ���õ� Material �̸�
	// class CIMGUITextInput* m_NewMaterialName; // ���ÿ� ���� �̸����� ����� ���̴�.
	
	// New Material
	m_NewMaterialName = AddWidget<CIMGUITextInput>("New Mtrl Name", 150.f, 20.f);
	m_NewMaterialName->SetHideName(true);
	m_NewMaterialName->SetHintText("New Mtrl Name");

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(160.f);

	m_CreateMaterialBtn = AddWidget<CIMGUIButton>("Create", 90.f, 20.f);
	m_CreateMaterialBtn->SetClickCallback<CMaterialEditor>(this, &CMaterialEditor::OnCreateMaterialCallback);

	// ���� ���õ� Material ����
	CIMGUIDummy* Dummy = AddWidget<CIMGUIDummy>("Dummy", 150.f, 20.f);

	m_SelectedMaterialName = AddWidget<CIMGUITextInput>("Current Mtrl Name", 150.f, 20.f);
	m_SelectedMaterialName->SetDropCallBack(this, &CMaterialEditor::OnDropAndCreateMaterialCallback);

	Line = AddWidget<CIMGUISameLine>("Line");
	Line->SetOffsetX(290.f);

	CIMGUIText* HelpText = AddWidget<CIMGUIText>("SelectMaterialHelpText", 90.f, 30.f);
	const char* SelectMaterialHelpText = R"(ex)  ���� ���õ� Material �̸��� �����ش�. 
ResourceWindow�κ��� Drag,Drop �� ���ؼ�, Material ��  ���� �ִ�.)";
	HelpText->SetText(SelectMaterialHelpText);
	HelpText->SetIsHelpMode(true);

	// Material �̸� Edit ���
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
	const char* SetParticleMaterialText = R"(ex) Particle Material �� ����ϱ� ���� �⺻ ����(Shader, RenderState)�� ���ִ� ��ư.)";
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
	const char* SetTextureHelpTexte = R"(ex) 1) ���° Index �� Texture �� �������� ���Ѵ�.
2) �� ���� ResourceWindow �κ��� Key���� Drag Drop�Ͽ� ã�ų�
3) FileBrowser ���� File�� Drag Drop�ؼ� �����Ѵ�.
4) Ȥ��, �ش� ��ư�� ���� �ϵ��ũ�κ��� Set �� Texture �� �����ؼ� Load �Ѵ�.)";
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
1) ResourceWindow �κ��� Key���� Drag Drop�Ͽ� ã�ų�
2) FileBrowser ���� File�� Drag Drop �ؼ� �����Ѵ�.
3) Ȥ��, �ش� ��ư�� ���� �ϵ��ũ�κ��� Add �� Texture �� �����Ͽ� Load �Ѵ�.)";
	HelpText->SetText(AddTextureHelpTexte);
	HelpText->SetIsHelpMode(true);

	// Load & Save Btn
	Dummy = AddWidget<CIMGUIDummy>("Dummy", 150.f, 20.f);

	HelpText = AddWidget<CIMGUIText>("SaveMtrl", 90.f, 30.f);
	const char* SaveMtrlText = R"(ex) �����ϴ� ���� �̸���, Material �� �̸��� �����ؾ� �Ѵ�.)";
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
	// // Shader �� Drag, Drop �� �� �ִ� ���
	// 
	// // Texture ����� �����ִ� ���
	// class CIMGUITableElemList* m_TextureInfoTable;
	// 
	// // Drop �ؼ� Set Texture �ϱ�
	// 
	// // Drop �ؼ� Add Texture �ϱ� 
	// 
	// // Drop �ؼ� Image �����ִ� Image Widget
	// 
	// 
	// // ������ ���� ����� Setting �ϴ� ���
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

	// �̹� ���õ� Material �� ���ٸ� X
	if (m_SelectedMaterial && m_SelectedMaterial == NewMaterial)
	{
		return;
	}

	m_NewMaterialName->ClearText();

	m_SelectedMaterial = NewMaterial;

	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	// �������� Texture ���� ����
	m_SetTexureImage->SetTexture(nullptr);

	// Resource Info Window ���� �����ϱ�
	CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
}

void CMaterialEditor::OnDropAndCreateMaterialCallback(const std::string& MaterialName)
{
	// �ߺ� ����
	if (m_SelectedMaterial && m_SelectedMaterial->GetName() == MaterialName)
		return;

	// ���� Material Name

	// 1) Key ������ �̿��ؼ� Material Manager ���� ����
	CMaterial* FoundMaterial = CResourceManager::GetInst()->FindMaterial(MaterialName);

	if (FoundMaterial)
	{
		m_SelectedMaterial = FoundMaterial;

		RefreshMaterialDisplayInfo(m_SelectedMaterial);

		return;
	}

	// 2) ������, �ش� �ϵ��ũ ���� �̸��� �̿��ؼ� Bin//Material//ParticleMaterial ������ �ִ� File �� ã�Ƽ�
	// �ش� Material ������ Material Manager �� Load �ϰ�
	// m_SeleteMaterial �� �����Ѵ�.
	char MaterialLoadFullPathMultibyte[MAX_PATH] = {};
	// TCHAR MaterialLoadFullPath[MAX_PATH] = {};

	const PathInfo* Info = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

	//  Bin//Material//ParticleMaterial �ش� ������ ���ٸ� ������ش�.
	CEngineUtil::CheckAndMakeDirectory(Info);

	if (Info)
		strcpy_s(MaterialLoadFullPathMultibyte, Info->PathMultibyte);

	strcat_s(MaterialLoadFullPathMultibyte, MaterialName.c_str());

	CMaterial* LoadedMaterial = CResourceManager::GetInst()->LoadMaterialFullPathMultibyte(MaterialLoadFullPathMultibyte);

	// �̰��� nullptr �̶�� �ǹ̴�, Load �� �����ߴٴ� �ǹ��̴�.
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
	// ���� Material �� Shader Name �� �����Ѵ�.
	// Shader Manager ���� Key ���� ã�Ƽ� �������ְ�
	// ������ X
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

	// Shader Setting ����, Refresh
	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Shader Successfully Set"), TEXT("Success"), NULL);
}

void CMaterialEditor::OnDropAndSetRenderStateToMaterial(const std::string& DropRenderStateName)
{
	const char* PrevRenderStateName = m_RenderStateSetInput->GetTextUTF8();

	// ���� Material �� Render State Name �� �����Ѵ�.
	// Shader Manager ���� Key ���� ã�Ƽ� �������ְ�
	// ������ X
	if (!m_SelectedMaterial)
		return;

	CRenderState* FoundRenderState = CRenderManager::GetInst()->FindRenderState(DropRenderStateName);

	if (!FoundRenderState)
	{
		MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("No RenderState Exist To According Sampler Key"), NULL, MB_OK);
		m_RenderStateSetInput->SetText(PrevRenderStateName);
		return;
	}

	// Render State Setting ���� Refresh
	m_SelectedMaterial->SetRenderState(FoundRenderState);

	RefreshMaterialDisplayInfo(m_SelectedMaterial);

	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("RenderState Successfully Set"), TEXT("Success"), NULL);
}

void CMaterialEditor::OnEditMaterialNameCallback()
{
	if (!m_SelectedMaterial || m_EditMaterialName->Empty())
		return;

	// �̸� ����
	m_SelectedMaterial->SetName(m_EditMaterialName->GetTextUTF8());

	// IMGUI Update
	m_SelectedMaterialName->SetText(m_EditMaterialName->GetTextUTF8());

	m_EditMaterialName->ClearText();
}

void CMaterialEditor::OnSetParticleMaterialSettingCallback()
{
	if (!m_SelectedMaterial)
		return;

	// Alpha Blend State ����
	CRenderState* AlphaBlendState = CRenderManager::GetInst()->FindRenderState("AlphaBlend");
	if (!AlphaBlendState)
	{
		assert(false);
		return;
	}
	m_SelectedMaterial->SetRenderState(AlphaBlendState);

	// No Cull Mode ����
	CRenderState* NoCullModeState = CRenderManager::GetInst()->FindRenderState("NoCull");

	if (!NoCullModeState)
	{
		assert(false);
		return;
	}
	m_SelectedMaterial->SetRenderState(NoCullModeState);

	// Particle Render Shader ����
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

	// ���� Texture �� Load �Ͽ� Setting �ϴ� ��� �����ϱ� 
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

		// FullPath ����
		char FileFullPathMultibyteCopy[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FileFullPathMultibyteCopy, ConvertLength, 0, 0);

		// ���� �̸� ����
		std::string FileName;
		std::string FileExt;
		CEditorUtil::ExtractFileNameAndExtFromPath(FileFullPathMultibyteCopy, FileName, FileExt);

		// ���� �̸��� Key������ �Ͽ�, �̹� Engine�� Load �Ǿ� �ִ��� Ȯ���Ѵ�.
		CTexture* TargetTexture = CResourceManager::GetInst()->FindTexture(FileName);

		// Texture �� Register ������ �Ѵ� �����Ѵ�.
		if (TargetTexture)
		{
			m_SelectedMaterial->SetTextureInfoResource(SetTextureIndex, TargetTexture);
			m_SelectedMaterial->SetTextureInfoRegister(SetTextureIndex, m_TextureRegister->GetVal());

			TextureSetSuccessCallback(TargetTexture);

			return;
		}

		// �װ� �ƴ϶��, Resource Manager �� ���� Load �ϰ� Texture �� �����Ѵ�.
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

	// 1) ���� Texture Manager ���� �ش� Key�� ����� Texture ������ �ִ����� �����ϰ�
	CTexture* TargetTexture = CResourceManager::GetInst()->FindTexture(InputName);

	if (TargetTexture)
	{
		m_SelectedMaterial->SetTextureInfoResource(SetTextureIndex, TargetTexture);

		TextureSetSuccessCallback(TargetTexture);
		return;
	}

	// 2) �׷��� ������, Hard Disk ���� �ҷ��ͼ� �����Ѵ�.
	// Drop �� Input �ȿ� �ִ� Texture ���� �ҷ����� 
	// Texture File �� File ����, Texture �� �����ϴ� Key ������ ����� ���̴�.
	std::string TextureKeyName;
	std::optional<std::string> FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(TEXTURE_PATH,
		InputName, TextureKeyName);

	// ã�� ���ߴٸ� 
	if (!FoundResult.has_value())
	{
		// Texture Path�� �ƴ϶�, Particle Path ������ �ѹ� ã�ƺ���.
		FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(PARTICLE_PATH,
			InputName, TextureKeyName);

		if (!FoundResult.has_value())
			return;
	}
	// 
	// Editor Util �Լ� �ȿ�, --> Ư�� ����, File �ȿ� �̸� �־��ָ�, �� ���鼭, ���� ã���ִ� �Լ� ����
	// �׷��� �ؼ� Full Path, File �̸� �޾ƿ���
	// �̸� ���� ResourceManager, Texture Manager ���� �ҷ�����
	// �� �ȿ����� ������ �״�� ����, ������ Load ���� ����

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

	// ���� Texture �� Load �Ͽ� Setting �ϴ� ��� �����ϱ� 
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
		// FullPath ����
		//const char* FileFullPathMultibyte = CEditorUtil::ChangeTCHARTextToMultibyte(FileFullPath);

		char FilePathMultibyte[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);


		char FileFullPathMultibyteCopy[MAX_PATH] = {};
		strcpy_s(FileFullPathMultibyteCopy, FilePathMultibyte);

		// ���� �̸� ����
		std::string FileName;
		std::string FileExt;
		CEditorUtil::ExtractFileNameAndExtFromPath(FileFullPathMultibyteCopy, FileName, FileExt);

		// 1) ���� �̸��� Key������ �Ͽ�, �̹� Engine�� Load �Ǿ� �ִ��� Ȯ���Ѵ�.
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

		// 2) �װ� �ƴ϶��, Resource Manager �� ���� Load �ϰ� Texture �� �����Ѵ�.
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

	// 1) ���� Texture Manager ���� �ش� Key�� ����� Texture ������ �ִ����� �����ϰ�
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
	
	// 2) ���� ���ٸ�, �� ����, �ϵ��ũ�� �������� �ش� ������ �ҷ��� ���̴�.

	// m_SelectedMaterial->SetOutLineThickNess(ThickNess);
	// Drop �� Input �ȿ� �ִ� Texture ���� �ҷ����� 
	// Texture File �� File ����, Texture �� �����ϴ� Key ������ ����� ���̴�.
	std::string TextureKeyName;

	std::optional<std::string> FoundResult = CEditorUtil::GetFullPathOfTargetFileNameInDir(TEXTURE_PATH,
		InputName, TextureKeyName);

	// ã�� ���ߴٸ� 
	if (!FoundResult.has_value())
		return;
	// 
	// Editor Util �Լ� �ȿ�, --> Ư�� ����, File �ȿ� �̸� �־��ָ�, �� ���鼭, ���� ã���ִ� �Լ� ����
	// �׷��� �ؼ� Full Path, File �̸� �޾ƿ���
	// �̸� ���� ResourceManager, Texture Manager ���� �ҷ�����
	// �� �ȿ����� ������ �״�� ����, ������ Load ���� ����

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
	// ���� Texture �� IMGUI Image �� �����ֱ�
	m_SetTexureImage->SetTexture(Texture);
	
	RefreshMaterialDisplayInfo(m_SelectedMaterial, Texture);
	
	// ResourceDisplay Window �� �ִ� Texture ��ϵ� Resource Window �� �߰��ؼ� �����ֱ�
	CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
	
	// ����� ���õǾ��ٴ� Message
	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Texture Set SuccessFully"), TEXT("Success"), NULL);
}

void CMaterialEditor::OnSaveMaterial()
{
	if (!m_SelectedMaterial)
		return;

	const PathInfo* MaterialPathInfo = CPathManager::GetInst()->FindPath(MATERIAL_PARTICLE_PATH);

	// Bin//Material//ParticleMaterial ������ �ִ��� Ȯ���ϰ� ������ش�.
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

		// Initial Name => Material �� Name ���� ����
		TCHAR TCHARInitFilename[MAX_PATH] = {};

		char FilePathMultibyte[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		CEditorUtil::ExtractFileNameAndExtFromPath(FilePathMultibyte, FileName, FileExt);

		// ���� �����ϴ� Material �� ���� �̸���, Material �� �̸��� �������� Ȯ���Ѵ�.
		if (strcmp(FileName, m_SelectedMaterial->GetName().c_str()) != 0)
		{
			TCHAR ErrorMessage[MAX_PATH] = {};
			lstrcpy(ErrorMessage, TEXT("FileName Has To Be Same With Material Name : "));
			lstrcat(ErrorMessage, TCHARInitFilename);
			MessageBox(CEngine::GetInst()->GetWindowHandle(), ErrorMessage, NULL, MB_OK);
			return;
		}

		// (�Ʒ��� �ڵ�� ���� Particle Material �뵵�θ� ����Ѵٴ� �����Ͽ� �ۼ�, ������ �ٸ� Material �������� ����ϹǷ� �Ʒ��ڵ� �����)
		// ���� �����ϴ� Directory�� Bin/Material/ParticleMaterial ���� Ȯ���ϱ� => �ƴ϶��, Save ����
		std::string PathInfoBeforeFileName;
		CEngineUtil::GetPathInfoBeforeFileName(FilePathMultibyte, PathInfoBeforeFileName);

		// if (strcmp(MaterialPathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
		// {
		// 	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Material �� ���, �ݵ�� Bin/Material/ParticleMaterial �� ����"), NULL, MB_OK);
		// 	return;
		// }

		// ���� �����ϴ� Path ����, ���� �̸��� FileName�� �����ϴ��� Ȯ���ϱ�
		char CheckMaterialFileName[MAX_PATH] = {};
		strcpy_s(CheckMaterialFileName, FileName);
		strcat_s(CheckMaterialFileName, FileExt);

		if (CEditorUtil::IsFileExistInDir(MATERIAL_PARTICLE_PATH, CheckMaterialFileName))
		{
			if (MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("���� �̸��� .mtrl ������ �����մϴ�. �����Ͻðڽ��ϱ�?"), NULL, MB_YESNO) != IDYES)
				return;
		}


		// Ȯ���� �˻��ϱ�
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

		// Ȯ���� .anim �� �ƴ϶�� return;
		if (strcmp(Ext, ".MTRL") != 0)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("EXT Has To Be .mtrl"), NULL, MB_OK);
			return;
		}

		// >> (������ �ݵ�� Particle �� Material �� ����Ϸ��� �߾���. ������, ���� �ٸ� Component �� Material Editor �ε� ����ؾ� �ϹǷ�)
		// >> (�Ʒ��� �ڵ�� ���ش�.)
		// ���� Load�ϴ� Directory�� Bin/Material/ParticleMaterial ���� Ȯ���ϱ� => �ƴ϶��, Load X
		// std::string PathInfoBeforeFileName;
		// CEngineUtil::GetPathInfoBeforeFileName(FilePathMultibyte, PathInfoBeforeFileName);
		// 
		// if (strcmp(MaterialPathInfo->PathMultibyte, PathInfoBeforeFileName.c_str()) != 0)
		// {
		// 	MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Particle Material �� ���, �ݵ�� Bin/Material/ParticleMaterial �κ��� Load �ؾ� �Ѵ�."), NULL, MB_OK);
		// 	return;
		// }

		// ���� �̸���, Material �� �����ϴ� Key ������ Ȱ���� ���̴�.
		CMaterial* LoadedMaterial = CResourceManager::GetInst()->LoadMaterialFullPathMultibyte(FilePathMultibyte);

		if (!LoadedMaterial)
		{
			MessageBox(CEngine::GetInst()->GetWindowHandle(), TEXT("Material Load Failure"), NULL, MB_OK);
			return;
		}

		m_SelectedMaterial = LoadedMaterial;

		RefreshMaterialDisplayInfo(m_SelectedMaterial);

		// ResourceDisplay Window �� �ִ� Texture ��ϵ� Resource Window �� �߰��ؼ� �����ֱ�
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

	// ���� Material �̸� �����ֱ�
	m_SelectedMaterialName->SetText(Material->GetName().c_str());

	// Edit Name Input ����ֱ� 
	m_EditMaterialName->ClearText();

	const std::vector<MaterialTextureInfo>& MtrlTexture = Material->GetTextureInfo();

	// ���� �������ְ��� �ϴ� Texture �� ����, ���� Material �� Texture �� ��� �ִٸ�
	if (!Texture && MtrlTexture.size() > 0)
	{
		int LastTexIdx = (int)MtrlTexture.size() - 1;
		// Texture Image �� ������ �� �����ֱ�
		m_SetTexureImage->SetTexture(MtrlTexture[LastTexIdx].Texture);
	}
	else
	{
		m_SetTexureImage->SetTexture(Texture);
	}

	// Texture Table �� Texture ������ ����
	// - Name, Sampler Type, Texture Name, FileName + EXT, Register ����
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

	// Set Texture, Add Texture Input ���� ����ֱ�
	m_AddTextureInput->ClearText();
	m_SetTextureInput->ClearText();

	// Info Table ����
	// Shader Name
	CShader* Shader = Material->GetShader();

	m_ShaderSetInput->ClearText();

	if (Shader)
	{
		m_ShaderSetInput->SetText(Shader->GetName().c_str());
	}
	 
	// TODO : Material �� ���� ���� ���� (����μ��� �ʿ��� ���� ����δ�)
	// BaseColor, Ambient Color, Specular Color, EmmisiveColor
	// m_MtrlInfoTable->Clear()
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameTRange, Sequence->GetStartFrame());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameRange, Sequence->GetEndFrame());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameLength, Sequence->GetFrameLength());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameMode, Sequence->GetFrameMode());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::PlayTime, Sequence->GetPlayTime());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::FrameTime, Sequence->GetFrameTime());
	// m_MtrlInfoTable->AddData(AnimationClipInfoKeys::PlayScale, Sequence->GetPlayScale());

	// Sampler ������ �����Ѵ�. -> Table ������ ����
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

	// RenderState Input �� Clear ���ش�.
	m_RenderStateSetInput->ClearText();

	// TODO : Metallic ���� ����
}
