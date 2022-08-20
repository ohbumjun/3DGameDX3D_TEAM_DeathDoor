
#include "ObjectCreateModal.h"
#include "IMGUITextInput.h"
#include "IMGUIComboBox.h"
#include "IMGUIButton.h"
#include "IMGUISameLine.h"
#include "../EditorInfo.h"
#include "../EditorUtil.h"
#include "IMGUIDummy.h"
#include "Scene/SceneManager.h"
#include "IMGUIManager.h"
#include "ObjectHierarchyWindow.h"
#include "SceneComponentHierarchyWindow.h"
#include "ObjectComponentWindow.h"
#include "SceneComponentCreateModal.h"
#include "IMGUITree.h"
#include "../Object/PlayerHook.h"
#include "IMGUIRadioButton.h"
#include "IMGUICheckBox.h"
#include "../EditorUtil.h"

CObjectCreateModal::CObjectCreateModal()	:
	m_NameTextInput(nullptr),
	m_ObjectCreateButton(nullptr)
{
}

CObjectCreateModal::~CObjectCreateModal()
{
}

bool CObjectCreateModal::Init()
{
	CIMGUIPopUpModal::Init();

	//m_ObjectCreatePopUp = AddWidget<CIMGUIPopUpModal>("CreateObjectPopUp");

	//m_ObjectTypeCombo = AddWidget<CIMGUIComboBox>("Object Type");
	//m_ObjectTypeCombo->SetHideName(true);
	//m_ObjectTypeCombo->AddItem("GameObject");
	//m_ObjectTypeCombo->AddItem("PlayerHook");

	m_NameTextInput = AddWidget<CIMGUITextInput>("Object Name");
	m_NameTextInput->SetHideName(true);
	m_NameTextInput->SetHintText("Object Name");

	CIMGUISameLine* Line = AddWidget<CIMGUISameLine>("Line");

	m_ObjectCreateButton = AddWidget<CIMGUIButton>("Create Button", 100.f, 20.f);
	m_ObjectCreateButton->SetClickCallback<CObjectCreateModal>(this, &CObjectCreateModal::OnCreateObject);

	m_ObjectTypeCheckBox = AddWidget<CIMGUIRadioButton>("Object Type");
	m_ObjectTypeCheckBox->AddCheckInfo("Player");
	m_ObjectTypeCheckBox->AddCheckInfo("Monster");
	m_ObjectTypeCheckBox->AddCheckInfo("MapObject");
	m_ObjectTypeCheckBox->AddCheckInfo("Particle");
	m_ObjectTypeCheckBox->AddCheckInfo("Projectile");
	m_ObjectTypeCheckBox->AddCheckInfo("None");

	m_ObjectTypeCheckBox->SetCheck(0, true);
	m_ObjectTypeCheckBox->AlwaysCheck(true);

	Line = AddWidget<CIMGUISameLine>("Line");

	m_EnemyCheckBox = AddWidget<CIMGUICheckBox>("Enemy Check");
	m_EnemyCheckBox->AddCheckInfo("Enemy");

	CIMGUIDummy* Dummy = AddWidget<CIMGUIDummy>("Dummy", 100.f, 40.f);

	return true;
}

void CObjectCreateModal::Update(float DeltaTime)
{
	CIMGUIPopUpModal::Render();
}

std::string CObjectCreateModal::GetObjectNameInput() const
{
	std::string Name = m_NameTextInput->GetTextMultibyte();

	return Name;
}

