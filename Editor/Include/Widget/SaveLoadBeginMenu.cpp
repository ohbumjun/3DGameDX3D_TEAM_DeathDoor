// Engine
#include "Engine.h"
#include "GameObject/GameObject.h"
#include "Component/StateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "../EditorUtil.h"
#include "../Window/ToolWindow.h"
#include "../Window/InspectorWindow.h"
// Window
#include "../Window/BehaviorTreeWindow.h"
#include "../Window/ObjectComponentWindow.h"
#include "../Window/SceneComponentHierarchyWindow.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "../Window/BehaviorTreeMenuBar.h"
#include "../Window/ResourceDisplayWindow.h"
#include "../Window/ObjectCreateModal.h"
#include "../Window/EffectEditor.h"
#include "../Window/SceneModeWindow.h"
#include "../EditorUtil.h"
#include "../EditorManager.h"
#include "../Object/3DParticleObject.h"
// IMGUI
#include "IMGUIManager.h"
#include "SaveLoadBeginMenu.h"
#include "IMGUIBeginMenu.h"
#include "IMGUIMenuItem.h"
#include "IMGUIPopUpModal.h"
#include "IMGUIManager.h"

CSaveLoadBeginMenu::CSaveLoadBeginMenu()
{
}

CSaveLoadBeginMenu::~CSaveLoadBeginMenu()
{
}

bool CSaveLoadBeginMenu::Init()
{
	if (!CIMGUIBeginMenu::Init())
		return false;

	// Scene
	m_SaveSceneMenu = AddMenuItem("Save Scene");
	m_SaveSceneMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnSaveSceneMenuCallback);

	m_LoadSceneMenu = AddMenuItem("Load Scene");
	m_LoadSceneMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnLoadSceneMenuCallback);

	// Object
	m_SaveObjectMenu = AddMenuItem("Save Object");
	m_SaveObjectMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnSaveObjectMenuCallback);

	m_LoadObjectMenu = AddMenuItem("Load Object");
	m_LoadObjectMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnLoadObjectMenuCallback);

	// Scene Component
	m_SaveSceneComponentMenu = AddMenuItem("Save SceneComponent");
	m_SaveSceneComponentMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnSaveSceneComponentMenuCallback);

	m_LoadSceneComponentMenu = AddMenuItem("Load SceneComponent");
	m_LoadSceneComponentMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnLoadSceneComponentMenuCallback);
	
	// Object Component
	m_SaveObjectComponentMenu = AddMenuItem("Save ObjectComponent");
	m_SaveObjectComponentMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnSaveObjectComponentMenuCallback);
	
	m_LoadObjectComponentMenu = AddMenuItem("Load ObjectComponent");
	m_LoadObjectComponentMenu->SetClickCallBack(this, &CSaveLoadBeginMenu::OnLoadObjectComponentMenuCallback);

	return true;
}

void CSaveLoadBeginMenu::Render()
{
	CIMGUIBeginMenu::Render();
}

void CSaveLoadBeginMenu::OnSaveSceneMenuCallback()
{
	TCHAR FileFullPath[MAX_PATH] = {};
	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(SCENE_PATH)->Path;
	OpenFile.lpstrFilter = TEXT("Scene File\0*.scn\0");
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();

	if (GetSaveFileName(&OpenFile) != 0)
	{
		char FilePathMultibyte[MAX_PATH] = {};
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		bool Success = CSceneManager::GetInst()->GetScene()->SaveFullPath(FilePathMultibyte);

		if (!Success)
		{
			MessageBox(nullptr, TEXT("씬 저장 실패"), TEXT("Error"), MB_OK);
			return;
		}
		
		MessageBox(nullptr, TEXT("씬 저장 성공"), TEXT("Success"), MB_OK);
	}
}

