#include "IMGUISetRect.h"
#include "Device.h"
#include "ImGuizmo.h"
#include "Engine.h"

CIMGUISetRect::CIMGUISetRect()
{
}

CIMGUISetRect::~CIMGUISetRect()
{
}

bool CIMGUISetRect::Init()
{
	return true;
}

void CIMGUISetRect::Render()
{
	RECT Rect;
	GetWindowRect(CEngine::GetInst()->GetWindowHandle(), &Rect);

	Resolution Res = CDevice::GetInst()->GetResolution();
	Vector2 Ratio = CDevice::GetInst()->GetViewportAspectRatio();
	Res.Width *= (unsigned int)Ratio.x;
	Res.Height *= (unsigned int)Ratio.y;

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect((float)Rect.left, (float)Rect.top, (float)Res.Width, (float)Res.Height);
}
