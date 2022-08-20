
#include "SceneComponentHierarchyWindow.h"
#include "IMGUITree.h"
#include "IMGUIPopUpModal.h"
#include "IMGUIComboBox.h"
#include "IMGUIButton.h"
#include "IMGUITextInput.h"
#include "IMGUISameLine.h"
#include "IMGUIDummy.h"
#include "../EditorUtil.h"
#include "Flag.h"
#include "SceneComponentCreateModal.h"
#include "IMGUIManager.h"
#include "ObjectHierarchyWindow.h"
#include "ObjectComponentWindow.h"
#include "ToolWindow.h"
#include "Scene/SceneManager.h"
#include "InspectorWindow.h"
#include "Engine.h"
#include "PathManager.h"

CSceneComponentHierarchyWindow::CSceneComponentHierarchyWindow() :
	m_Root(nullptr),
	m_ComponentCreatePopUpButton(nullptr),
	m_ComponentCreateModal(nullptr),
	m_DragSrc(nullptr),
	m_DragDest(nullptr),
	m_ComponentDeleteButton(nullptr)
{
}

CSceneComponentHierarchyWindow::~CSceneComponentHierarchyWindow()
{
	//size_t Count = m_vecSceneComponentTree.size();

	//for (size_t i = 0; i < Count; ++i)
	//{
	//	SAFE_DELETE(m_vecSceneComponentTree[i]);
	//}
}

CIMGUITree* CSceneComponentHierarchyWindow::GetRoot() const
{
	return m_Root;
}

CIMGUITree* CSceneComponentHierarchyWindow::GetDragSrc() const
{
	return m_DragSrc;
}

CIMGUITree* CSceneComponentHierarchyWindow::GetDragDest() const
{
	return m_DragDest;
}

void CSceneComponentHierarchyWindow::SetDragSrc(CIMGUITree* DragSrc)
{
	m_DragSrc = DragSrc;
}

void CSceneComponentHierarchyWindow::SetDragDest(CIMGUITree* DragDest)
{
	m_DragDest = DragDest;
}

void CSceneComponentHierarchyWindow::ClearExistingHierarchy()
{
	m_Root->DeleteHierarchy();
}

bool CSceneComponentHierarchyWindow::Init()
{
	CIMGUIWindow::Init();

	m_Root = AddWidget<CIMGUITree>("SceneComponents");
	m_Root->SetSelected(true);
	m_SelectNode = m_Root;

	m_ComponentCreatePopUpButton = AddWidget<CIMGUIButton>("Create", 50.f, 20.f);
	m_ComponentCreatePopUpButton->SetClickCallback<CSceneComponentHierarchyWindow>(this, &CSceneComponentHierarchyWindow::OnCreateComponentPopUp);

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");

	m_ComponentDeleteButton = AddWidget<CIMGUIButton>("Delete", 50.f, 20.f);
	m_ComponentDeleteButton->SetClickCallback<CSceneComponentHierarchyWindow>(this, &CSceneComponentHierarchyWindow::OnDeleteComponent);

	m_vecComponentTree.push_back(m_Root);

	return true;
}

void CSceneComponentHierarchyWindow::Update(float DeltaTime)
{
	CIMGUIWindow::Update(DeltaTime);

	// �� �����Ӹ��� Tree�� ��ȸ�ϸ鼭 m_SelectNode�� ���� ���õ� ��带 ��������
	//FindSelectNode(m_Root);
}

bool CSceneComponentHierarchyWindow::DeleteTreeNode(CSceneComponent* Comp, CIMGUITree* ParentNode)
{
	if (!ParentNode)
		ParentNode = m_Root;

	CIMGUITree* Child = ParentNode->FindChild(Comp->GetName(), Comp);

	if (Child)
	{
		ParentNode->DeleteChild(Child->GetName());
		return true;
	}

	return false;
}

bool CSceneComponentHierarchyWindow::AddTreeNode(CSceneComponent* Comp, CIMGUITree* ParentNode)
{
	if (!ParentNode)
		ParentNode = m_Root;

	CIMGUITree* Child = ParentNode->FindChild(Comp->GetName(), Comp);

	if (Child)
		return false;

	else
	{
		ParentNode->AddChild(Comp->GetName(), Comp);
		return true;
	}
}

