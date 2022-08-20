
#include "ObjectHierarchyWindow.h"
#include "SceneComponentHierarchyWindow.h"
#include "IMGUITree.h"
#include "../EditorUtil.h"
#include "IMGUIButton.h"
#include "IMGUITree.h"
#include "IMGUITextInput.h"
#include "../EditorInfo.h"
#include "Flag.h"
#include "IMGUISameLine.h"
#include "IMGUIDummy.h"
#include "IMGUIGizmo.h"
#include "IMGUIGrid.h"
#include "ObjectCreateModal.h"
#include "IMGUIManager.h"
#include "Scene/SceneManager.h"
#include "GameObject/GameObject.h"
#include "InspectorWindow.h"
#include "ToolWindow.h"
#include "ObjectComponentWindow.h"
#include "Component/AnimationMeshComponent.h"
#include "Resource/Animation/SkeletonSokcet.h"

CObjectHierarchyWindow::CObjectHierarchyWindow() :
	m_Root(nullptr),
	m_ObjectDeleteButton(nullptr),
	m_SelectObject(nullptr),
	m_SocketParentObject(nullptr)
{
}

CObjectHierarchyWindow::~CObjectHierarchyWindow()
{
	//size_t Count = m_vecObjectTree.size();

	//for (size_t i = 0; i < Count; ++i)
	//{
	//	SAFE_DELETE(m_vecObjectTree[i]);
	//}
}

CIMGUITree* CObjectHierarchyWindow::GetRoot() const
{
	return m_Root;
}

CGameObject* CObjectHierarchyWindow::GetSelectObject() const
{
	return m_SelectObject;
}

bool CObjectHierarchyWindow::Init()
{
	CIMGUIWindow::Init();

	m_Root = AddWidget<CIMGUITree>("Objects");
	// Object Tree�� RootNode�� Component Tree�� RootNode�� �ڽ� TreeNode�� �����ϱ� �������� ����Ʈ�� ���õ� ���·� �س��´�
	m_Root->SetSelected(true);
	m_SelectNode = m_Root;

	m_ObjectCreatePopUpButton = AddWidget<CIMGUIButton>("Create", 50.f, 20.f);
	m_ObjectCreatePopUpButton->SetClickCallback<CObjectHierarchyWindow>(this, &CObjectHierarchyWindow::OnCreateObjectPopUp);

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");

	m_ObjectDeleteButton = AddWidget<CIMGUIButton>("Delete", 50.f, 20.f);
	m_ObjectDeleteButton->SetClickCallback<CObjectHierarchyWindow>(this, &CObjectHierarchyWindow::OnDeleteObject);

	m_AddAsSocket = AddWidget<CIMGUICheckBox>("Add Child As Socket?");
	m_AddAsSocket->AddCheckInfo("Add Child As Socket?");

	m_SocketName = AddWidget<CIMGUITextInput>("Socket Name", 80.f, 20.f);
	m_SocketName->SetHintText("Enter Socket Name");
	m_SocketName->SetHideName(true);


	m_vecObjectTree.push_back(m_Root);
	return true;
}

void CObjectHierarchyWindow::Update(float DeltaTime)
{
	CIMGUIWindow::Update(DeltaTime);

	//CEditorUtil::ShowDemo();

	//CIMGUITree Tree;

	// �� �����Ӹ��� Tree�� ��ȸ�ϸ鼭 m_SelectNode�� ���� ���õ� ��带 ��������
	//FindSelectNode(m_Root);
}
void CObjectHierarchyWindow::OnRenameObject(const std::string& Name)
{
	m_SelectNode->SetName(Name);
}

