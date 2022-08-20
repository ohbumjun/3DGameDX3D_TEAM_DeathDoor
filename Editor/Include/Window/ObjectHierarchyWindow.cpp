
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
	// Object Tree의 RootNode나 Component Tree의 RootNode는 자식 TreeNode를 선택하기 전까지는 디폴트로 선택된 상태로 해놓는다
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

	// 매 프레임마다 Tree를 순회하면서 m_SelectNode에 지금 선택된 노드를 갱신해줌
	//FindSelectNode(m_Root);
}
void CObjectHierarchyWindow::OnRenameObject(const std::string& Name)
{
	m_SelectNode->SetName(Name);
}

void CObjectHierarchyWindow::OnCreateObject(CGameObject* Object)
{
	// Parent가 있는(다른 Object의 자식 Object) Object는 그 Object의 최상위 Object가 OncreateObject를 호출했을때
	// 자식들의 Tree노드까지 OnCreateObjectRecursive 함수로 하위로 다 넣어주므로 최상위 노드가 아닌 노드들은 이 함수를 바로 빠져 나간다
	if (Object->GetParentObject())
		return;

	std::string Name = Object->GetName();

	// 노드 생성
	CIMGUITree* NewNode = m_Root->AddChild(Name, Object);
	NewNode->SetData(Object);

	// 콜백 연결
	NewNode->AddSelectCallback(this, &CObjectHierarchyWindow::OnSetSelectNode);
	NewNode->SetDragDropSourceCallback(this, &CObjectHierarchyWindow::OnDragDropSrc);
	NewNode->SetDragDropDestCallback(this, &CObjectHierarchyWindow::OnDragDropDest);

	// 현재 선택 오브젝트로
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

	// 노드 생성
	CIMGUITree* NewNode = ParentTreeNode->AddChild(Name, Object);
	NewNode->SetData(Object);

	// 콜백 연결
	NewNode->AddSelectCallback(this, &CObjectHierarchyWindow::OnSetSelectNode);
	NewNode->SetDragDropSourceCallback(this, &CObjectHierarchyWindow::OnDragDropSrc);
	NewNode->SetDragDropDestCallback(this, &CObjectHierarchyWindow::OnDragDropDest);

	// 현재 선택 오브젝트로
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

	

	// GUI상에서 노드 제거
	if (Root)
	{
		CompWindow->OnClearComponents(Root->GetName());
	}


	// 엔진 상에서 Object 계층 구조 내에서 제거
	Obj->DeleteObj();

	m_SelectNode->Delete();

	// Inspector에서 제거
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

	// Window 갱신
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
				MessageBox(nullptr, TEXT("해당 이름의 소켓이 없습니다"), TEXT("Error"), MB_OK);
				return;
			}

			DestObj->AddChildObject(SrcObj, Socket->GetName());

			// 여기 안에서 CSceneComponent::AddChild를 호출해서 DestObj의 Comp의 Child Component로 SrcObj의 RootComponent를 넣을 것이므로
			// 일단 SccneComponentHierarychy Window GUI상에서도 지우고 새로운 부모(DestObj의 RootComponent)에 추가해주는 작업을 해줘야 한다
			CSceneComponentHierarchyWindow* Window = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);

			Window->DeleteTreeNode(SrcObj->GetRootComponent());
			CIMGUITree* DestSceneCompTreeNode = Window->FindTreeNode(DestObj->GetRootComponent());

			Window->AddTreeNode(SrcObj->GetRootComponent(), DestSceneCompTreeNode);

			MessageBox(nullptr, TEXT("소켓에 추가 성공"), TEXT("Warning"), MB_OK);
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

			// SceneComponent Hierarychy Window, ObjectComponent List GUI상에서도 추가해줘야한다
		}
	}
}

CGameObject* CObjectHierarchyWindow::GetSelectGameObject()
{
	return m_SelectObject;
}