CIMGUITree* CSceneComponentHierarchyWindow::FindTreeNode(CSceneComponent* Comp)
{
	if (m_Root->GetName() == Comp->GetName() && m_Root->GetData() == Comp)
	{
		return m_Root;
	}

	CIMGUITree* TreeNode = m_Root->FindChild(Comp->GetName(), Comp);

	if (TreeNode)
		return TreeNode;

	else
		return nullptr;
}

void CSceneComponentHierarchyWindow::OnRenameComponent(const std::string& NewName, const std::string& PrevName)
{
	CIMGUITree* Node = m_Root->FindChild(PrevName);
	Node->SetName(NewName);
}

void CSceneComponentHierarchyWindow::OnCreateComponentPopUp()
{
	if (!m_ComponentCreateModal)
		m_ComponentCreateModal = AddWidget<CSceneComponentCreateModal>(SCENECOMPONENT_CREATE_POPUPMODAL);

	else
	{
		PopUpModalState State = m_ComponentCreateModal->GetPopUpModalState();

		if (State == PopUpModalState::Closed)
			m_ComponentCreateModal->SetPopUpModalState(PopUpModalState::Open);

		m_ComponentCreateModal->SetRender(true);
	}
}

void CSceneComponentHierarchyWindow::OnDragDropSrc(CIMGUITree* SrcTree)
{
	if (m_SelectNode == m_Root || m_DragSrc == m_Root)
		return;

	m_DragSrc = SrcTree;

	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (Window)
	{
		std::string ObjName = Window->GetSelectNode()->GetName();

		CGameObject* Obj = CSceneManager::GetInst()->GetScene()->FindObject(ObjName);

		if (Obj)
		{
			// ObjectHierarchy Window���� ã�� Object������ SceneComponent Hierarchy Window���� Drag Drop�� Src�� �ش��ϴ� Component�� ã��
			CSceneComponent * Comp = (CSceneComponent*)Obj->FindComponent(m_DragSrc->GetName());

			// Comp->ClearParent();
		}
	}
}

void CSceneComponentHierarchyWindow::OnDragDropDest(CIMGUITree* DestTree, const std::string& ParentName, const std::string& NewChildName)
{
	if (m_SelectNode == m_Root)
		return;

	m_DragDest = DestTree;

	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (Window)
	{
		std::string ObjName = Window->GetSelectNode()->GetName();

		CGameObject* Obj = CSceneManager::GetInst()->GetScene()->FindObject(ObjName);

		if (Obj)
		{
			CSceneComponent* ParentComponent = (CSceneComponent*)Obj->FindComponent(ParentName);
			CSharedPtr<CSceneComponent> ChildComponent = (CSceneComponent*)Obj->FindComponent(NewChildName);

			// ���� Child Component�� �θ𿡰� �� ������Ʈ�� �ڽĿ��� �����϶�� �˸�
			CSceneComponent* OriginChildRoot = ChildComponent->GetParent();

			if (OriginChildRoot)
			{
				OriginChildRoot->DeleteChild(ChildComponent);
			}

			ParentComponent->AddChild(ChildComponent);
		}
	}
}

void CSceneComponentHierarchyWindow::OnUpdateSceneComponentWindow(CGameObject* Object)
{
	if (!Object)
		return;

	DisableCurrent();
	
	CSceneComponent* RootComp = Object->GetRootComponent();

	if (!RootComp)
		return;

	std::string Name = RootComp->GetName();
	CIMGUITree* RootTreeNode = m_Root->FindChild(Name, Object->GetRootComponent());
	RootTreeNode->EnableAll();
}

void CSceneComponentHierarchyWindow::OnCreateComponent(bool IsRoot, CSceneComponent* Component)
{
	if (!Component)
	{
		return;
	}

	std::string Name = Component->GetName();

	CIMGUITree* NewNode = nullptr;

	if (IsRoot)
	{
		NewNode = m_Root->AddChild(Name);
		NewNode->SetData(Component);
	}

	else
	{
		// Component�� �θ� Component�� IMGUITree�󿡼� ã�Ƽ� �� Tree�� ChildNode�� �־������
		CIMGUITree* RootCompNode = m_Root->FindChild(Component->GetParent()->GetName(), Component->GetParent());
		NewNode = RootCompNode->AddChild(Name, Component);
	}

	NewNode->AddSelectCallback(this, &CSceneComponentHierarchyWindow::OnSetSelectNode);
	NewNode->SetDragDropSourceCallback(this, &CSceneComponentHierarchyWindow::OnDragDropSrc);
	NewNode->SetDragDropDestCallback(this, &CSceneComponentHierarchyWindow::OnDragDropDest);
}