void CObjectHierarchyWindow::OnCreateObject(CGameObject* Object)
{
	// Parent�� �ִ�(�ٸ� Object�� �ڽ� Object) Object�� �� Object�� �ֻ��� Object�� OncreateObject�� ȣ��������
	// �ڽĵ��� Tree������ OnCreateObjectRecursive �Լ��� ������ �� �־��ֹǷ� �ֻ��� ��尡 �ƴ� ������ �� �Լ��� �ٷ� ���� ������
	if (Object->GetParentObject())
		return;

	std::string Name = Object->GetName();

	// ��� ����
	CIMGUITree* NewNode = m_Root->AddChild(Name, Object);
	NewNode->SetData(Object);

	// �ݹ� ����
	NewNode->AddSelectCallback(this, &CObjectHierarchyWindow::OnSetSelectNode);
	NewNode->SetDragDropSourceCallback(this, &CObjectHierarchyWindow::OnDragDropSrc);
	NewNode->SetDragDropDestCallback(this, &CObjectHierarchyWindow::OnDragDropDest);

	// ���� ���� ������Ʈ��
	m_SelectObject = Object;
	m_SelectNode = NewNode;

	size_t Count = Object->GetChildObjectCount();

	for (size_t i = 0; i < Count; ++i)
	{
		OnCreateObjectRecursive(Object->GetChildObject((int)i), NewNode);
	}
}

void CObjectHierarchyWindow::OnCreateObjectRecursive(CGameObject* Object, CIMGUITree* ParentTreeNode)
{
	std::string Name = Object->GetName();

	// ��� ����
	CIMGUITree* NewNode = ParentTreeNode->AddChild(Name, Object);
	NewNode->SetData(Object);

	// �ݹ� ����
	NewNode->AddSelectCallback(this, &CObjectHierarchyWindow::OnSetSelectNode);
	NewNode->SetDragDropSourceCallback(this, &CObjectHierarchyWindow::OnDragDropSrc);
	NewNode->SetDragDropDestCallback(this, &CObjectHierarchyWindow::OnDragDropDest);

	// ���� ���� ������Ʈ��
	m_SelectObject = Object;
	m_SelectNode = NewNode;

	size_t Count = Object->GetChildObjectCount();

	for (size_t i = 0; i < Count; ++i)
	{
		OnCreateObjectRecursive(Object->GetChildObject((int)i), NewNode);
	}
}

void CObjectHierarchyWindow::Clear()
{
	m_Root->DeleteHierarchy();
}

void CObjectHierarchyWindow::OnCreateObjectPopUp()
{
	if (!m_ObjectCreateModal)
		m_ObjectCreateModal = AddWidget<CObjectCreateModal>(OBJECTCREATE_POPUPMODAL);

	else
	{
		PopUpModalState State = m_ObjectCreateModal->GetPopUpModalState();

		if (State == PopUpModalState::Closed)
			m_ObjectCreateModal->SetPopUpModalState(PopUpModalState::Open);

		m_ObjectCreateModal->SetRender(true);
	}
}

void CObjectHierarchyWindow::OnDeleteObject()
{
	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);
	CSceneComponentHierarchyWindow* CompWindow = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);

	std::string SelectObjName = Window->GetSelectNode()->GetName();

	CGameObject* Obj = CSceneManager::GetInst()->GetScene()->FindObject(SelectObjName);

	if (!Obj)
		return;

	CSceneComponent* Root = Obj->GetRootComponent();

	

	// GUI�󿡼� ��� ����
	if (Root)
	{
		CompWindow->OnClearComponents(Root->GetName());
	}


	// ���� �󿡼� Object ���� ���� ������ ����
	Obj->DeleteObj();

	m_SelectNode->Delete();

	// Inspector���� ����
	CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);
	Inspector->OnDeleteGameObject();
}

//void CObjectHierarchyWindow::FindSelectNode(CIMGUITree* RootNode)
//{
//	if (!RootNode)
//		return;
//
//	if (RootNode->IsSelected())
//	{
//		m_SelectNode = RootNode;
//		return;
//	}
//
//	else
//	{
//		size_t Count = RootNode->GetChildCount();
//
//		if(Count > 0)
//		{
//			for (size_t i = 0; i < Count; ++i)
//				FindSelectNode(RootNode->GetNode((int)i));
//		}
//	}
//}

CIMGUITree* CObjectHierarchyWindow::GetSelectNode() const
{
	return m_SelectNode;
}

