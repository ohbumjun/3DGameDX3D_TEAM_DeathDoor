
#include "EyeLaserComponentWidget.h"
#include "../DeathDoor/Component/EyeLaserComponent.h"
#include "IMGUITextInput.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUISliderFloat.h"
#include "IMGUIColor3.h"
#include "IMGUIInputFloat.h"
#include "IMGUIComboBox.h"
#include "IMGUIDummy.h"
#include "IMGUITree.h"
#include "Engine.h"
#include "PathManager.h"
#include "Scene/SceneManager.h"
#include "EngineUtil.h"

CEyeLaserComponentWidget::CEyeLaserComponentWidget()
{

}

CEyeLaserComponentWidget::~CEyeLaserComponentWidget()
{
}

bool CEyeLaserComponentWidget::Init()
{
	CSceneComponentWidget::Init();

	m_ComponentTypeText->SetText("EyeLaser Component");

	// 최상위 트리
	CIMGUITree* m_RootTree = AddWidget<CIMGUITree>("EyeLaser Variables");

	m_MaterialTextureChangeButton = m_RootTree->AddWidget<CIMGUIButton>("Material Change");
	m_MaterialTextureChangeButton->SetSize(100.f, 20.f);

	m_MeshName = m_RootTree->AddWidget<CIMGUITextInput>("Mesh Name");
	m_RootTree->AddWidget<CIMGUISameLine>("Line");

	// Transparent는 모든 Material에 일괄 적용
	m_TransparencyEdit = m_RootTree->AddWidget<CIMGUICheckBox>("Enable Transparency", 200.f);

	m_MaterialSlotCombo = m_RootTree->AddWidget<CIMGUIComboBox>("Material Slot", 200.f);
	m_BaseColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("BaseColor", 200.f);
	m_ShaderName = m_RootTree->AddWidget<CIMGUITextInput>("Shader", 200.f);
	m_ShaderName->ReadOnly(true);
	m_EmissiveColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("Emissive", 200.f);
	m_OpacityEdit = m_RootTree->AddWidget<CIMGUISliderFloat>("Opacity", 200.f);

	AddWidget<CIMGUISeperator>("Sep");

	// Initial Setting

	m_TransparencyEdit->AddCheckInfo("Transparency");
	m_OpacityEdit->SetMin(0.f);
	m_OpacityEdit->SetMax(1.f);

	// CallBack
	m_MaterialSlotCombo->SetSelectCallback(this, &CEyeLaserComponentWidget::OnSelectMaterialSlotCombo);
	m_BaseColorEdit->SetCallBack(this, &CEyeLaserComponentWidget::OnEditBaseColor);
	m_EmissiveColorEdit->SetCallBack(this, &CEyeLaserComponentWidget::OnEditEmissiveColor);
	m_TransparencyEdit->SetCallBackIdx(this, &CEyeLaserComponentWidget::OnCheckTransparency);
	m_OpacityEdit->SetCallBack(this, &CEyeLaserComponentWidget::OnEditOpacity);
	m_MaterialTextureChangeButton->SetClickCallback<CEyeLaserComponentWidget>(this, &CEyeLaserComponentWidget::OnChangeMaterialTexture);

	return true;
}

void CEyeLaserComponentWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		RefreshMeshWidget(MeshCom->GetMesh());
	}

	m_OpacityEdit->SetValue(MeshCom->GetMaterial()->GetOpacity());
}

void CEyeLaserComponentWidget::OnEditBaseColor(const Vector3 & Color)
{
	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetBaseColor(Vector4(Color.x, Color.y, Color.z, 1.f), m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CEyeLaserComponentWidget::OnEditEmissiveColor(const Vector3& Color)
{
	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetEmissiveColor(Vector4(Color.x, Color.y, Color.z, 1.f), m_MaterialSlotCombo->GetSelectIndex());
	}
}

void CEyeLaserComponentWidget::OnCheckTransparency(int Idx, bool Check)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetTransparencyMaterial(Check);
	}
}

void CEyeLaserComponentWidget::OnEditOpacity(float Opacity)
{
	if (m_MaterialSlotCombo->GetSelectIndex() == -1)
	{
		return;
	}

	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		MeshCom->SetOpacity(Opacity);
	}
}

void CEyeLaserComponentWidget::OnChangeMaterialTexture()
{
	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("png File\0*.png\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(TEXTURE_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		std::string TextureName;
		char FilePathMultibyte[MAX_PATH] = {};

		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		CEngineUtil::GetFileNameOnly(FilePathMultibyte, TextureName);
		if (!CSceneManager::GetInst()->GetScene()->GetResource()->LoadTextureFullPath(TextureName, FilePath))
		{
			MessageBox(nullptr, TEXT("텍스쳐 로드 실패"), TEXT("실패"), MB_OK);
		}

		CTexture* NewTexture = CSceneManager::GetInst()->GetScene()->GetResource()->FindTexture(TextureName);

		MeshCom->GetMaterial()->SetTexture(0, 0, (int)Buffer_Shader_Type::Pixel, TextureName, NewTexture);

		//RefreshMeshWidget(MeshCom->GetMesh());
	}
}

void CEyeLaserComponentWidget::OnSelectMaterialSlotCombo(int Idx, const char* Label)
{
	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	if (MeshCom->GetMesh())
	{
		CMaterial* Mat = MeshCom->GetMaterial();
		std::string ShaderName = Mat->GetShader()->GetName();
		m_ShaderName->SetText(ShaderName.c_str());

		m_BaseColorEdit->SetRGB(Mat->GetBaseColor().x, Mat->GetBaseColor().y, Mat->GetBaseColor().z);
		m_EmissiveColorEdit->SetRGB(Mat->GetEmissiveColor().x, Mat->GetEmissiveColor().y, Mat->GetEmissiveColor().z);
		m_TransparencyEdit->SetCheck(0, Mat->IsTransparent());
		m_OpacityEdit->SetValue(Mat->GetOpacity());
	}
}

void CEyeLaserComponentWidget::RefreshMeshWidget(CMesh* Mesh)
{
	CEyeLaserComponent* MeshCom = (CEyeLaserComponent*)m_Component;

	m_MeshName->SetText(MeshCom->GetMesh()->GetName().c_str());

	std::string AutoName;

	// Material 이름이 없을 경우 자동으로 이름 지정
	if (MeshCom->GetMaterial()->GetName().empty())
	{
		AutoName = "Material0";
		m_MaterialSlotCombo->AddItem(AutoName);
		AutoName.clear();
	}
	else
	{
		m_MaterialSlotCombo->AddItem(MeshCom->GetMaterial()->GetName());
	}


	m_TransparencyEdit->SetCheck(0, MeshCom->IsTransparent());
}