void CSceneComponentHierarchyWindow::OnLoadGameObject(CGameObject* Object)
{
	if (!Object || !Object->GetRootComponent())
	{
		return;
	}

	// ���� Hierachy ��Ȱ��ȭ
	DisableCurrent();

	CSceneComponent* Root = Object->GetRootComponent();
	std::string RootName = Root->GetName();

	// Hierachy ����
	CIMGUITree* RootComponentNode = m_Root->AddChild(RootName, Root);
	MakeHierachyRecursive(Root, RootComponentNode);

	size_t Count = Object->GetChildObjectCount();

	for (size_t i = 0; i < Count; ++i)
	{
		OnLoadGameObject(Object->GetChildObject((int)i));
	}
}

void CSceneComponentHierarchyWindow::OnClearComponents(const std::string& RootComponentName)
{
	size_t Count = m_Root->GetChildCount();

	for (size_t i = 0; i < Count; ++i)
	{
		CIMGUITree* RootComponent = m_Root->FindChild(RootComponentName);

		if (RootComponent && RootComponent->IsEnable())
		{
			RootComponent->Delete();
			break;
		}
	}

	//CObjectHierarchyWindow* ObjWindow = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	//CGameObject* SelectObj = ObjWindow->GetSelectObject();

	//for (size_t i = 0; i < Count;)
	//{
	//	CIMGUITree* Node = m_Root->GetNode(i);

	//	if (Node)
	//	{
	//		CComponent* Comp = SelectObj->FindComponent(Node->GetName());

	//		if (Comp)
	//		{
	//			Node->Delete();
	//			Count = m_Root->GetChildCount();
	//		}

	//		else
	//			++i;
	//	}

	//	else
	//		++i;
	//}

	CObjectComponentWindow* ObjCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);

	if (ObjCompWindow)
		ObjCompWindow->ClearListBox();
}

void CSceneComponentHierarchyWindow::OnSetSelectNode(CIMGUITree* Tree)
{
	m_SelectNode = Tree;

	// Gizmo ���� ���õ� ������Ʈ ������Ʈ
	CSceneComponent* Comp = (CSceneComponent*)FindSelectComponent();

	((CToolWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(TOOL))->SetGizmoComponent(Comp);
}

void CSceneComponentHierarchyWindow::OnDeleteComponent()
{
	CSceneComponent* DeleteComp = (CSceneComponent*)FindSelectComponent();

	if (!DeleteComp)
		return;

	CGameObject* Object = DeleteComp->GetGameObject();

	// Inspector���� Widget����
	((CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR))->OnDeleteSceneComponent(DeleteComp);

	// ��Ʈ ���� ���� ����, �߰��� �ִ� ���� �� ��� �����ϰ� ù��° �ڽ� Component�� �ø��� ������ GUI�󿡼��� ����
	m_SelectNode->Delete();

	CSceneComponent* NewRoot = nullptr;
	if (DeleteComp)
	{
		NewRoot = DeleteComp->GetChild(0);
		DeleteComp->DeleteComponent();
	}
}

void CSceneComponentHierarchyWindow::OnSaveComponent()
{
	CComponent* SaveComponent = FindSelectComponent();

	if (!SaveComponent)
	{
		MessageBox(nullptr, TEXT("���õ� ������Ʈ ����"), TEXT("Error"), MB_OK);
		return;
	}

	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All File\0*.*\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(COMPONENT_PATH)->Path;

	if (GetSaveFileName(&OpenFile) != 0)
	{
		char FullPath[MAX_PATH] = {};

		int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FullPath, Length, 0, 0);

		CGameObject* Object = SaveComponent->GetGameObject();
		bool Ret = Object->SaveOnly(SaveComponent, FullPath);

		if (!Ret)
		{
			MessageBox(nullptr, TEXT("������Ʈ ���� ����"), TEXT("Error"), MB_OK);
			return;
		}

		MessageBox(nullptr, TEXT("������Ʈ ���� ����"), TEXT("Success"), MB_OK);
	}
}

