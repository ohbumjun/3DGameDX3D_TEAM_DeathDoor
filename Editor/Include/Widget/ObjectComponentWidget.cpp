#include "ObjectComponentWidget.h"
#include "IMGUITextInput.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUIDummy.h"
#include "Component/ObjectComponent.h"
#include "IMGUIManager.h"
#include "../Window/ObjectComponentWindow.h"
#include "../EditorInfo.h"

CObjectComponentWidget::CObjectComponentWidget() :
	m_NameInput(nullptr),
	m_RenameButton(nullptr),
	m_EnableCheckBox(nullptr)
{
}

CObjectComponentWidget::~CObjectComponentWidget()
{
}

bool CObjectComponentWidget::Init()
{
	CIMGUIWidgetList::Init();
	
	// AddWidget
	m_ComponentTypeText = AddWidget<CIMGUIText>("Text");
	m_ComponentTypeText->SetText("Object Component Name");

	AddWidget<CIMGUISameLine>("Line");

	m_EnableCheckBox = AddWidget<CIMGUICheckBox>("ObjEnable");
	m_EnableCheckBox->AddCheckInfo("Enable");

	m_NameInput = AddWidget<CIMGUITextInput>("ObjectComponentNameInput");
	m_NameInput->SetHideName(true);
	AddWidget<CIMGUISameLine>("Line");
	m_RenameButton = AddWidget<CIMGUIButton>("Rename", 0.f, 0.f);

	AddWidget<CIMGUISeperator>("Sep");

	// CallBack
	m_RenameButton->SetClickCallback(this, &CObjectComponentWidget::OnClickRenameButton);
	m_EnableCheckBox->SetCallBackIdx(this, &CObjectComponentWidget::OnCheckEnableCheckBox);

	return true;
}

void CObjectComponentWidget::SetObjectComponent(CObjectComponent* Com)
{
	m_Component = Com;

	m_EnableCheckBox->SetCheck(0, m_Component->IsEnable());
	m_NameInput->SetText(m_Component->GetName().c_str());
	m_PrevName = m_NameInput->GetName();
}

void CObjectComponentWidget::OnGameObjectEnable(bool Enable)
{
	m_EnableCheckBox->SetCheck(0, Enable);
}

void CObjectComponentWidget::OnClickRenameButton()
{
	// Hierachy °»½Å
	CObjectComponentWindow* Window = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
	Window->OnRenameComponent(m_NameInput->GetTextMultibyte(), m_PrevName);

	m_Component->SetName(m_NameInput->GetTextMultibyte());
	m_PrevName = m_NameInput->GetTextMultibyte();
}

void CObjectComponentWidget::OnCheckEnableCheckBox(int Idx, bool Check)
{
	m_Component->Enable(Check);
}
