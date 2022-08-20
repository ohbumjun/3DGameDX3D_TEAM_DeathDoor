#include "TransformWidget.h"
#include "Component/SceneComponent.h"
#include "IMGUIText.h"
#include "IMGUICheckBox.h"
#include "IMGUITree.h"
#include "IMGUIInputFloat3.h"
#include "IMGUISameLine.h"
#include "IMGUIDummy.h"

CTransformWidget::CTransformWidget()	:
	m_InheritCheckBox(nullptr),
	m_WorldTree(nullptr),
	m_Transform(nullptr),
	m_RelativeTree(nullptr),
	m_WorldPosInput(nullptr),
	m_WorldRotInput(nullptr),
	m_WorldScaleInput(nullptr),
	m_RelativePosInput(nullptr),
	m_RelativeRotInput(nullptr),
	m_RelativeScaleInput(nullptr)
{
}

CTransformWidget::~CTransformWidget()
{
	// CallBack 삭제
	if (m_Transform)
	{
		m_Transform->DeleteChangePosCallBack(this);
		m_Transform->DeleteChangeRotCallBack(this);
		m_Transform->DeleteChangeScaleCallBack(this);
	}
}

bool CTransformWidget::Init()
{
	CIMGUIWidgetList::Init();

	// AddWidget
	CIMGUIText* Text = AddWidget<CIMGUIText>("Text");
	Text->SetText("Inheritance");

	m_InheritCheckBox = AddWidget<CIMGUICheckBox>("Inherit", 0.f, 0.f);
	m_InheritCheckBox->SetColNum(4);
	m_InheritCheckBox->AddCheckInfo("Scale");
	m_InheritCheckBox->AddCheckInfo("RotX");
	m_InheritCheckBox->AddCheckInfo("RotY");
	m_InheritCheckBox->AddCheckInfo("RotZ");
	m_InheritCheckBox->AddCheckInfo("PosX");
	m_InheritCheckBox->AddCheckInfo("PosY");
	m_InheritCheckBox->AddCheckInfo("PosZ");

	AddWidget<CIMGUIDummy>("Dummy", 0.f, 0.f);

	m_WorldTree = AddWidget<CIMGUITree>("World Transform");
	m_WorldPosInput = m_WorldTree->AddWidget<CIMGUIInputFloat3>("Position", 150.f);
	m_WorldRotInput = m_WorldTree->AddWidget<CIMGUIInputFloat3>("Rotation", 150.f);
	m_WorldScaleInput = m_WorldTree->AddWidget<CIMGUIInputFloat3>("Scale", 150.f);

	m_RelativeTree = AddWidget<CIMGUITree>("Relative Transform");
	m_RelativePosInput = m_RelativeTree->AddWidget<CIMGUIInputFloat3>("Position", 150.f);
	m_RelativeRotInput = m_RelativeTree->AddWidget<CIMGUIInputFloat3>("Rotation", 150.f);
	m_RelativeScaleInput = m_RelativeTree->AddWidget<CIMGUIInputFloat3>("Scale", 150.f);

	// CallBack
	m_InheritCheckBox->SetCallBackIdx(this, &CTransformWidget::OnCheckInherit);
	m_WorldPosInput->SetCallBack(this, &CTransformWidget::OnChangeWorldPos);
	m_WorldRotInput->SetCallBack(this, &CTransformWidget::OnChangeWorldRot);
	m_WorldScaleInput->SetCallBack(this, &CTransformWidget::OnChangeWorldScale);
	m_RelativePosInput->SetCallBack(this, &CTransformWidget::OnChangeRelativePos);
	m_RelativeRotInput->SetCallBack(this, &CTransformWidget::OnChangeRelativeRot);
	m_RelativeScaleInput->SetCallBack(this, &CTransformWidget::OnChangeRelativeScale);

	return true;
}

