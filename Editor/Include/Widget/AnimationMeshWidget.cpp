// IMGUI Widget
#include "ToonShaderWidget.h"
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
#include "IMGUITable.h"
#include "IMGUITableElemList.h"
#include "IMGUIComboBox.h"
#include "IMGUIDummy.h"
#include "IMGUITree.h"
#include "IMGUICollapsingHeader.h"
// Components
#include "Component/AnimationMeshComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Engine.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Render/RenderManager.h"
#include "Component/SceneComponent.h"
#include "Animation/AnimationSequenceInstance.h"
// Editor Windows
#include "ParticleComponentWidget.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "../EditorInfo.h"
#include "../EditorUtil.h"
#include "../Object/3DCameraObject.h"
#include "../EditorManager.h"
#include "AnimationMeshWidget.h"
#include "../Window/ResourceDisplayWindow.h"
// C++ 17
#include <filesystem>
// Shader
#include "Resource/Shader/ToonShader.h"

CAnimationMeshWidget::CAnimationMeshWidget()
{
}

CAnimationMeshWidget::~CAnimationMeshWidget()
{
}

bool CAnimationMeshWidget::Init()
{
    CSceneComponentWidget::Init();

    // AddWidget
    m_ComponentTypeText->SetText("Animation Mesh Component");

    // 최상위 트리
    CIMGUITree* m_RootTree = AddWidget<CIMGUITree>("Animation Variables");
    m_CurrentAnimSequence = m_RootTree->AddWidget<CIMGUITextInput>("Current Sqc", 150.f, 30.f);

    // Load & Save
    m_LoadAnimInstanceBtn = m_RootTree->AddWidget<CIMGUIButton>("Load Inst", 150.f, 20.f);

    // Anim Table
	m_AnimInfoTable = m_RootTree->AddWidget<CIMGUITableElemList>("TestTable", 200.f, 150.f);

	// Transparent는 모든 Material에 일괄 적용
	m_TransparencyEdit = m_RootTree->AddWidget<CIMGUICheckBox>("Enable Transparency", 200.f);

	m_MaterialSlotCombo = m_RootTree->AddWidget<CIMGUIComboBox>("Material Slot", 200.f);
	m_ShaderName = m_RootTree->AddWidget<CIMGUITextInput>("Shader", 200.f);
	m_ShaderWidgetTree = m_RootTree->AddWidget<CIMGUICollapsingHeader>("Shader Params", 200.f);
	m_BaseColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("BaseColor", 200.f);
	m_AmbientColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("Ambient", 200.f);
	m_SpecularColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("Specluar", 200.f);
	m_SpecluarPowerEdit = m_RootTree->AddWidget<CIMGUIInputFloat>("Specluar Power", 200.f);
	m_EmissiveColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("Emissive", 200.f);
	m_Metallic = m_RootTree->AddWidget<CIMGUICheckBox>("Metallic", 200.f);
	m_OpacityEdit = m_RootTree->AddWidget<CIMGUISliderFloat>("Opacity", 200.f);
	
	AddWidget<CIMGUISeperator>("Sep");

	// Initial Setting
	m_TransparencyEdit->AddCheckInfo("Transparency");
	m_Metallic->AddCheckInfo("Metallic");
	m_OpacityEdit->SetMin(0.f);
	m_OpacityEdit->SetMax(1.f);
	m_ShaderName->ReadOnly(true);

	// CallBack
	m_MaterialSlotCombo->SetSelectCallback(this, &CAnimationMeshWidget::OnSelectMaterialSlotCombo);
	m_BaseColorEdit->SetCallBack(this, &CAnimationMeshWidget::OnEditBaseColor);
	m_AmbientColorEdit->SetCallBack(this, &CAnimationMeshWidget::OnEditAmbientColor);
	m_SpecularColorEdit->SetCallBack(this, &CAnimationMeshWidget::OnEditSpecluarColor);
	m_EmissiveColorEdit->SetCallBack(this, &CAnimationMeshWidget::OnEditEmissiveColor);
	m_SpecluarPowerEdit->SetCallBack(this, &CAnimationMeshWidget::OnEditSpecluarPower);
	m_TransparencyEdit->SetCallBackIdx(this, &CAnimationMeshWidget::OnCheckTransparency);
	m_OpacityEdit->SetCallBack(this, &CAnimationMeshWidget::OnEditOpacity);
	m_LoadAnimInstanceBtn->SetClickCallback<CAnimationMeshWidget>(this, &CAnimationMeshWidget::OnLoadAnimationInstance);
	m_ShaderName->SetDropCallBack(this, &CAnimationMeshWidget::OnDropShaderName);
	m_Metallic->SetCallBackIdx(this, &CAnimationMeshWidget::OnCheckMetallic);

    // CIMGUISameLine* Line = m_RootTree->AddWidget<CIMGUISameLine>("Line");
    // Line->SetOffsetX(125.f);

    // m_SaveAnimInstanceBtn = m_RootTree->AddWidget<CIMGUIButton>("Save", 60.f, 20.f);

    // Text Input
    // m_ReNameSequenceBtn = m_RootTree->AddWidget<CIMGUIButton>("Edit", 30.f, 20.f);
	// 
    // Line = m_RootTree->AddWidget<CIMGUISameLine>("Line");
    // Line->SetOffsetX(65.f);
	// 
    // m_NewNameInput = m_RootTree->AddWidget<CIMGUITextInput>("Edit Key Name", 80.f, 30.f);

	return true;
}

void CAnimationMeshWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CAnimationSequenceInstance* Instance =  dynamic_cast<CAnimationMeshComponent*>(Com)->GetAnimationInstance();

	if (Instance)
	{
		// m_Animation 은 어차피, Animation Mesh Component 가 가지고 있는 Animation Instance 에 대한 포인터만을 들고 있는 형태
		m_Animation = Instance;

		SetAnimationRelatedInfoToWidget(m_Animation);
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		RefreshMeshWidget(MeshCom->GetMesh());
	}
}

void CAnimationMeshWidget::OnClickLoadMesh()
{
}

void CAnimationMeshWidget::OnSelectMaterialSlotCombo(int Idx, const char* Label)
{
	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		CMaterial* Mat = MeshCom->GetMaterial(Idx);
		std::string ShaderName = Mat->GetShader()->GetName();
		m_ShaderName->SetText(ShaderName.c_str());

		MakeShaderWidget(Mat, ShaderName);

		m_BaseColorEdit->SetRGB(Mat->GetBaseColor().x, Mat->GetBaseColor().y, Mat->GetBaseColor().z);
		m_AmbientColorEdit->SetRGB(Mat->GetAmbientColor().x, Mat->GetAmbientColor().y, Mat->GetAmbientColor().z);
		m_SpecularColorEdit->SetRGB(Mat->GetSpecularColor().x, Mat->GetSpecularColor().y, Mat->GetSpecularColor().z);
		m_EmissiveColorEdit->SetRGB(Mat->GetEmissiveColor().x, Mat->GetEmissiveColor().y, Mat->GetEmissiveColor().z);
		m_SpecluarPowerEdit->SetVal(Mat->GetSpecularPower());
		m_OpacityEdit->SetValue(Mat->GetOpacity());
		m_Metallic->SetCheck(0, Mat->IsMetallic());
	}
}

