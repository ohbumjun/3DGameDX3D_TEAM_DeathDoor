#include "DDMouseWidgetWindow.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Component/CameraComponent.h"
#include "Widget/Image.h"
#include "Device.h"
#include "Scene/SceneManager.h"

CDDMouseWidgetWindow::CDDMouseWidgetWindow()
{
	SetTypeID<CDDMouseWidgetWindow>();
}

CDDMouseWidgetWindow::~CDDMouseWidgetWindow()
{
}

bool CDDMouseWidgetWindow::Init()
{
	m_Image = CreateWidget<CImage>("Mouse");
	m_Image->SetTexture("DDMouseUI", TEXT("Mouse/Cursor.png"));

	return true;
}

void CDDMouseWidgetWindow::Render()
{
	CScene* Scene = CSceneManager::GetInst()->GetScene();
	CGameObject* PlayerObj = Scene->GetPlayerObject();

	if (PlayerObj)
	{
		if (Scene)
		{
			CCameraComponent* CurCam = Scene->GetCameraManager()->GetCurrentCamera();

			Matrix matView = CurCam->GetViewMatrix();
			Matrix matProj = CurCam->GetProjMatrix();

			Matrix matVP = matView * matProj;
			Vector3 PlayerPos = PlayerObj->GetWorldPos();

			// �÷��̾��� ȭ����������� ��ġ�� ���Ѵ�.
			Vector3 PlayerSceenPos = PlayerPos.TransformCoord(matVP);
			PlayerSceenPos.x = PlayerSceenPos.x / PlayerSceenPos.z;
			PlayerSceenPos.y = PlayerSceenPos.y / PlayerSceenPos.z;
			PlayerSceenPos.x = PlayerSceenPos.x * 0.5f + 0.5f;
			PlayerSceenPos.y = PlayerSceenPos.y * -0.5f + 0.5f;

			Resolution RS = CDevice::GetInst()->GetResolution();
			PlayerSceenPos.x *= (float)RS.Width;
			PlayerSceenPos.y *= (float)RS.Height;
			PlayerSceenPos.z = 0.f;

			// �÷��̾��� ȭ������� ��ġ�κ��� ���콺�� ���ϴ� ���͸� ���Ѵ�.
			Vector3 ToMouse = Vector3(m_Pos.x, m_Pos.y, 0.f) - PlayerSceenPos;
			ToMouse.Normalize();

			// (0,1,0)�� �̷�� ������ ȸ�������� �Ѵ�.
			float Angle = Vector3::Axis[AXIS::AXIS_Y].Angle(ToMouse);

			if (ToMouse.x < 0)
			{
				m_Image->SetAngle(Angle);
			}
			else
			{
				m_Image->SetAngle(-Angle);
			}
		}
	}

	CWidgetWindow::Render();
}
