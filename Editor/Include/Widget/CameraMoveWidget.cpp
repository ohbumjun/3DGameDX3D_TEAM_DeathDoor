#include "CameraMoveWidget.h"
#include "GameObject/GameObject.h"
#include "Component/CameraComponent.h"
#include "CameraWidget.h"
#include "IMGUIInputFloat3.h"
#include "IMGUIInputFloat.h"
#include "IMGUITextInput.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"

CCameraMoveWidget::CCameraMoveWidget()
{
}

CCameraMoveWidget::~CCameraMoveWidget()
{
}

bool CCameraMoveWidget::Init()
{
	CIMGUIWidget::Init();

	m_DestPos = AddWidget<CIMGUIInputFloat3>("Dest Point", 200.f);
	m_DestReachTime = AddWidget<CIMGUIInputFloat>("Reach Time", 200.f);
	m_StayTime = AddWidget<CIMGUIInputFloat>("Stay Time", 200.f);
	m_Delete = AddWidget<CIMGUIButton>("Delete", 0.f, 0.f);
	AddWidget<CIMGUISeperator>("Sep");

	m_DestPos->SetCallBack(this, &CCameraMoveWidget::OnChangeDestPos);
	m_DestReachTime->SetCallBack(this, &CCameraMoveWidget::OnChangeDestReachTime);
	m_StayTime->SetCallBack(this, &CCameraMoveWidget::OnChangeStayTime);
	m_Delete->SetClickCallback(this, &CCameraMoveWidget::OnClickDelete);

	return true;
}

void CCameraMoveWidget::SetCamMoveData(CamMoveData* Data)
{
	m_MoveData = Data;

	m_DestPos->SetVal(m_MoveData->DestPoint);
	m_DestReachTime->SetVal(m_MoveData->NextPointReachTime);
	m_StayTime->SetVal(m_MoveData->StayTime);
}

void CCameraMoveWidget::OnChangeDestPos(const Vector3& Pos)
{
	m_MoveData->DestPoint = Pos;
}

void CCameraMoveWidget::OnChangeDestReachTime(float Val)
{
	m_MoveData->NextPointReachTime = Val;
}

void CCameraMoveWidget::OnChangeStayTime(float Val)
{
	m_MoveData->StayTime = Val;
}

void CCameraMoveWidget::OnClickDelete()
{
	m_CamWidget->OnDeleteMoveData(m_MoveData, this);
}
