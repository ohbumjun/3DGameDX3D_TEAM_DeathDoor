#include "PaperBurnWidget.h"
#include "Component/PaperburnComponent.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "Scene/SceneManager.h"
#include "PathManager.h"
#include "IMGUITextInput.h"
#include "IMGUIComboBox.h"
#include "IMGUIButton.h"
#include "IMGUICheckBox.h"
#include "IMGUIInputFloat.h"
#include "IMGUIColor4.h"
#include "IMGUIText.h"
#include "IMGUISeperator.h"
#include "IMGUISameLine.h"
#include "IMGUICollapsingHeader.h"
#include "IMGUIImage.h"

CPaperBurnWidget::CPaperBurnWidget()
{
}

CPaperBurnWidget::~CPaperBurnWidget()
{
}

bool CPaperBurnWidget::Init()
{
	CObjectComponentWidget::Init();
	
	m_ComponentTypeText->SetText("Paperburn Component");

	// Add Widget
	CIMGUICollapsingHeader* RootTree = AddWidget<CIMGUICollapsingHeader>("Paperburn Variables");
	m_TexName = RootTree->AddWidget<CIMGUITextInput>("Texture Name", 200.f);
	RootTree->AddWidget<CIMGUISameLine>("Line");
	m_LoadTex = RootTree->AddWidget<CIMGUIButton>("Load", 0.f, 0.f);
	m_Tex = RootTree->AddWidget<CIMGUIImage>("Burn Texture", 200.f, 200.f);
	m_EndEventType = RootTree->AddWidget<CIMGUIComboBox>("End Event Type", 200.f);
	m_PaperBurnComponent = RootTree->AddWidget<CIMGUITextInput>("PaperBurn Component Name", 200.f);
	RootTree->AddWidget<CIMGUISameLine>("Line");
	m_FindCompnent = RootTree->AddWidget<CIMGUIButton>("Find Component", 0.f, 0.f);
	m_Inverse = RootTree->AddWidget<CIMGUICheckBox>("Inverse", 200.f);
	m_FinishTime = RootTree->AddWidget<CIMGUIInputFloat>("Finish Time", 200.f);
	m_OutlineColor = RootTree->AddWidget<CIMGUIColor4>("Outline Color", 200.f);
	m_OutFilter = RootTree->AddWidget<CIMGUIInputFloat>("Outline Filter", 200.f);
	m_CenterlineColor = RootTree->AddWidget<CIMGUIColor4>("Centerline Color", 200.f);
	m_CenterFilter = RootTree->AddWidget<CIMGUIInputFloat>("Centerline Filter", 200.f);
	m_InlineColor = RootTree->AddWidget<CIMGUIColor4>("Inline Color", 200.f);
	m_InFilter = RootTree->AddWidget<CIMGUIInputFloat>("Inline Filter", 200.f);
	m_StartButton = RootTree->AddWidget<CIMGUIButton>("Start", 0.f, 0.f);
	AddWidget<CIMGUISeperator>("Sep");

	// Initial Value
	m_Tex->SetTableTitle("Burn Texture");
	m_Inverse->AddCheckInfo("Inverse");

	for (int i = 0; i < (int)PaperBurnEndEvent::Max; ++i)
	{
		m_EndEventType->AddItem(CEngineUtil::PaperBurnEndEventTypeToString((PaperBurnEndEvent)i));
	}

	// CallBack
	m_LoadTex->SetClickCallback(this, &CPaperBurnWidget::OnClickLoad);
	m_TexName->SetDropCallBack(this, &CPaperBurnWidget::OnDropTexture);
	m_StartButton->SetClickCallback(this, &CPaperBurnWidget::OnClickStart);
	m_Inverse->SetCallBackIdx(this, &CPaperBurnWidget::OnCheckInverse);
	m_FinishTime->SetCallBack(this, &CPaperBurnWidget::OnChangeFinishTime);
	m_OutFilter->SetCallBack(this, &CPaperBurnWidget::OnChangeOutFilter);
	m_CenterFilter->SetCallBack(this, &CPaperBurnWidget::OnChangeCenterFilter);
	m_InFilter->SetCallBack(this, &CPaperBurnWidget::OnChangeInFilter);
	m_OutlineColor->SetCallBack(this, &CPaperBurnWidget::OnChangeOutColor);
	m_CenterlineColor->SetCallBack(this, &CPaperBurnWidget::OnChangeCenterColor);
	m_InlineColor->SetCallBack(this, &CPaperBurnWidget::OnChangeInColor);
	m_EndEventType->SetSelectCallback(this, &CPaperBurnWidget::OnSelectEndEventType);
	m_FindCompnent->SetClickCallback(this, &CPaperBurnWidget::OnClickFindComponent);

	return true;
}