void CObjectHierarchyWindow::OnSetSelectNode(CIMGUITree* SelectNode)
{
	m_SelectNode = SelectNode;
	m_SelectObject = CSceneManager::GetInst()->GetScene()->FindObject(m_SelectNode->GetName());

	// Window ����
	CToolWindow* ToolWindow = (CToolWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(TOOL);
	CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);
	CSceneComponentHierarchyWindow* SceneCompHierachy = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
	CObjectComponentWindow* ObjectCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);

	if (ToolWindow)
	{
		ToolWindow->SetGizmoObject(m_SelectObject);
	}
	if (Inspector)
	{
		Inspector->OnSelectGameObject(m_SelectObject);
	}
	if (SceneCompHierachy)
	{
		SceneCompHierachy->OnUpdateSceneComponentWindow(m_SelectObject);
	}
	if (ObjectCompWindow)
	{
		ObjectCompWindow->OnRefreshObjectComponentList(m_SelectObject);
	}
}

void CObjectHierarchyWindow::OnDragDropSrc(CIMGUITree* SrcTree)
{
	if (m_SelectNode == m_Root || m_DragSrc == m_Root)
		return;

	m_DragSrc = SrcTree;
}

void CObjectHierarchyWindow::OnDragDropDest(CIMGUITree* DestTree, const std::string& ParentName, const std::string& NewChildName)
{
	if (m_SelectNode == m_Root)
		return;

	m_DragDest = DestTree;

	bool CheckSocket = m_AddAsSocket->GetCheck(0);

	if (CheckSocket)
	{
		CScene* CurrentScene = CSceneManager::GetInst()->GetScene();

		CGameObject* SrcObj = CurrentScene->FindObject(m_DragSrc->GetName());
		CGameObject* DestObj = CurrentScene->FindObject(m_DragDest->GetName());

		if (m_DragDest->GetData() == DestObj && m_DragSrc->GetData() == SrcObj)
		{
			std::string SocketName = m_SocketName->GetTextMultibyte();

			CAnimationMeshComponent* Comp = DestObj->FindComponentFromType<CAnimationMeshComponent>();

			if (!Comp)
				return;

			CSkeleton* Skeleton = Comp->GetAnimationInstance()->GetSkeleton();

			CSkeletonSocket* Socket = Skeleton->GetSocket(SocketName);

			if (!Socket)
			{
				MessageBox(nullptr, TEXT("�ش� �̸��� ������ �����ϴ�"), TEXT("Error"), MB_OK);
				return;
			}

			DestObj->AddChildObject(SrcObj, Socket->GetName());

			// ���� �ȿ��� CSceneComponent::AddChild�� ȣ���ؼ� DestObj�� Comp�� Child Component�� SrcObj�� RootComponent�� ���� ���̹Ƿ�
			// �ϴ� SccneComponentHierarychy Window GUI�󿡼��� ����� ���ο� �θ�(DestObj�� RootComponent)�� �߰����ִ� �۾��� ����� �Ѵ�
			CSceneComponentHierarchyWindow* Window = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);

			Window->DeleteTreeNode(SrcObj->GetRootComponent());
			CIMGUITree* DestSceneCompTreeNode = Window->FindTreeNode(DestObj->GetRootComponent());

			Window->AddTreeNode(SrcObj->GetRootComponent(), DestSceneCompTreeNode);

			MessageBox(nullptr, TEXT("���Ͽ� �߰� ����"), TEXT("Warning"), MB_OK);
		}
	}

	else
	{
		if (m_DragDest && m_DragSrc)
		{
			CScene* CurrentScene = CSceneManager::GetInst()->GetScene();

			CGameObject* SrcObj = CurrentScene->FindObject(m_DragSrc->GetName());
			CGameObject* DestObj = CurrentScene->FindObject(m_DragDest->GetName());

			SrcObj->ClearParent();

			DestObj->AddChildObject(SrcObj);

			// SceneComponent Hierarychy Window, ObjectComponent List GUI�󿡼��� �߰�������Ѵ�
		}
	}
}

CGameObject* CObjectHierarchyWindow::GetSelectGameObject()
{
	return m_SelectObject;
}

