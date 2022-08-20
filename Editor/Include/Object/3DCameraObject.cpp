
#include "3DCameraObject.h"
#include "Component/CameraComponent.h"
#include "Input.h"
#include "Scene/Scene.h"
#include "Scene/CameraManager.h"

C3DCameraObject::C3DCameraObject()	:
	m_Speed(1.f)
{
	SetTypeID<C3DCameraObject>();
	m_ExcludeSceneSave = true;
}

C3DCameraObject::C3DCameraObject(const C3DCameraObject& obj)	:
	CGameObject(obj)
{
	m_FocusPoint = (CSceneComponent*)FindComponent("Focus");
	m_Camera = (CCameraComponent*)FindComponent("Camera");

	// Ctrl + Mouse L Drag -> 시점 이동 
	CInput::GetInst()->SetCtrlKey("CameraMove", true);
	CInput::GetInst()->SetKeyCallback("CameraMove", KeyState_Down, this, &C3DCameraObject::OnDragMoveStart);
	CInput::GetInst()->SetKeyCallback("CameraMove", KeyState_Push, this, &C3DCameraObject::OnDragMove);
	CInput::GetInst()->SetKeyCallback("CameraMove", KeyState_Up, this, &C3DCameraObject::OnDragMoveEnd);

	// Ctrl + Mouse R  Drag -> 시점 회전
	CInput::GetInst()->SetCtrlKey("CameraRot", true);
	CInput::GetInst()->SetKeyCallback("CameraRot", KeyState_Push, this, &C3DCameraObject::OnDragRotate);
}

C3DCameraObject::~C3DCameraObject()
{
}

bool C3DCameraObject::Init()
{
	CGameObject::Init();

	m_FocusPoint = CreateComponent<CSceneComponent>("Focus");
	m_Camera = CreateComponent<CCameraComponent>("Camera");

	m_FocusPoint->AddChild(m_Camera);
	m_Camera->SetInheritRotX(true);
	m_Camera->SetInheritRotY(true);
	m_Camera->SetInheritRotZ(true);
	m_Camera->SetInheritParentRotationPosX(true);
	m_Camera->SetInheritParentRotationPosY(true);
	m_Camera->SetInheritParentRotationPosZ(true);

	m_Camera->SetCameraType(Camera_Type::Camera3D);
	m_Camera->SetViewAngle(15.f);

	m_Camera->AddRelativePos(0.f, 30.f, -30.f);
	m_Camera->SetRelativeRotationX(45.f);

	// Input Key 설정
	CInput::GetInst()->CreateKey("CameraMove", VK_LBUTTON);
	CInput::GetInst()->CreateKey("CameraRot", VK_RBUTTON);

	// Ctrl + Mouse L Drag -> 시점 이동 
	CInput::GetInst()->SetCtrlKey("CameraMove", true);
	CInput::GetInst()->SetKeyCallback("CameraMove", KeyState_Down, this, &C3DCameraObject::OnDragMoveStart);
	CInput::GetInst()->SetKeyCallback("CameraMove", KeyState_Push, this, &C3DCameraObject::OnDragMove);
	CInput::GetInst()->SetKeyCallback("CameraMove", KeyState_Up, this, &C3DCameraObject::OnDragMoveEnd);

	// Ctrl + Mouse R  Drag -> 시점 회전
	CInput::GetInst()->SetCtrlKey("CameraRot", true);
	CInput::GetInst()->SetKeyCallback("CameraRot", KeyState_Push, this, &C3DCameraObject::OnDragRotate);

	return true;
}

void C3DCameraObject::Start()
{
	CGameObject::Start();

	// Main Camera로 설정
	m_Scene->GetCameraManager()->SetCurrentCamera(m_Camera);
}

void C3DCameraObject::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);

	if (CInput::GetInst()->GetWheelDir())
	{
		Vector3 AxisZ = m_Camera->GetWorldAxis(AXIS::AXIS_Z);
		AxisZ *= CInput::GetInst()->GetWheelDir() * -m_Speed;
		AddWorldPos(AxisZ);
	}
}

void C3DCameraObject::OnDragMoveStart(float DeltaTime)
{
	m_MoveStartCamPos = m_Camera->GetWorldPos();
}

void C3DCameraObject::OnDragMove(float DeltaTime)
{
	Vector3 moveAmount = Vector3(-1.f * CInput::GetInst()->GetMouseMove().x, CInput::GetInst()->GetMouseMove().y, 0.f);

	moveAmount *= m_Speed;

	m_Camera->AddRelativePos(moveAmount);
}

void C3DCameraObject::OnDragMoveEnd(float DeltaTime)
{
	Vector3 CamMove = m_Camera->GetWorldPos() - m_MoveStartCamPos;

	AddWorldPos(CamMove);
	m_Camera->AddWorldPos(CamMove * -1.f);
}

void C3DCameraObject::OnDragRotate(float DeltaTime)
{
	Vector3 moveAmount = Vector3(-1.f * CInput::GetInst()->GetMouseMove().x, CInput::GetInst()->GetMouseMove().y, 0.f);
	
	Vector3 Rotation = Vector3(moveAmount.y, moveAmount.x, 0.f);

	Rotation  *= m_Speed;

	AddWorldRotation(Rotation);
}

void C3DCameraObject::OnZoomIn(float DetlaTime)
{
}

void C3DCameraObject::OnZoomOut(float DetlaTime)
{
}