void CSaveLoadBeginMenu::OnLoadSceneMenuCallback()
{
	TCHAR FileFullPath[MAX_PATH] = {};
	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(SCENE_PATH)->Path;
	OpenFile.lpstrFilter = TEXT("Scene File\0*.scn\0");
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char FilePathMultibyte[MAX_PATH] = {};
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		bool ValidExt = CEditorUtil::CompareExt(FilePathMultibyte, ".scn");

		if (!ValidExt)
		{
			MessageBox(nullptr, TEXT("확장자명 다름"), TEXT("Error"), MB_OK);
			return;
		}

		bool Success = CSceneManager::GetInst()->LoadNewSceneFullPath(FilePathMultibyte, true);

		if (!Success)
		{
			MessageBox(nullptr, TEXT("씬 로드 실패"), TEXT("Error"), MB_OK);
			return;
		}

		// Scene 연관 윈도우 전부 초기화
		ClearSceneRelatedWindows();
		
		// Hierachy 갱신
		std::vector<CGameObject*> vecObj;
		CSceneManager::GetInst()->GetNextScene()->GetAllIncludeSaveObjectsPointer(vecObj);
		RefreshSceneRelatedWindow(vecObj);

		// Scene Mode Window 갱신
		CSceneModeWindow* SceneModeWindow = (CSceneModeWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENEMODE_WINDOW);
		CSceneMode* SceneMode = CSceneManager::GetInst()->GetNextScene()->GetSceneMode();
		SceneModeWindow->OnRefreshSceneModeWidget(SceneMode);

		// ToolWindow
		CToolWindow* ToolWindow = (CToolWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(TOOL);
		ToolWindow->SetPlayText(false);

		MessageBox(nullptr, TEXT("씬 로드 성공"), TEXT("Success"), MB_OK);

		// 모든 Scene 을 로드하고나서, Resource Display Window Texture, Material 정보 Update
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
		CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
	}
}

void CSaveLoadBeginMenu::OnSaveObjectMenuCallback()
{
	CObjectHierarchyWindow* ObjectWindow = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (ObjectWindow)
	{
		CGameObject* Object = ObjectWindow->GetSelectObject();

		if (!Object)
			return;

		TCHAR FileFullPath[MAX_PATH] = {};
		OPENFILENAME OpenFile = {};
		OpenFile.lStructSize = sizeof(OPENFILENAME);
		OpenFile.lpstrFile = FileFullPath;
		OpenFile.nMaxFile = MAX_PATH;
		OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(OBJECT_PATH)->Path;
		OpenFile.lpstrFilter = TEXT("모든파일\0*.*\0*.GameObject File\0*.gobj");
		OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();

		if (GetSaveFileName(&OpenFile) != 0)
		{
			char FilePathMultibyte[MAX_PATH] = {};
			int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
			WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);

			//Object->Save(FilePathMultibyte);
			Object->SaveHierarchy(FilePathMultibyte);
		}
	}
}

void CSaveLoadBeginMenu::OnLoadObjectMenuCallback()
{
	TCHAR FileFullPath[MAX_PATH] = {};
	OPENFILENAME OpenFile = {};
	OpenFile.lStructSize = sizeof(OPENFILENAME);
	OpenFile.lpstrFile = FileFullPath;
	OpenFile.nMaxFile = MAX_PATH;
	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(OBJECT_PATH)->Path;
	OpenFile.lpstrFilter = TEXT("모든파일\0*.*\0*.GameObject File\0*.gobj");
	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();

	if (GetOpenFileName(&OpenFile) != 0)
	{
		char FilePathMultibyte[MAX_PATH] = {};
		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);

		bool ValidExt = CEditorUtil::CompareExt(FilePathMultibyte, "gobj");

		if (!ValidExt)
		{
			MessageBox(nullptr, TEXT("확장자명 다름"), TEXT("Error"), MB_OK);
			return;
		}

		CScene* Scene = CSceneManager::GetInst()->GetScene();

		CGameObject* NewObject = Scene->CreateEmtpyObject();

		bool Success = NewObject->LoadHierarchy(FilePathMultibyte);

		std::string NewObjName = NewObject->GetName();
		std::string ChangeName;

		while (Scene->IsExistSameName(NewObjName))
		{
			ChangeName = CEditorUtil::ConcatNumOverlapName(NewObjName);
			NewObject->SetName(ChangeName);
			NewObjName = NewObject->GetName();
		}

		if (!Success)
		{
			MessageBox(nullptr, TEXT("로드 실패"), TEXT("Error"), MB_OK);
			return;
		}

		RefreshSceneRelatedWindow(NewObject);

		MessageBox(nullptr, TEXT("로드 성공"), TEXT("Success"), MB_OK);


		// Resource Display 에 있는 Texture, Material 정보 다시 Update 
		// 사실 아래 코드는 거의 쓸모 없다 => 어차피 각 Animation Mesh Component 등, Mesh 파일에서 불러온 Material 을 세팅할 때에는
		// 공유되는 Material 이 사용되는 것이 아니기 때문이다.
		// 따라서 Mesh Component 에서 각각이 별도로 사용하는 Material File 을 Load 한다고 하더라도
		// 그냥 Mesh Component 만 사용할 뿐, 별도로 Material Manager 에 저장하는 것이 아니기 때문이다.
		// (나중에 필요하면, Animation Mesh Component Load 과정에서 Material Manager, Texture Manager 에 추가해주기)
		// CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedTextureResources();
		// CEditorManager::GetInst()->GetResourceDisplayWindow()->RefreshLoadedMaterialResources();
	}
}