void CSceneComponentHierarchyWindow::OnLoadComponent()
{
	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (!Window)
	{
		return;
	}

	CGameObject* SelectObject = Window->GetSelectGameObject();

	if (!SelectObject)
	{
		return;
	}

	TCHAR   FilePath[MAX_PATH] = {};

	OPENFILENAME    OpenFile = {};

	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
	OpenFile.lpstrFilter = TEXT("All File\0*.*\0");
	OpenFile.lpstrFile = FilePath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(COMPONENT_PATH)->Path;

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char FullPath[MAX_PATH] = {};

		int Length = WideCharToMultiByte(CP_ACP, 0, FilePath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FilePath, -1, FullPath, Length, 0, 0);

		CComponent* LoadComp = nullptr;
		bool Ret = SelectObject->LoadOnly(FullPath, LoadComp);

		if (!Ret)
		{
			MessageBox(nullptr, TEXT("������Ʈ �ε� ����"), TEXT("Error"), MB_OK);
			return;
		}

		// Hierachy Update
		OnAddComponent(SelectObject, (CSceneComponent*)LoadComp);

		// Insepctor Update
		CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);

		if (Inspector)
		{
			if(LoadComp->GetComponentType() == Component_Type::SceneComponent)
				Inspector->OnCreateSceneComponent((CSceneComponent*)LoadComp);
			else
				Inspector->OnCreateObjectComponent((CObjectComponent*)LoadComp);
		}

		MessageBox(nullptr, TEXT("������Ʈ �ε� ����"), TEXT("Success"), MB_OK);
	}
}

CComponent* CSceneComponentHierarchyWindow::FindSelectComponent()
{
	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (!Window)
	{
		return nullptr;
	}

	std::string SelectObjName = Window->GetSelectNode()->GetName();

	CGameObject* Obj = CSceneManager::GetInst()->GetScene()->FindObject(SelectObjName);

	if (!Obj)
	{
		return nullptr;
	}

	CComponent* Comp = Obj->FindComponent(m_SelectNode->GetName());

	return Comp;
}

void CSceneComponentHierarchyWindow::OnAddComponent(class CGameObject* Object, CSceneComponent* Component)
{
	if (!Object)
	{
		return;
	}

	if (Object->GetRootComponent() == Component)
	{
		m_Root->AddChild(Component->GetName(), Component);
	}
	else
	{
		m_Root->GetNode(0)->AddChild(Component->GetName(), Component);
	}
}

void CSceneComponentHierarchyWindow::MakeHierachyRecursive(CSceneComponent* Parent, CIMGUITree* ParentTree)
{
	size_t Count = Parent->GetChildCount();

	// CallBack ����
	ParentTree->AddSelectCallback(this, &CSceneComponentHierarchyWindow::OnSetSelectNode);
	ParentTree->SetDragDropSourceCallback(this, &CSceneComponentHierarchyWindow::OnDragDropSrc);
	ParentTree->SetDragDropDestCallback(this, &CSceneComponentHierarchyWindow::OnDragDropDest);

	// Tree ����
	for (size_t i = 0; i < Count; ++i)
	{
		ParentTree->AddChild(Parent->GetChild((int)i)->GetName(), Parent->GetChild((int)i));
	}

	// ����� ����
	for (size_t i = 0; i < Count; ++i)
	{
		CSceneComponent* Child = Parent->GetChild((int)i);
		CIMGUITree* ChildTree = ParentTree->GetNode((int)i);
		MakeHierachyRecursive(Child, ChildTree);
	}
}

void CSceneComponentHierarchyWindow::DisableCurrent()
{
	// ���� Component Hierarchy Window�� ��µǰ� �ִ°͵��� ���(�ֻ����� �ִ� "Components" TreeNode�����ϰ�) Disable ó��
	size_t Count = m_Root->GetChildCount();

	for (size_t i = 0; i < Count; ++i)
	{
		m_Root->GetNode((int)i)->DisableAll();
	}
}
