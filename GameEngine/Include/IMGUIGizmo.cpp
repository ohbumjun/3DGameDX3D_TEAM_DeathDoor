#include "IMGUIGizmo.h"
#include "ImGuizmo.h"
#include "Scene/SceneManager.h"
#include "Scene/CameraManager.h"
#include "Device.h"
#include "Component/CameraComponent.h"

CIMGUIGizmo::CIMGUIGizmo() :
	m_Component(nullptr),
	m_Using(false)
{
	m_Operation = ImGuizmo::OPERATION::TRANSLATE;
	m_Mode = ImGuizmo::MODE::WORLD;
	m_matWorld.Identity();
}

CIMGUIGizmo::~CIMGUIGizmo()
{
}

bool CIMGUIGizmo::Init()
{
	CIMGUIWidget::Init();

	return true;
}

void CIMGUIGizmo::Render()
{
	CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	Matrix matScale, matRot, matTrans;

	matScale.Identity();
	matRot.Identity();
	matTrans.Identity();

	m_matWorld = matScale * matRot * matTrans;

	if (m_Component)
	{
		m_matWorld = m_Component->GetWorldMatrix();
	}

	Matrix matView, matProj;

	matView = Camera->GetViewMatrix();
	matProj = Camera->GetProjMatrix();

 	ImGuizmo::Manipulate((float*)&matView, (float*)&matProj, m_Operation,
 		m_Mode, (float*)&m_matWorld);

	if (ImGuizmo::IsUsing() && m_Component)
	{
		// Gizmo�� ��� ���϶��� Transform�� World Matrix�� ���� �ִ� ������� �����Ѵ�.
		m_Component->SetTransformByWorldMatrix(m_matWorld);
		m_Using = true;
	}
	else
	{
		if (m_Using)
		{
			// Gizmo����� ������, �ٽ� Scale, Pos, Rot ������ World Matirx�� �����ϰ� �������´�.
			m_Component->SetUpdateByMat(false);
			m_Component->DecomposeWorld();
			m_Using = false;
		}
	}
}

void CIMGUIGizmo::SetGameObject(CGameObject* Object)
{
	if (!Object)
	{
		SetIdentity();
		return;
	}
	else if (!Object->GetRootComponent())
	{
		SetIdentity();
		return;
	}

	CSceneComponent* Root = Object->GetRootComponent();

	if (Root != m_Component)
	{
		Root->AddOnDestoryCallBack(this, &CIMGUIGizmo::OnDestroyComponent);
	}

	m_Component = Root;
}

void CIMGUIGizmo::SetComponent(CSceneComponent* Component)
{
	if (!Component)
	{
		SetIdentity();
	}

	if (Component != m_Component)
	{
		Component->AddOnDestoryCallBack(this, &CIMGUIGizmo::OnDestroyComponent);
	}

	m_Component = Component;
}

void CIMGUIGizmo::OnDestroyComponent(CSceneComponent* Component)
{
	if (m_Component == Component)
	{
		m_Component = nullptr;
	}
}

