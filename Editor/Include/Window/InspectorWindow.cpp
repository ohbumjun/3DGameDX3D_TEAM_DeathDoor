#include "InspectorWindow.h"
#include "../Widget/GameObjectWidget.h"
#include "IMGUISeperator.h"
#include "IMGUIText.h"

CInspectorWindow::CInspectorWindow()	:
	m_ObjWidget(nullptr)
{
}

CInspectorWindow::~CInspectorWindow()
{
}

bool CInspectorWindow::Init()
{
	CIMGUIWindow::Init();

	CIMGUIText* Text = AddWidget<CIMGUIText>("Text");
	Text->SetText("Inspector");
	AddWidget<CIMGUISeperator>("Sep");

	m_ObjWidget = AddWidget<CGameObjectWidget>("GameObjectInspector");
	m_ObjWidget->SetRender(false);

	return true;
}

void CInspectorWindow::OnSelectGameObject(CGameObject* Obj)
{
	// ���� ���õ� ���ӿ�����Ʈ�� �ٽ� ���õ� ���
	CGameObject* CurObject = m_ObjWidget->GetGameObject();

	if (CurObject)
	{
		if (m_ObjWidget->GetGameObject() == Obj)
		{
			return;
		}
	}

	// Object Widget�� GameObject �����ϸ鼭 Inspector ���� 
	m_ObjWidget->SetRender(true);
	m_ObjWidget->ClearComponentWidget();
	m_ObjWidget->SetGameObject(Obj);
}

void CInspectorWindow::OnCreateSceneComponent(CSceneComponent* Com)
{
	if (!Com)
	{
		return;
	}

	// �ߺ� ���� (OBJ)
	if (FindSceneComponent(Com))
		return;

	m_ObjWidget->CreateSceneComponentWidget(Com);
}

void CInspectorWindow::OnCreateObjectComponent(CObjectComponent* Com)
{
	if (!Com)
	{
		return;
	}

	m_ObjWidget->CreateObjectComponentWidget(Com);
}

bool CInspectorWindow::FindSceneComponent(CSceneComponent* Com)
{
	return m_ObjWidget->FindSceneComponent(Com);
}
void CInspectorWindow::OnDeleteSceneComponent(CSceneComponent* Com)
{
	m_ObjWidget->DeleteSceneComponentWidget(Com);
}

void CInspectorWindow::OnDeleteGameObject()
{
	m_ObjWidget->ClearComponentWidget();
	m_ObjWidget->SetRender(false);
}
