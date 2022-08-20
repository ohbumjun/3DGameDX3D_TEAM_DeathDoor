#include "WaterComponentWidget.h"
#include "Component/WaterComponent.h"
#include "IMGUITextInput.h"
#include "IMGUIButton.h"
#include "IMGUISeperator.h"
#include "IMGUISameLine.h"
#include "IMGUIText.h"
#include "IMGUIInputInt.h"
#include "IMGUIInputFloat.h"
#include "IMGUIColor3.h"
#include "IMGUICollapsingHeader.h"
#include "Resource/ResourceManager.h"
#include "Engine.h"
#include "PathManager.h"

CWaterComponentWidget::CWaterComponentWidget()
{
}

CWaterComponentWidget::~CWaterComponentWidget()
{
}

bool CWaterComponentWidget::Init()
{
	CSceneComponentWidget::Init();

	m_ComponentTypeText->SetText("WaterComponent");

	// Add Widget
	CIMGUICollapsingHeader* RootTree = AddWidget<CIMGUICollapsingHeader>("Water Value");
	m_MatName = RootTree->AddWidget<CIMGUITextInput>("Material Name", 200.f);
	RootTree->AddWidget<CIMGUISameLine>("Line");
	m_LoadButton = RootTree->AddWidget<CIMGUIButton>("Load", 0.f, 0.f);
	m_Speed = RootTree->AddWidget<CIMGUIInputFloat>("Speed", 200.f, 0.f);
	m_FoamDepth = RootTree->AddWidget<CIMGUIInputFloat>("Foam Depth", 200.f);
	m_Attn1 = RootTree->AddWidget<CIMGUIInputFloat>("Attn1", 200.f);
	m_Attn2 = RootTree->AddWidget<CIMGUIInputFloat>("Attn2", 200.f);
	m_BaseColorEdit = RootTree->AddWidget<CIMGUIColor3>("BaseColor", 200.f);
	m_AmbientColorEdit = RootTree->AddWidget<CIMGUIColor3>("Ambient", 200.f);
	m_SpecularColorEdit = RootTree->AddWidget<CIMGUIColor3>("Specluar", 200.f);
	m_SpecluarPowerEdit = RootTree->AddWidget<CIMGUIInputFloat>("Specluar Power", 200.f);
	m_EmissiveColorEdit = RootTree->AddWidget<CIMGUIColor3>("Emissive", 200.f);
	AddWidget<CIMGUISeperator>("Sep");

	// Initial Value
	m_MatName->ReadOnly(true);
	m_MatName->SetHintText("Load Or Drop Material");

	// CallBack
	m_MatName->SetDropCallBack(this, &CWaterComponentWidget::OnDropMaterialName);
	m_LoadButton->SetClickCallback(this, &CWaterComponentWidget::OnClickLoadMaterial);
	m_Speed->SetCallBack(this, &CWaterComponentWidget::OnChangeSpeed);
	m_FoamDepth->SetCallBack(this, &CWaterComponentWidget::OnChangeFoamDepth);
	m_Attn1->SetCallBack(this, &CWaterComponentWidget::OnChangeAttn1);
	m_Attn2->SetCallBack(this, &CWaterComponentWidget::OnChangeAttn2);
	m_BaseColorEdit->SetCallBack(this, &CWaterComponentWidget::OnEditBaseColor);
	m_AmbientColorEdit->SetCallBack(this, &CWaterComponentWidget::OnEditAmbientColor);
	m_SpecularColorEdit->SetCallBack(this, &CWaterComponentWidget::OnEditSpecluarColor);
	m_EmissiveColorEdit->SetCallBack(this, &CWaterComponentWidget::OnEditEmissiveColor);
	m_SpecluarPowerEdit->SetCallBack(this, &CWaterComponentWidget::OnEditSpecluarPower);

	return true;
}

void CWaterComponentWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(Com);

	float Speed = WCom->GetSpeed();
	float FoamDepth = WCom->GetFoamDepth();
	float Attn1 = WCom->GetAttn1();
	float Attn2 = WCom->GetAttn2();

	m_Speed->SetVal(Speed);
	m_FoamDepth->SetVal(FoamDepth);
	m_Attn1->SetVal(Attn1);
	m_Attn2->SetVal(Attn2);

	CMaterial* Mat = WCom->GetMaterial();

	if (Mat)
	{
		m_BaseColorEdit->SetRGB(Mat->GetBaseColor().x, Mat->GetBaseColor().y, Mat->GetBaseColor().z);
		m_AmbientColorEdit->SetRGB(Mat->GetAmbientColor().x, Mat->GetAmbientColor().y, Mat->GetAmbientColor().z);
		m_SpecularColorEdit->SetRGB(Mat->GetSpecularColor().x, Mat->GetSpecularColor().y, Mat->GetSpecularColor().z);
		m_EmissiveColorEdit->SetRGB(Mat->GetEmissiveColor().x, Mat->GetEmissiveColor().y, Mat->GetEmissiveColor().z);
		m_SpecluarPowerEdit->SetVal(Mat->GetSpecularPower());
	}
}

void CWaterComponentWidget::OnClickLoadMaterial()
{
	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("Material File\0*.mtrl\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(MATERIAL_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char FullPathMultiByte[MAX_PATH] = {};
		int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, nullptr, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, Length, FullPathMultiByte, Length, 0, 0);

		CMaterial* LoadMat = CResourceManager::GetInst()->LoadMaterialFullPathMultibyte(FullPathMultiByte);

		if (!LoadMat)
		{
			MessageBox(nullptr, TEXT("Material 로드 실패"), TEXT("실패"), MB_OK);
			return;
		}

		CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
		WCom->SetMaterial(LoadMat);

		std::string MatName = LoadMat->GetName();
		m_MatName->SetText(MatName.c_str());
	}
}

void CWaterComponentWidget::OnEditBaseColor(const Vector3& Color)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	CMaterial* Mat = WCom->GetMaterial();

	if (Mat)
	{
		Mat->SetBaseColor(Vector4(Color.x, Color.y, Color.z, 1.f));
	}
}

void CWaterComponentWidget::OnEditAmbientColor(const Vector3& Color)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	CMaterial* Mat = WCom->GetMaterial();

	if (Mat)
	{
		Mat->SetAmbientColor(Vector4(Color.x, Color.y, Color.z, 1.f));
	}
}

void CWaterComponentWidget::OnEditSpecluarColor(const Vector3& Color)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	CMaterial* Mat = WCom->GetMaterial();

	if (Mat)
	{
		Mat->SetSpecularColor(Vector4(Color.x, Color.y, Color.z, 1.f));
	}
}

void CWaterComponentWidget::OnEditSpecluarPower(float Power)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	CMaterial* Mat = WCom->GetMaterial();

	if (Mat)
	{
		Mat->SetSpecularPower(Power);
	}
}

void CWaterComponentWidget::OnEditEmissiveColor(const Vector3& Color)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	CMaterial* Mat = WCom->GetMaterial();

	if (Mat)
	{
		Mat->SetEmissiveColor(Vector4(Color.x, Color.y, Color.z, 1.f));
	}
}

void CWaterComponentWidget::OnChangeSpeed(float Val)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	WCom->SetSpeed(Val);
}

void CWaterComponentWidget::OnChangeFoamDepth(float Val)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	WCom->SetFoamDepth(Val);
}

void CWaterComponentWidget::OnChangeAttn1(float Val)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	WCom->SetAttn1(Val);
}

void CWaterComponentWidget::OnChangeAttn2(float Val)
{
	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	WCom->SetAttn2(Val);
}

void CWaterComponentWidget::OnDropMaterialName(const std::string& Name)
{
	CMaterial* Mat = CResourceManager::GetInst()->FindMaterial(Name);

	if (!Mat)
	{
		m_MatName->ClearText();
		MessageBox(nullptr, TEXT("Material 로드 실패"), TEXT("실패"), MB_OK);
		return;
	}

	CWaterComponent* WCom = dynamic_cast<CWaterComponent*>(m_Component);
	WCom->SetMaterial(Mat);
}
