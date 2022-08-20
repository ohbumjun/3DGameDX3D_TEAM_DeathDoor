
#include "TriggerBoxDataWidget.h"
#include "../DeathDoor/Component/TriggerBoxData.h"
#include "IMGUIComboBox.h"
#include "IMGUICheckBox.h"

CTriggerBoxDataWidget::CTriggerBoxDataWidget()	:
	m_Data(nullptr)
{
}

CTriggerBoxDataWidget::~CTriggerBoxDataWidget()
{
}

bool CTriggerBoxDataWidget::Init()
{
	CObjectComponentWidget::Init();

	// AddWidget
	m_SelectOrder = AddWidget<CIMGUIComboBox>("Box Order");
	m_CurrentActive = AddWidget<CIMGUICheckBox>("Active");

	m_CurrentActive->AddCheckInfo("Active");

	m_SelectOrder->AddItem("1");
	m_SelectOrder->AddItem("2");
	m_SelectOrder->AddItem("3");
	m_SelectOrder->AddItem("4");
	m_SelectOrder->AddItem("5");

	m_SelectOrder->SetSelectCallback<CTriggerBoxDataWidget>(this, &CTriggerBoxDataWidget::OnSelectOrder);
	m_CurrentActive->SetCallBackIdx<CTriggerBoxDataWidget>(this, &CTriggerBoxDataWidget::OnSetCurrentActive);

	return true;
}

void CTriggerBoxDataWidget::SetObjectComponent(CObjectComponent* Com)
{
	CObjectComponentWidget::SetObjectComponent(Com);

	m_Data = dynamic_cast<CTriggerBoxData*>(m_Component);

	bool CurrentActive = m_Data->IsCurrentActive();

	m_CurrentActive->SetCheck(0, CurrentActive);

	int Order = m_Data->GetBoxOrder();

	m_SelectOrder->SetSelectIndex(Order - 1);
}

void CTriggerBoxDataWidget::OnSetCurrentActive(int Idx, bool Active)
{
	m_Data->SetCurrentActive(Active);
}

void CTriggerBoxDataWidget::OnSelectOrder(int Order, const char* Label)
{
	m_Data->SetBoxOrder(Order + 1);
}
