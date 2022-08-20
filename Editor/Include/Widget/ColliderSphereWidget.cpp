
#include "ColliderSphereWidget.h"
#include "IMGUIText.h"
#include "IMGUITree.h"
#include "IMGUIInputFloat.h"
#include "IMGUISeperator.h"
#include "Component/ColliderSphere.h"

CColliderSphereWidget::CColliderSphereWidget()
{
}

CColliderSphereWidget::~CColliderSphereWidget()
{
}

bool CColliderSphereWidget::Init()
{
	CColliderComponentWidget::Init();

	m_ComponentTypeText->SetText("ColliderSphere Component");

	m_RadiusTree = AddWidget<CIMGUITree>("ColliderSphere Variables");

	m_RadiusInput = m_RadiusTree->AddWidget<CIMGUIInputFloat>("Radius");
	m_RadiusInput->SetCallBack(this, &CColliderSphereWidget::OnChangeRadius);

	AddWidget<CIMGUISeperator>("Sep");

	return true;
}

void CColliderSphereWidget::SetSceneComponent(CSceneComponent* Com)
{
	CColliderComponentWidget::SetSceneComponent(Com);

	float Radius = ((CColliderSphere*)m_Component)->GetInfo().Radius;

	m_RadiusInput->SetVal(Radius);
}

void CColliderSphereWidget::OnChangeRadius(float Radius)
{
	((CColliderSphere*)m_Component)->SetRadius(Radius);
}
