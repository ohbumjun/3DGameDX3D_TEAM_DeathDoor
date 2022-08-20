#include "ArmComponentWidget.h"
#include "Component/Arm.h"
#include "IMGUIText.h"
#include "IMGUIInputFloat3.h"
#include "IMGUIInputFloat.h"
#include "IMGUISeperator.h"
#include "Component/Arm.h"

CArmComponentWidget::CArmComponentWidget()
{
}

CArmComponentWidget::~CArmComponentWidget()
{
}

bool CArmComponentWidget::Init()
{
	CSceneComponentWidget::Init();

	m_ComponentTypeText->SetText("Arm Component");

	m_TargetDist = AddWidget<CIMGUIInputFloat>("Target Distance", 200.f);
	m_Offset = AddWidget<CIMGUIInputFloat3>("Offset", 200.f);
	AddWidget<CIMGUISeperator>("Sep");

	m_TargetDist->SetCallBack(this, &CArmComponentWidget::OnChangeTargetDist);
	m_Offset->SetCallBack(this, &CArmComponentWidget::OnChangeOffset);

	return true;
}

void CArmComponentWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CArm* Arm = dynamic_cast<CArm*>(Com);

	m_TargetDist->SetVal(Arm->GetTargetDistance());
	m_Offset->SetVal(Arm->GetOffset());
}

void CArmComponentWidget::OnChangeTargetDist(float Val)
{
	CArm* Arm = dynamic_cast<CArm*>(m_Component);
	Arm->SetTargetDistance(Val);
}

void CArmComponentWidget::OnChangeOffset(const Vector3& Offset)
{
	CArm* Arm = dynamic_cast<CArm*>(m_Component);
	Arm->SetOffset(Offset);
}