void CSaveLoadBeginMenu::OnSaveSceneComponentMenuCallback()
{
	CSceneComponentHierarchyWindow* SceneComponentWindow = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);

	if (SceneComponentWindow)
	{
		SceneComponentWindow->OnSaveComponent();
	}
}

void CSaveLoadBeginMenu::OnLoadSceneComponentMenuCallback()
{
	CSceneComponentHierarchyWindow* SceneComponentWindow = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);

	if (SceneComponentWindow)
	{
		SceneComponentWindow->OnLoadComponent();
	}
}

void CSaveLoadBeginMenu::OnSaveObjectComponentMenuCallback()
{
	CObjectComponentWindow* ObjList = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);

	if (ObjList)
	{
		ObjList->OnSaveComponent();
	}
}

void CSaveLoadBeginMenu::OnLoadObjectComponentMenuCallback()
{
	CObjectComponentWindow* ObjList = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);

	if (ObjList)
	{
		ObjList->OnLoadComponent();
	}
}

void CSaveLoadBeginMenu::ClearSceneRelatedWindows()
{
	CObjectHierarchyWindow* ObjHierachy = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);
	CSceneComponentHierarchyWindow* SceneCompHierachy = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
	CObjectComponentWindow* ObjCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
	CToolWindow* ToolWindow = (CToolWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(TOOL);
	CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);

	if (ObjHierachy)
	{
		ObjHierachy->Clear();
	}
	if (SceneCompHierachy)
	{
		SceneCompHierachy->ClearExistingHierarchy();
	}
	if (ObjCompWindow)
	{
		ObjCompWindow->Clear();
	}
	if (ToolWindow)
	{
		ToolWindow->SetGizmoObject(nullptr);
	}
	if (Inspector)
	{
		Inspector->OnDeleteGameObject();
	}
}

void CSaveLoadBeginMenu::RefreshSceneRelatedWindow(CGameObject* Object)
{
	// Window 갱신
	CObjectHierarchyWindow* ObjHierachy = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);
	CSceneComponentHierarchyWindow* SceneCompHierachy = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);
	CObjectComponentWindow* ObjCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
	CToolWindow* ToolWindow = (CToolWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(TOOL);
	CInspectorWindow* Inspector = (CInspectorWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(INSPECTOR);

	if (ObjHierachy)
	{
		ObjHierachy->OnCreateObject(Object);
	}
	if (SceneCompHierachy)
	{
		SceneCompHierachy->OnLoadGameObject(Object);
	}
	if (ObjCompWindow)
	{
		ObjCompWindow->OnRefreshObjectComponentList(Object);
	}
	if (ToolWindow)
	{
		ToolWindow->SetGizmoObject(Object);
	}
	if (Inspector)
	{
		Inspector->OnSelectGameObject(Object);
	}

}

void CSaveLoadBeginMenu::RefreshSceneRelatedWindow(const std::vector<CGameObject*>& vecObj)
{
	size_t Size = vecObj.size();

	for (size_t i = 0; i < Size; ++i)
	{
		RefreshSceneRelatedWindow(vecObj[i]);
	}

	// 렌더 관련 위젯 리프레쉬 
	CToolWindow* ToolWindow = (CToolWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(TOOL);
	ToolWindow->RefreshGlobalSceneDataWidget();

	// Effect Editor 관련 Refresh
	CEffectEditor* EffectEditor = (CEffectEditor*)CIMGUIManager::GetInst()->FindIMGUIWindow(PARTICLE_EDITOR);
	EffectEditor->SetGameObjectReady();
}
