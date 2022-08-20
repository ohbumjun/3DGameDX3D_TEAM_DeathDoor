
#include "BehaviorTreeMenuBar.h"
#include "ObjectCreateModal.h"
#include "IMGUIBeginMenu.h"
#include "../EditorUtil.h"
#include "../EditorManager.h"
#include "IMGUIPopUpModal.h"
#include "Engine.h"
#include "IMGUIManager.h"
#include "BehaviorTreeWindow.h"
#include "ObjectComponentWindow.h"
#include "SceneComponentHierarchyWindow.h"
#include "IMGUIManager.h"
#include "ObjectHierarchyWindow.h"
#include "GameObject/GameObject.h"
#include "Component/StateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

CBehaviorTreeMenuBar::CBehaviorTreeMenuBar()	:
	m_TreeEditorWindow(nullptr)
{
}

CBehaviorTreeMenuBar::~CBehaviorTreeMenuBar()
{
	if (!m_TreeEditorWindow)
	{
		GraphEditorDelegate Delegate;

		size_t Count = Delegate.GetTemplateCount();

		for (size_t i = 0; i < Count; ++i)
		{
			GraphEditor::Template tmp = Delegate.GetTemplate(i);
			if (tmp.mInputNames)
			{
				SAFE_DELETE_ARRAY(tmp.mInputNames);
			}

			if (tmp.mOutputNames)
			{
				SAFE_DELETE_ARRAY(tmp.mOutputNames);
			}
		}

		Delegate.m_IsCleared = true;
	}
}

bool CBehaviorTreeMenuBar::Init()
{
	if (!CIMGUIWindow::Init())
		return false;


	return true;
}

