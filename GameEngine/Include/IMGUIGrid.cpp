#include "IMGUIGrid.h"
#include "ImGuizmo.h"
#include "Scene/SceneManager.h"
#include "Scene/CameraManager.h"
#include "Device.h"
#include "Component/CameraComponent.h"

CIMGUIGrid::CIMGUIGrid()
{
	m_matIdentity.Identity();
}

CIMGUIGrid::~CIMGUIGrid()
{
}

bool CIMGUIGrid::Init()
{
	CIMGUIWidget::Init();

	return true;
}

void CIMGUIGrid::Render()
{
	CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	Matrix matView, matProj;
	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

	RECT Rect;
	GetWindowRect(CEngine::GetInst()->GetWindowHandle(), &Rect);

	Resolution Res = CDevice::GetInst()->GetResolution();
	Vector2 Ratio = CDevice::GetInst()->GetViewportAspectRatio();
	Res.Width *= (unsigned int)Ratio.x;
	Res.Height *= (unsigned int)Ratio.y;

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect((float)Rect.left, (float)Rect.top,(float)Res.Width, (float)Res.Height);
	ImGuizmo::DrawGrid((float*)&matView, (float*)&matProj, (float*)&m_matIdentity, m_Size.x);
}