void CTransformWidget::SetSceneCompoent(CSceneComponent* Component)
{
	m_SceneComponent = Component;
	m_Transform = Component->GetTransform();

	m_InheritCheckBox->SetCheck(0, m_Transform->IsInheritScale());
	m_InheritCheckBox->SetCheck(1, m_Transform->IsInheritRotX());
	m_InheritCheckBox->SetCheck(2, m_Transform->IsInheritRotY());
	m_InheritCheckBox->SetCheck(3, m_Transform->IsInheritRotZ());
	m_InheritCheckBox->SetCheck(4, m_Transform->IsInheritPosX());
	m_InheritCheckBox->SetCheck(5, m_Transform->IsInheritPosY());
	m_InheritCheckBox->SetCheck(6, m_Transform->IsInheritPosZ());

	m_WorldPosInput->SetVal(m_Transform->GetWorldPos());
	m_WorldRotInput->SetVal(m_Transform->GetWorldRot());
	m_WorldScaleInput->SetVal(m_Transform->GetWorldScale());

	m_RelativePosInput->SetVal(m_Transform->GetRelativePos());
	m_RelativeRotInput->SetVal(m_Transform->GetRelativeRot());
	m_RelativeScaleInput->SetVal(m_Transform->GetRelativeScale());

	// Transform 에서 값 변경시 위젯 갱신해주는 콜백 등록
	m_Transform->AddChangePosCallBack(this, &CTransformWidget::OnChangeTransformPos);
	m_Transform->AddChangeRotCallBack(this, &CTransformWidget::OnChangeTransformRot);
	m_Transform->AddChangeScaleCallBack(this, &CTransformWidget::OnChangeTransformScale);

	// Transform이 삭제될 때, 이 위젯이 참조하는 Transform을 nullptr로 만든다.
	m_Transform->AddOnDestroyCallBack(this, &CTransformWidget::OnTransformDestroy);
}

void CTransformWidget::OnChangeTransformScale(const Vector3& WorldScale, const Vector3& RelativeScale)
{
	m_WorldScaleInput->SetVal(WorldScale);
	m_RelativeScaleInput->SetVal(RelativeScale);
}

void CTransformWidget::OnChangeTransformPos(const Vector3& WorldPos, const Vector3& RelativePos)
{
	m_WorldPosInput->SetVal(WorldPos);
	m_RelativePosInput->SetVal(RelativePos);
}

void CTransformWidget::OnChangeTransformRot(const Vector3& WorldRot, const Vector3& RelativeRot)
{
	m_WorldRotInput->SetVal(WorldRot);
	m_RelativeRotInput->SetVal(RelativeRot);
}

void CTransformWidget::OnTransformDestroy()
{
	m_SceneComponent = nullptr;
	m_Transform = nullptr;
}

void CTransformWidget::OnCheckInherit(int Idx, bool Check)
{
	if (m_Transform)
	{
		switch (Idx)
		{
		case 0:
			m_Transform->SetInheritScale(Check);
			break;
		case 1:
			m_Transform->SetInheritRotX(Check);
			break;
		case 2:
			m_Transform->SetInheritRotY(Check);
			break;
		case 3:
			m_Transform->SetInheritRotZ(Check);
			break;
		case 4:
			m_Transform->SetInheritParentRotationPosX(Check);
			break;
		case 5:
			m_Transform->SetInheritParentRotationPosY(Check);
			break;
		case 6:
			m_Transform->SetInheritParentRotationPosZ(Check);
			break;
		}
	}
}

void CTransformWidget::OnChangeWorldPos(const Vector3& Pos)
{
	if (m_Transform)
	{
		m_Transform->SetWorldPos(Pos);
	}
}

void CTransformWidget::OnChangeWorldRot(const Vector3& Rot)
{
	if (m_Transform)
	{
		m_Transform->SetWorldRotation(Rot);
	}
}

void CTransformWidget::OnChangeWorldScale(const Vector3& Scale)
{
	if (m_Transform)
	{
		m_Transform->SetWorldScale(Scale);
	}
}

void CTransformWidget::OnChangeRelativePos(const Vector3& Pos)
{
	if (m_Transform)
	{
		m_Transform->SetRelativePos(Pos);
	}
}

void CTransformWidget::OnChangeRelativeRot(const Vector3& Rot)
{
	if (m_Transform)
	{
		m_Transform->SetRelativeRotation(Rot);
	}
}

void CTransformWidget::OnChangeRelativeScale(const Vector3& Scale)
{
	if (m_Transform)
	{
		m_Transform->SetRelativeScale(Scale);
	}
}