void CAnimationMeshWidget::OnEditBaseColor(const Vector3& Color)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{ 
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetBaseColor(Vector4(Color.x, Color.y, Color.z, 1.f), m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CAnimationMeshWidget::OnEditAmbientColor(const Vector3& Color)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetAmbientColor(Vector4(Color.x, Color.y, Color.z, 1.f), m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CAnimationMeshWidget::OnEditSpecluarColor(const Vector3& Color)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetSpecularColor(Vector4(Color.x, Color.y, Color.z, 1.f), m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CAnimationMeshWidget::OnEditSpecluarPower(float Power)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetSpecularPower(Power, m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CAnimationMeshWidget::OnEditEmissiveColor(const Vector3& Color)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetEmissiveColor(Vector4(Color.x, Color.y, Color.z, 1.f), m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CAnimationMeshWidget::OnCheckTransparency(int Idx, bool Check)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetTransparencyAllMaterial(Check);
	}
}

void CAnimationMeshWidget::OnEditOpacity(float Opacity)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetOpacity(Opacity, m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CAnimationMeshWidget::OnCheckMetallic(int Idx, bool Check)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetMetallic(Check, m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CAnimationMeshWidget::OnLoadAnimationInstance()
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
		char FileName[MAX_PATH] = {};
		char FilePathMultibyte[MAX_PATH] = {};
		
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);
		_splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, FileName, MAX_PATH, Ext, _MAX_EXT);

		_strupr_s(Ext);

		// 확장자 .anim 이 아니라면 return;
		if (strcmp(Ext, ".ANIM") != 0)
			return;

		// Animation .anim File 들은, .anim 파일 확장자로 저장된 
		// 파일 이름으로 식별할 것이다.
		// 같은 Animation File 을 Load 하는 것이라면 Skip
		if (m_Animation && strcmp(FileName, m_Animation->GetSavedFileName()) == 0)
			return;

		// 기존에 Load 해둔 Animation Instance 가 있다면 지워준다.
		if (m_Animation)
		{
			ClearExistingAnimationSeqInfos();

			dynamic_cast<CAnimationMeshComponent*>(m_Component)->DeleteAnimationInstance();

			m_Animation = nullptr;
		}

		m_Animation = dynamic_cast<CAnimationMeshComponent*>(m_Component)->LoadAnimationInstance<CAnimationSequenceInstance>();

		m_Animation->LoadAnimationFullPath(FilePathMultibyte);

		// CurrentAnimation 체크, + Msh, Skeleton 모두 Load 하기 
		// bool LoadResult = LoadElementsForSqcLoading();
		std::pair<bool, std::string> LoadResult = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(m_Animation);

		if (!m_Animation->GetCurrentAnimation() || LoadResult.first == false)
		{
			ClearExistingAnimationSeqInfos();

			dynamic_cast<CAnimationMeshComponent*>(m_Component)->DeleteAnimationInstance();
			m_Animation = nullptr;

			return;
		}

		m_LoadedMeshName = LoadResult.second;

		// Mesh 를 바꾸기 전에 Mesh 를 공유하는 InstancingCheckingList 목록에서 제거 --> 아래 Set Mesh 에서 어차피 해준다.
		// bool DeleteResult = dynamic_cast<CAnimationMeshComponent*>(m_Component.Get())->DeleteInstancingCheckList();
		// 현재 Load 한 Mesh 를 세팅
		dynamic_cast<CAnimationMeshComponent*>(m_Component)->SetMesh(m_LoadedMeshName);

		// Animation 관련 정보를 모두 정상적으로 Load 했다면 Start 함수 호출하여, 필요한 정보 세팅
		m_Animation->Start();

		// Animation Inst 관련 IMGUI Update
		SetAnimationRelatedInfoToWidget(m_Animation);

		// Mesh, Material 관련 IMGUI Update
		RefreshMeshWidget(dynamic_cast<CAnimationMeshComponent*>(m_Component)->GetMesh());


		// Resource Display Window Update 하기
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
	}
}

void CAnimationMeshWidget::OnSaveAnimationInstance()
{
}

void CAnimationMeshWidget::OnEditAnimationKeyName()
{
}

void CAnimationMeshWidget::OnRefreshAnimationInfo()
{
	if (!m_Animation)
		return;

	m_AnimInfoTable->ClearContents();

	std::unordered_map<std::string, CAnimationSequenceData*>& AnimationSequences = m_Animation->GetAnimationSequenceMap();

	auto iter = AnimationSequences.begin();
	auto iterEnd = AnimationSequences.end();

	size_t Size = AnimationSequences.size();

	for (; iter != iterEnd; ++iter)
	{
		CAnimationSequenceData* Sequence = iter->second;

		m_AnimInfoTable->AddData(AnimationClipInfoKeys::AnimSeqKey, iter->first);
		
		const char* CharFileName = Sequence->GetAnimationSequence()->GetSequenceFileNameMultibyte();

		char FileName[MAX_PATH] = {};

		strcpy_s(FileName, CharFileName);

		m_AnimInfoTable->AddData(AnimationClipInfoKeys::AnimSeqFileName, FileName);
	}
	
}

void CAnimationMeshWidget::OnDropShaderName(const std::string& Name)
{
	int Index = m_MaterialSlotCombo->GetSelectIndex();

	if (Index == -1)
	{
		return;
	}

	CShader* Shader = CSceneManager::GetInst()->GetScene()->GetResource()->FindShader(Name);

	if (!Shader || Shader->GetShaderType() != Shader_Type::Graphic)
	{
		return;
	}

	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetMaterialShader(Index, (CGraphicShader*)Shader);
	}

	m_ShaderName->SetText(Name.c_str());
}

void CAnimationMeshWidget::SetAnimationRelatedInfoToWidget(CAnimationSequenceInstance* AnimationInstance)
{
	if (!AnimationInstance)
		return;

	// Current Animation 정보로 세팅한다.
	int CurAnimIdx = m_Animation->GetCurrentAnimationOrder();

	if (CurAnimIdx == -1)
		return;

	// Current Animation Key Name 을 Sequenc eKey Text 에 세팅
	if (m_Animation->GetCurrentAnimationKeyName().has_value())
	{
		m_CurrentAnimSequence->SetText(m_Animation->GetCurrentAnimationKeyName().value().c_str());
	}

	// Table 정보 세팅
	OnRefreshAnimationInfo();

 //	// 현재 Scene의 정보를 m_Scene으로 지정해준다
 //	m_Animation->SetScene(CSceneManager::GetInst()->GetScene());

	// Engine 을 Play 한다.
	CEngine::GetInst()->Play();

	// Animation을 시작한다
	m_Animation->Play();
}

bool CAnimationMeshWidget::LoadElementsForSqcLoading()
{
	std::pair<bool, std::string> Result = CResourceManager::GetInst()->LoadMeshTextureBoneInfo(m_Animation);

	if (!Result.first)
		return false;

	m_LoadedMeshName = Result.second;

	// Mesh 를 바꾸기 전에 Mesh 를 공유하는 InstancingCheckingList 목록에서 제거
	bool DeleteResult = dynamic_cast<CAnimationMeshComponent*>(m_Component)->DeleteInstancingCheckList();

	// 현재 Load 한 Mesh 를 세팅
	dynamic_cast<CAnimationMeshComponent*>(m_Component)->SetMesh(m_LoadedMeshName);

	return true;
}

void CAnimationMeshWidget::ClearExistingAnimationSeqInfos()
{
	if (!m_Animation)
		return;

	m_Animation->ClearAnimationSequenceFromAnimationEditor();

	// Combo Box Clear
	// m_CurAnimComboBox->Clear();

	// Table Claer
	m_AnimInfoTable->ClearContents();
}

void CAnimationMeshWidget::RefreshMeshWidget(CMesh* Mesh)
{
	CAnimationMeshComponent* MeshCom = (CAnimationMeshComponent*)m_Component;

	std::string AutoName;

	int MatSlotSize = MeshCom->GetMaterialSlotCount();

	for (int i = 0; i < MatSlotSize; ++i)
	{
		// Material 이름이 없을 경우 자동으로 이름 지정
		if (MeshCom->GetMaterial(i)->GetName().empty())
		{
			AutoName = "Material" + std::to_string(i);
			m_MaterialSlotCombo->AddItem(AutoName);
			AutoName.clear();
		}
		else
		{
			m_MaterialSlotCombo->AddItem(MeshCom->GetMaterial(i)->GetName());
		}
	}

	m_TransparencyEdit->SetCheck(0, MeshCom->IsTransparent());
}

bool CAnimationMeshWidget::MakeShaderWidget(class CMaterial* Mat, const std::string& ShaderName)
{
	m_ShaderWidgetTree->ClearWidget();

	CShader* Shader = CResourceManager::GetInst()->FindShader(ShaderName);
	size_t TypeID = Shader->GetTypeID();

	if (TypeID == typeid(CToonShader).hash_code())
	{
		m_ShaderWidget = m_ShaderWidgetTree->AddWidget<CToonShaderWidget>("ToonShaderWidget", 200.f, 200.f);
	}
	else
	{
		return false;
	}

	ShaderParams MatShaderParams = Mat->GetShaderParams();
	m_ShaderWidget->SetShaderParams(MatShaderParams);
	m_ShaderWidget->SetMaterial(Mat);
	return true;
}