void CBehaviorTreeMenuBar::Update(float DeltaTime)
{
	//CIMGUIWindow::Update(DeltaTime);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene"))
			{
				
			}
			if (ImGui::MenuItem("Load Scene"))
			{
				
			}
			if (ImGui::MenuItem("Save GameObject"))
			{
				// CObjectComponentWindow* ComponentWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
				CObjectHierarchyWindow* ObjectWindow = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

				if (ObjectWindow)
				{
					CGameObject* Obj = ObjectWindow->GetSelectObject();
					OnSaveGameObject(Obj);
				}
			}
			if (ImGui::MenuItem("Load GameObject"))
			{
				// CObjectComponentWindow* ComponentWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
				CObjectHierarchyWindow* ObjectWindow = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

				if (ObjectWindow)
				{
					OnLoadGameObject();
				}
			}
			if (ImGui::MenuItem("Save SceneComponent"))
			{
				CSceneComponentHierarchyWindow* SceneComponentWindow = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);

				if (SceneComponentWindow)
				{
					SceneComponentWindow->OnSaveComponent();
				}
			}
			if (ImGui::MenuItem("Load SceneComponent"))
			{
				CSceneComponentHierarchyWindow* SceneComponentWindow = (CSceneComponentHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(SCENECOMPONENT_HIERARCHY);

				if (SceneComponentWindow)
				{
					SceneComponentWindow->OnLoadComponent();
				}
			}
			if (ImGui::MenuItem("Save ObjectComponent"))
			{
				CObjectComponentWindow* ObjList = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);

				if (ObjList)
				{
					ObjList->OnSaveComponent();
				}
			}
			if (ImGui::MenuItem("Load ObjectComponent"))
			{
				CObjectComponentWindow* ObjList = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);

				if (ObjList)
				{
					ObjList->OnLoadComponent();
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("BehaviorTree"))
		{
			if (ImGui::MenuItem("Open Tree Editor"))
			{
				CObjectComponentWindow* ComponentWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
				CObjectHierarchyWindow* ObjectWindow = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

				if (ComponentWindow && ObjectWindow)
				{
					CGameObject* Obj = ObjectWindow->GetSelectObject();

					if (Obj)
					{
						CComponent* Component = Obj->FindObjectComponentFromType<CStateComponent>();
						CAnimationMeshComponent* AnimMeshComp = Obj->FindComponentFromType<CAnimationMeshComponent>();

						// Object Component Window에서 StateComponent가 선택됐을때만 Behavior Tree Window 열어주기
						if (Component && AnimMeshComp)
						{
							if (!m_TreeEditorWindow)
							{
								m_TreeEditorWindow = CIMGUIManager::GetInst()->AddWindow<CBehaviorTreeWindow>(BEHAVIORTREE_WINDOW);
							
								m_TreeEditorWindow->Open();
								m_TreeEditorWindow->SetStateComponent((CStateComponent*)Component);
								m_TreeEditorWindow->GetStateComponent()->SetTreeUpdate(false);
								((CStateComponent*)Component)->SetAnimationMeshComponent(AnimMeshComp);
							}

							else
							{
								m_TreeEditorWindow->Open();
								m_TreeEditorWindow->SetStateComponent((CStateComponent*)Component);

								if(AnimMeshComp)
									((CStateComponent*)Component)->SetAnimationMeshComponent(AnimMeshComp);
							}
						}
					}
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	//CEditorUtil::ShowDemo();
}

 //void CBehaviorTreeMenuBar::OnSaveGameObject(CGameObject* Object)
 //{
 //	if (!Object)
 //		return;
 //
 //	TCHAR FileFullPath[MAX_PATH] = {};
 //	OPENFILENAME OpenFile = {};
 //	OpenFile.lStructSize = sizeof(OPENFILENAME);
 //	OpenFile.lpstrFile = FileFullPath;
 //	OpenFile.nMaxFile = MAX_PATH;
 //	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(OBJECT_PATH)->Path;
 //	OpenFile.lpstrFilter = TEXT("모든파일\0*.*\0*.GameObject File\0*.gobj");
 //	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
 //
 //	if (GetSaveFileName(&OpenFile) != 0)
 //	{
 //		char FilePathMultibyte[MAX_PATH] = {};
 //		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, 0, 0, 0, 0);
 //		WideCharToMultiByte(CP_ACP, 0, FileFullPath, -1, FilePathMultibyte, ConvertLength, 0, 0);
 //
 //		Object->Save(FilePathMultibyte);
 //
 //		// GameEngine 폴더에 저장하기
 //		// std::string ExtraFolderName = ENGINE_RESOURCE_OBJECT_PATH;
 //		// 
 //		// const PathInfo* EngineSequenceFolder = CPathManager::GetInst()->FindPath(ExtraFolderName);
 //		// 
 //		// // 파일 이름을 뽑아낸다.
 //		// char SavedFileName[MAX_PATH] = {};
 //		// char SavedExt[_MAX_EXT] = {};
 //		// _splitpath_s(FilePathMultibyte, nullptr, 0, nullptr, 0, SavedFileName, MAX_PATH, SavedExt, _MAX_EXT);
 //		// 
 //		// // 최종 GameEngine 경로를 만든다.
 //		// char SavedGameEnginePath[MAX_PATH] = {};
 //		// strcpy_s(SavedGameEnginePath, EngineSequenceFolder->PathMultibyte);
 //		// strcat_s(SavedGameEnginePath, SavedFileName);
 //		// strcat_s(SavedGameEnginePath, SavedExt);
 //		// 
 //		// // 현재 저장되는 경로와 다르다면, GameEngine 쪽에도 저장한다.
 //		// if (strcmp(EngineSequenceFolder->PathMultibyte, FilePathMultibyte) != 0)
 //		// {
 //		// 	Object->Save(SavedGameEnginePath);
 //		// }
 //	}
 //}
 //
 //CGameObject* CBehaviorTreeMenuBar::OnLoadGameObject()
 //{
 //	TCHAR LoadFilePath[MAX_PATH] = {};
 //	
 //	OPENFILENAME OpenFile = {};
 //	OpenFile.lStructSize = sizeof(OPENFILENAME);
 //	OpenFile.lpstrFile = LoadFilePath;
 //	OpenFile.nMaxFile = MAX_PATH;
 //	OpenFile.hwndOwner = CEngine::GetInst()->GetWindowHandle();
 //	OpenFile.lpstrFilter = TEXT("모든파일\0*.*\0*.GameObject File\0*.gobj");
 //	OpenFile.lpstrInitialDir = CPathManager::GetInst()->FindPath(OBJECT_PATH)->Path;
 //	
 //	if (GetOpenFileName(&OpenFile) != 0)
 //	{
 //		char FilePathMultibyte[MAX_PATH] = {};
 //		int ConvertLength = WideCharToMultiByte(CP_ACP, 0, LoadFilePath, -1, 0, 0, 0, 0);
 //		WideCharToMultiByte(CP_ACP, 0, LoadFilePath, -1, FilePathMultibyte, ConvertLength, 0, 0);
 //
 //		CGameObject* LoadedObject = CEditorManager::GetInst()->GetObjectHierarchyWindow()->GetObjectCreateModal()->OnCreateObject(FilePathMultibyte);
 //
 //		if (!LoadedObject)
 //			return nullptr;
 //
 //		// Object Hierarchy GameObject 목록에 추가한다.
 //		//CEditorManager::GetInst()->GetObjectComponentWindow()->AddObjectComponent(LoadedObject->GetName());
 //
 //		std::vector<CObjectComponent*> vecObjComp;
 //		LoadedObject->GetAllObjectComponentsPointer(vecObjComp);
 //
 //		size_t Count = vecObjComp.size();
 //
 //		for (size_t i = 0; i < Count; ++i)
 //		{
 //			CEditorManager::GetInst()->GetObjectComponentWindow()->AddObjectComponent(vecObjComp[i]->GetName());
 //
 //			if (vecObjComp[i]->GetTypeID() == typeid(CNavAgent).hash_code())
 //			{
 //				LoadedObject->SetNavAgent((CNavAgent*)vecObjComp[i]);
 //				((CNavAgent*)vecObjComp[i])->SetUpdateComponent(LoadedObject->GetRootComponent());
 //			}
 //		}
 //
 //		return LoadedObject;
 //	}
 //
 //	return nullptr;
 //}
 //