void CPaperBurnWidget::SetObjectComponent(CObjectComponent* Com)
{
	CObjectComponentWidget::SetObjectComponent(Com);

	CPaperBurnComponent* PCom = dynamic_cast<CPaperBurnComponent*>(m_Component);

	CTexture* PaperBurnTex = PCom->GetTexture();
	m_TexName->SetText(PaperBurnTex->GetName().c_str());
	m_Tex->SetTexture(PaperBurnTex);

	m_Inverse->SetCheck(0, PCom->IsInverse());
	m_FinishTime->SetVal(PCom->GetFinishTime());
	m_OutlineColor->SetRGBA(PCom->GetOutlineColor());
	m_CenterlineColor->SetRGBA(PCom->GetCenterlineColor());
	m_InlineColor->SetRGBA(PCom->GetInlineColor());
	m_OutFilter->SetVal(PCom->GetOutFilter());
	m_CenterFilter->SetVal(PCom->GetCenterFilter());
	m_InFilter->SetVal(PCom->GetInFilter());

	PaperBurnEndEvent EndEventType = PCom->GetEndEvent();
	std::string	BurnComponentName = PCom->GetPaperBurnComponentName();

	m_EndEventType->SetSelectIndex((int)EndEventType);
	m_PaperBurnComponent->SetText(BurnComponentName.c_str());
}

void CPaperBurnWidget::OnClickLoad()
{
	std::string InputTexName = m_TexName->GetTextMultibyte();

	if (InputTexName.empty())
	{
		MessageBox(nullptr, TEXT("로드할 텍스쳐 이름 없음"), TEXT("실패"), MB_OK);
		return;
	}

	CTexture* OverlapNameTex = CSceneManager::GetInst()->GetScene()->GetResource()->FindTexture(InputTexName);

	if (OverlapNameTex)
	{
		MessageBox(nullptr, TEXT("같은 이름의 텍스쳐 존재함. 이미 로드된 텍스쳐를 설정하고 싶다면 Resources/Display/Texture 탭에서 드래그/드랍"), TEXT("실패"), MB_OK);
		return;
	}

	TCHAR   FilePath[MAX_PATH] = {};
	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("모든 파일\0*.*\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(TEXTURE_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		if (!CSceneManager::GetInst()->GetScene()->GetResource()->LoadTextureFullPath(InputTexName, FilePath))
		{
			MessageBox(nullptr, TEXT("텍스쳐 로드 실패"), TEXT("실패"), MB_OK);
			return;
		}

		CTexture* LoadTex = CSceneManager::GetInst()->GetScene()->GetResource()->FindTexture(InputTexName);
		m_Tex->SetTexture(LoadTex);

		CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);
		Com->SetBurnTexture(LoadTex);

		MessageBox(nullptr, TEXT("텍스쳐 로드 성공"), TEXT("성공"), MB_OK);
	}
}

void CPaperBurnWidget::OnSelectEndEventType(int Idx, const char* Label)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);

	Com->SetEndEvent((PaperBurnEndEvent)Idx);
}

void CPaperBurnWidget::OnClickFindComponent()
{
	std::string InputCompName = m_PaperBurnComponent->GetTextMultibyte();

	CGameObject* OwnerObject = m_Component->GetGameObject();

	CComponent* DropComponent = OwnerObject->FindComponent(InputCompName);

	bool Success = false;
	if (DropComponent->GetComponentType() == Component_Type::SceneComponent)
	{
		Success = dynamic_cast<CPaperBurnComponent*>(m_Component)->SetPaperBurnComponent((CSceneComponent*)DropComponent);
		if (Success)
		{
			MessageBox(nullptr, TEXT("컴포넌트 지정 성공"), TEXT("성공"), MB_OK);
		}
	}

	if (!Success)
	{
		m_PaperBurnComponent->ClearText();
		MessageBox(nullptr, TEXT("PaperBurn Component 지정 실패 - 컴포넌트 타입이 다르거나, 오브젝트에 해당 컴포넌트 없음"), TEXT("실패"), MB_OK);
	}
}

void CPaperBurnWidget::OnCheckInverse(int Idx, bool Bool)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);
	
	Com->SetInverse(Bool);
}

void CPaperBurnWidget::OnChangeFinishTime(float Val)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);
	
	Com->SetFinishTime(Val);
}

void CPaperBurnWidget::OnChangeInFilter(float Val)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);
	
	Com->SetInFilter(Val);
}

void CPaperBurnWidget::OnChangeCenterFilter(float Val)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);
	
	Com->SetCenterFilter(Val);
}

void CPaperBurnWidget::OnChangeOutFilter(float Val)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);
	
	Com->SetOutFilter(Val);
}

void CPaperBurnWidget::OnChangeInColor(const Vector4& Color)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);

	Com->SetInLineColor(Color);
}

void CPaperBurnWidget::OnChangeCenterColor(const Vector4& Color)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);

	Com->SetCenterLineColor(Color);
}

void CPaperBurnWidget::OnChangeOutColor(const Vector4& Color)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);

	Com->SetOutLineColor(Color);
}

void CPaperBurnWidget::OnClickStart()
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);

	Com->StartPaperBurn();
}

void CPaperBurnWidget::OnDropTexture(const std::string& Name)
{
	CPaperBurnComponent* Com = dynamic_cast<CPaperBurnComponent*>(m_Component);

	CTexture* DropTex = CSceneManager::GetInst()->GetScene()->GetResource()->FindTexture(Name);

	if (DropTex)
	{
		Com->SetBurnTexture(DropTex);
		m_Tex->SetTexture(DropTex);
	}
}
