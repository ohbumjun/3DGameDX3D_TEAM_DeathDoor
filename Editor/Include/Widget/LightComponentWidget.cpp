#include "LightComponentWidget.h"
#include "Component/LightComponent.h"
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
#include "EngineUtil.h"

CLightComponentWidget::CLightComponentWidget()
{

}

CLightComponentWidget::~CLightComponentWidget()
{
}

bool CLightComponentWidget::Init()
{
	CSceneComponentWidget::Init();

	// Create Widget
	m_ComponentTypeText->SetText("Light Component");

	// 최상위 트리
	CIMGUITree* m_RootTree = AddWidget<CIMGUITree>("Light Variables");
	m_LightTypeEdit = m_RootTree->AddWidget<CIMGUIComboBox>("Light Type", 200.f);
	m_LightColorEdit = m_RootTree->AddWidget<CIMGUIColor3>("Light Color", 200.f);
	m_LightDistEdit = m_RootTree->AddWidget<CIMGUIInputFloat>("Light Distance", 200.f);
	m_LightAngleInEdit = m_RootTree->AddWidget<CIMGUIInputFloat>("Light Angle In", 200.f);
	m_LightAngleOutEdit = m_RootTree->AddWidget<CIMGUIInputFloat>("Light Angle Out", 200.f);
	m_LightAtt1Edit = m_RootTree->AddWidget<CIMGUISliderFloat>("Light Att1", 200.f);
	m_LightAtt2Edit = m_RootTree->AddWidget<CIMGUISliderFloat>("Light Att2", 200.f);
	m_LightAtt3Edit = m_RootTree->AddWidget<CIMGUISliderFloat>("Light Att3", 200.f);
	AddWidget<CIMGUISeperator>("Sep");

	// Initial Value
	for (int i = 0; i < (int)Light_Type::End; ++i)
	{
		m_LightTypeEdit->AddItem(CEngineUtil::LightTypeToString((Light_Type)i));
	}

	// CallBack
	m_LightTypeEdit->SetSelectCallback(this, &CLightComponentWidget::OnEditLightType);
	m_LightColorEdit->SetCallBack(this, &CLightComponentWidget::OnEditLightColor);
	m_LightDistEdit->SetCallBack(this, &CLightComponentWidget::OnEditLightDist);
	m_LightAngleInEdit->SetCallBack(this, &CLightComponentWidget::OnEditLightAngleIn);
	m_LightAngleOutEdit->SetCallBack(this, &CLightComponentWidget::OnEditLightAngleOut);
	m_LightAtt1Edit->SetCallBack(this, &CLightComponentWidget::OnEditLightAtt1);
	m_LightAtt2Edit->SetCallBack(this, &CLightComponentWidget::OnEditLightAtt2);
	m_LightAtt3Edit->SetCallBack(this, &CLightComponentWidget::OnEditLightAtt3);

	return true;
}

void CLightComponentWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);

	m_LightTypeEdit->SetSelectIndex((int)LightCom->GetLightType());
	m_LightColorEdit->SetRGB(LightCom->GetLightColor().x, LightCom->GetLightColor().y, LightCom->GetLightColor().z);
	m_LightDistEdit->SetVal(LightCom->GetLightDistance());
	m_LightAngleInEdit->SetVal(LightCom->GetLightAngleIn());
	m_LightAngleOutEdit->SetVal(LightCom->GetLightAngleOut());
	m_LightAtt1Edit->SetValue(LightCom->GetLightAtt1());
	m_LightAtt2Edit->SetValue(LightCom->GetLightAtt2());
	m_LightAtt3Edit->SetValue(LightCom->GetLightAtt3());
}

void CLightComponentWidget::OnEditLightColor(const Vector3& Color)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetColor(Vector4(Color.x, Color.y, Color.z, 1.f));
}

void CLightComponentWidget::OnEditLightType(int Idx, const char* Label)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetLightType((Light_Type)Idx);
}

void CLightComponentWidget::OnEditLightDist(float Value)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetDistance(Value);
}

void CLightComponentWidget::OnEditLightAngleIn(float Value)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetAngleIn(Value);
}

void CLightComponentWidget::OnEditLightAngleOut(float Value)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetAngleOut(Value);
}

void CLightComponentWidget::OnEditLightAtt1(float Value)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetAtt1(Value);
}

void CLightComponentWidget::OnEditLightAtt2(float Value)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetAtt2(Value);
}

void CLightComponentWidget::OnEditLightAtt3(float Value)
{
	CLightComponent* LightCom = static_cast<CLightComponent*>(m_Component);
	LightCom->SetAtt3(Value);
}
