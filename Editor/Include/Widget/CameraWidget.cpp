#include "CameraWidget.h"
#include "IMGUIText.h"
#include "IMGUIInputFloat.h"
#include "IMGUISeperator.h"
#include "IMGUISameLine.h"
#include "IMGUICheckBox.h"
#include "IMGUIButton.h"
#include "IMGUITree.h"
#include "CameraMoveWidget.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/CameraManager.h"

CCameraWidget::CCameraWidget()	:
	m_IsMoveEditMode(false)
{
}

CCameraWidget::~CCameraWidget()
{
}

bool CCameraWidget::Init()
{
	CSceneComponentWidget::Init();

	m_ComponentTypeText->SetText("Camera Component");

	CIMGUITree* Tree = AddWidget<CIMGUITree>("Camrera Variables", 200.f);

	m_ViewAngle = Tree->AddWidget<CIMGUIInputFloat>("ViewAngle", 200.f);
	m_SetCurrent = Tree->AddWidget<CIMGUIButton>("Set Current", 0.f, 0.f);
	m_ShakeTime = Tree->AddWidget<CIMGUIInputFloat>("Shake Time", 200.f);
	m_ShakeAmount = Tree->AddWidget<CIMGUIInputFloat>("Shake Amount", 200.f);
	m_Shake = Tree->AddWidget<CIMGUIButton>("Shake Test", 0.f, 0.f);

	Tree = AddWidget<CIMGUITree>("Camera Move Data", 200.f);
	m_MoveDataEditMode = Tree->AddWidget<CIMGUICheckBox>("EditMode", 200.f);
	m_AddMoveData = Tree->AddWidget<CIMGUIButton>("Add MoveData", 0.f, 0.f);
	m_MoveDataList = Tree->AddWidget<CIMGUIWidgetList>("Move Data List", 200.f, 200.f);
	m_MoveTest = Tree->AddWidget<CIMGUIButton>("Move Test", 0.f, 0.f);
	m_ClearMoveData = Tree->AddWidget<CIMGUIButton>("Data Clear", 0.f, 0.f);
	AddWidget<CIMGUISeperator>("Sep");

	m_MoveDataEditMode->AddCheckInfo("Move Data Edit Mode");

	m_MoveDataEditMode->SetCallBackIdx(this, &CCameraWidget::OnCheckMoveDataEditMode);
	m_ViewAngle->SetCallBack(this, &CCameraWidget::OnChangeViewAngle);
	m_SetCurrent->SetClickCallback(this, &CCameraWidget::OnClickSetCurrent);
	m_Shake->SetClickCallback(this, &CCameraWidget::OnClickShake);
	m_AddMoveData->SetClickCallback(this, &CCameraWidget::OnClickAddMoveData);
#ifdef _DEBUG
	m_MoveTest->SetClickCallback(this, &CCameraWidget::OnClickTestMove);
#endif // DEBUG
	m_ClearMoveData->SetClickCallback(this, &CCameraWidget::OnClickClearMove);

	return true;
}

void CCameraWidget::SetSceneComponent(CSceneComponent* Com)
{
	CSceneComponentWidget::SetSceneComponent(Com);

	CCameraComponent* CamCom = dynamic_cast<CCameraComponent*>(m_Component);

	float ViewAngle = CamCom->GetViewAngle();

	m_ViewAngle->SetVal(ViewAngle);

	std::list<CamMoveData*> MoveDataList = CamCom->GetMoveData();

	auto iter = MoveDataList.begin();
	auto iterEnd = MoveDataList.end();

	for (; iter != iterEnd; ++iter)
	{
		CCameraMoveWidget* Widget = m_MoveDataList->AddWidget<CCameraMoveWidget>("MoveWidget");
		Widget->SetCameraComponent(CamCom);
		Widget->SetCamMoveData(*iter);
		Widget->SetCameraWidget(this);
	}
}

void CCameraWidget::OnChangeViewAngle(float Val)
{
	CCameraComponent* CamCom = dynamic_cast<CCameraComponent*>(m_Component);

	CamCom->SetViewAngle(Val);
}

void CCameraWidget::OnClickSetCurrent()
{
	CCameraManager* CamMgr = CSceneManager::GetInst()->GetScene()->GetCameraManager();

	CCameraComponent* CurCam = CamMgr->GetCurrentCamera();

	if (CurCam == (CCameraComponent*)m_Component)
	{
		CamMgr->ReturnCamera();
		return;
	}
	else
	{
		CamMgr->KeepCamera();
		CamMgr->SetCurrentCamera((CCameraComponent*)m_Component);
	}
}

void CCameraWidget::OnClickShake()
{
	CCameraManager* CamMgr = CSceneManager::GetInst()->GetScene()->GetCameraManager();

	CCameraComponent* CurCam = CamMgr->GetCurrentCamera();

	if (CurCam != (CCameraComponent*)m_Component)
	{
		MessageBox(nullptr, TEXT("현재 카메라가 아님"), TEXT("Warning"), MB_OK);
		return;
	}

	float Time = m_ShakeTime->GetVal();
	float Amount = m_ShakeAmount->GetVal();

	if (Time * Amount < 0)
	{
		MessageBox(nullptr, TEXT("잘못된 입력값"), TEXT("Warning"), MB_OK);
		return;
	}

	CurCam->Shake(Time, Amount);
}

void CCameraWidget::OnCheckMoveDataEditMode(int Idx, bool Enable)
{
	m_IsMoveEditMode = Enable;

	if (Enable)
	{
		m_OriginCamPosRelative = m_Component->GetRelativePos();
	}
	else
	{
		m_Component->SetRelativePos(m_OriginCamPosRelative);
	}
}

void CCameraWidget::OnClickAddMoveData()
{
	if (!m_IsMoveEditMode)
	{
		MessageBox(nullptr, TEXT("카메라 경로 입력 모드가 아님"), TEXT("Failed"), MB_OK);
	}

	CCameraComponent* CamCom = dynamic_cast<CCameraComponent*>(m_Component);

	Vector3 CurCamPos = CamCom->GetWorldPos();
	CamMoveData* Data = CamCom->AddCutSceneMoveData(CurCamPos, 1.f);

	CCameraMoveWidget* MoveWidget = m_MoveDataList->AddWidget<CCameraMoveWidget>("MoveData");
	MoveWidget->SetCameraComponent(CamCom);
	MoveWidget->SetCamMoveData(Data);
	MoveWidget->SetCameraWidget(this);
}
#ifdef _DEBUG
void CCameraWidget::OnClickTestMove()
{
	CCameraComponent* CamCom = dynamic_cast<CCameraComponent*>(m_Component);

	if (!m_IsMoveEditMode)
	{
		CamCom->StartTestMove();
	}
}
#endif // DEBUG

void CCameraWidget::OnClickClearMove()
{
	CCameraComponent* CamCom = dynamic_cast<CCameraComponent*>(m_Component);

	CamCom->ClearCutSceneMoveData();
	m_MoveDataList->ClearWidget();
}

void CCameraWidget::OnDeleteMoveData(CamMoveData* Data, CCameraMoveWidget* DeleteWidget)
{
	CCameraComponent* CamCom = dynamic_cast<CCameraComponent*>(m_Component);

	CamCom->DeleteCutSceneMoveData(Data);
	m_MoveDataList->DeleteWidget(DeleteWidget);
}