void CObjectCreateModal::OnCreateObject()
{
	CScene* CurrentScene = CSceneManager::GetInst()->GetScene();

	if (!CurrentScene)
		return;

	CIMGUITree* NewObjectTree = nullptr;

	CGameObject* NewObject = nullptr;

	std::string Name;
	Name = m_NameTextInput->GetTextMultibyte();

	CScene* Scene = CSceneManager::GetInst()->GetScene();

	while (Scene)
	{
		bool Same = Scene->CheckSameName(Name);

		if (!Same)
			break;

		Name = CEditorUtil::ConcatNumOverlapName(Name);
	}


	NewObject = CurrentScene->CreateGameObject<CGameObject>(Name);


	// 차후, Loading 을 위해서 ObjectCombo Select Index 정보를 저장해준다.
	// NewObject->SetEditorObjectModalIndex(Index);
	int Idx = -1;
	int CheckIdx = m_ObjectTypeCheckBox->GetCheckItemIdx();

	// TODO : Object_Type 추가될 때 마다 추가
	switch (CheckIdx)
	{
	case 0:
		NewObject->SetObjectType(Object_Type::Player);
		break;
	case 1:
		NewObject->SetObjectType(Object_Type::Monster);
		break;
	case 2:
		NewObject->SetObjectType(Object_Type::MapObject);
		break;
	case 3:
		NewObject->SetObjectType(Object_Type::Particle);
		break;
	case 4:
		NewObject->SetObjectType(Object_Type::Projectile);
		break;
	case 5:
		NewObject->SetObjectType(Object_Type::None);
		break;
	}

	Idx = -1;
	bool IsEnemyCheck = m_EnemyCheckBox->GetCheck(0);

	if (IsEnemyCheck)
		NewObject->SetEnemy(true);

	CObjectHierarchyWindow* ObjHierachy = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (ObjHierachy)
	{
		ObjHierachy->OnCreateObject(NewObject);
	}
}

 //CGameObject* CObjectCreateModal::OnCreateObject(const char* FullPathMultibyte)
 //{
 //	CScene* CurrentScene = CSceneManager::GetInst()->GetScene();
 //
 //	if (!CurrentScene)
 //		return nullptr;
 //
 //	// 확장자 확인
 //	char	Ext[_MAX_EXT] = {};
 //	char FileName[MAX_PATH] = {};
 //	char FilePathMultibyte[MAX_PATH] = {};
 //
 //	strcpy_s(FilePathMultibyte, FullPathMultibyte);
 //
 //	_splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, FileName, MAX_PATH, Ext, _MAX_EXT);
 //
 //	_strupr_s(Ext);
 //
 //	if (!strcmp(Ext, ".GOBJ") == 0)
 //		return nullptr;
 //
 //
 //	CGameObject* LoadedObject = CSceneManager::GetInst()->GetScene()->LoadGameObject<CGameObject>();
 //
 //	if (!LoadedObject->Load(FullPathMultibyte))
 //		return nullptr;
 //
 //	CIMGUITree* NewObjectTree = nullptr;
 //
 //	CGameObject* NewObject = nullptr;
 //
 //	char Name[256] = {};
 //	strcpy_s(Name, LoadedObject->GetName().c_str());
 //
 //	CObjectHierarchyWindow* Window = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);
 //	CSceneComponentHierarchyWindow* SceneCompWindow = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
 //	CObjectComponentWindow* ObjCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
 //
 //	if (Window)
 //	{
 //		// 해당 Object 의 Root Component 로 Widget 구성
 //		CIMGUITree* NewNode = Window->GetRoot()->AddChild(Name);
 //		NewNode->AddSelectCallback<CObjectHierarchyWindow>(Window, &CObjectHierarchyWindow::OnSetSelectNode);
 //		NewNode->AddSelectCallback<CSceneComponentHierarchyWindow>(SceneCompWindow, &CSceneComponentHierarchyWindow::OnUpdateSceneComponetWindow);
 //		NewNode->AddSelectCallback<CObjectComponentWindow>(ObjCompWindow, &CObjectComponentWindow::OnUpdateObjectComponetWindow);
 //		NewNode->SetDragDropSourceCallback<CObjectHierarchyWindow>(Window, &CObjectHierarchyWindow::OnDragDropSrc);
 //		NewNode->SetDragDropDestCallback<CObjectHierarchyWindow>(Window, &CObjectHierarchyWindow::OnDragDropDest);
 //
 //		// 해당 노드를 Select Node 로 세팅한다.
 //		Window->OnSetSelectNode(NewNode);
 //
 //		// 해당 Object 의 Root Component 로 Widget 구성
 //		CSceneComponentHierarchyWindow* ComponentWindow = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
 //		ComponentWindow->GetSceneComponentCreateModal()->OnLoadComponentRecursive(LoadedObject);
 //	}
 //
 //	return LoadedObject;
 //}

void CObjectCreateModal::SetObjectType(int Index, bool Boolean)
{
}
