#include "SceneComponentWidget.h"
#include "IMGUITextInput.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUIDummy.h"
#include "IMGUITree.h"
#include "Component/SceneComponent.h"
#include "../Widget/TransformWidget.h"
#include "IMGUIManager.h"
#include "../Window/SceneComponentHierarchyWindow.h"
#include "../EditorInfo.h"

CSceneComponentWidget::CSceneComponentWidget()	:
	m_NameInput(nullptr),
	m_RenameButton(nullptr),
	m_EnableCheckBox(nullptr),
	m_TransformWidget(nullptr)
{
}

CSceneComponentWidget::~CSceneComponentWidget()
{
}

bool CSceneComponentWidget::Init()
{
	CIMGUIWidgetList::Init();
	
	// AddWidget
	m_ComponentTypeText = AddWidget<CIMGUIText>("CompTypeText");
	m_ComponentTypeText->SetText("SceneComponent");

	AddWidget<CIMGUISameLine>("Line");

	m_EnableCheckBox = AddWidget<CIMGUICheckBox>("Enable");
	m_EnableCheckBox->AddCheckInfo("Enable");

	m_NameInput = AddWidget<CIMGUITextInput>("Name");
	AddWidget<CIMGUISameLine>("Line");
	m_RenameButton = AddWidget<CIMGUIButton>("Rename", 0.f, 0.f);

	CIMGUITree* TransformTree = AddWidget<CIMGUITree>("Transform");
	m_TransformWidget = TransformTree->AddWidget<CTransformWidget>("Transform");

	// CallBack
	m_RenameButton->SetClickCallback(this, &CSceneComponentWidget::OnClickRenameButton);
	m_EnableCheckBox->SetCallBackIdx(this, &CSceneComponentWidget::OnCheckEnableCheckBox);

	return true;
}


void CSceneComponentWidget::SetSceneComponent(CSceneComponent* Com)
{
	m_Component = Com;

	m_EnableCheckBox->SetCheck(0, m_Component->IsEnable());
	m_NameInput->SetText(m_Component->GetName().c_str());
	m_TransformWidget->SetSceneCompoent(m_Component);
	m_PrevName = m_Component->GetName();
}

void CSceneComponentWidget::OnGameObjectEnable(bool Enable)
{
	m_EnableCheckBox->SetCheck(0, Enable);
}

bool CSceneComponentWidget::IsMyWidget(CSceneComponent* Component)
{
	return Component == m_Component;
}

void CSceneComponentWidget::OnClickRenameButton()
{
	// Hierachy °»½Å
	CSceneComponentHierarchyWindow* Window = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
	Window->OnRenameComponent(m_NameInput->GetTextMultibyte(), m_PrevName);

	m_Component->SetName(m_NameInput->GetTextMultibyte());
	m_PrevName = m_NameInput->GetTextMultibyte();
}

void CSceneComponentWidget::OnCheckEnableCheckBox(int Idx, bool Check)
{
	m_Component->Enable(Check);
}
