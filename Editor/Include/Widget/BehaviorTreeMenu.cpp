#include "BehaviorTreeMenu.h"
#include "IMGUIMenuItem.h"
#include "../Window/ObjectCreateModal.h"
#include "IMGUIBeginMenu.h"
#include "../EditorUtil.h"
#include "../EditorManager.h"
#include "IMGUIPopUpModal.h"
#include "Engine.h"
#include "IMGUIManager.h"
#include "../Window/BehaviorTreeWindow.h"
#include "../Window/ObjectComponentWindow.h"
#include "../Window/SceneComponentHierarchyWindow.h"
#include "IMGUIManager.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "GameObject/GameObject.h"
#include "../DeathDoor/Component/GameStateComponent.h"
#include "Component/AnimationMeshComponent.h"
#include "Component/StaticMeshComponent.h"
#include "PathManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

CBehaviorTreeMenu::CBehaviorTreeMenu()
{
}

CBehaviorTreeMenu::~CBehaviorTreeMenu()
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

bool CBehaviorTreeMenu::Init()
{
	// Open Tree Editor --> 실제 Menu 상에서 보여지는 이름
	m_OpenTreeEditorMenu = AddMenuItem("Open Tree Editor");
	m_OpenTreeEditorMenu->SetClickCallBack(this, &CBehaviorTreeMenu::OnOpenTreeEditorCallback);

	return true;
}

void CBehaviorTreeMenu::Render()
{
	CIMGUIBeginMenu::Render();
}

void CBehaviorTreeMenu::OnOpenTreeEditorCallback()
{
	CObjectComponentWindow* ComponentWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
	CObjectHierarchyWindow* ObjectWindow = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);

	if (ComponentWindow && ObjectWindow)
	{
		CGameObject* Obj = ObjectWindow->GetSelectObject();

		if (Obj)
		{
			CComponent* Component = Obj->FindObjectComponentFromType<CGameStateComponent>();
			CAnimationMeshComponent* AnimMeshComp = Obj->FindComponentFromType<CAnimationMeshComponent>();
			CStaticMeshComponent* StaticMeshComp = Obj->FindComponentFromType<CStaticMeshComponent>();


			// Object Component Window에서 StateComponent가 선택됐고 AnimationMeshComponent가 있거나 StaticMeshComponent가 있을때만 Behavior Tree Window 열어주기
			if (Component && (AnimMeshComp || StaticMeshComp))
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
					bool NeedUpdate = false;
					CStateComponent* StateComp = (CStateComponent*)Component;

					if (StateComp != m_TreeEditorWindow->GetStateComponent())
					{
						NeedUpdate = true;
						m_TreeEditorWindow->Clear();

						m_TreeEditorWindow->SetStateComponent(StateComp);

						if (AnimMeshComp)
							((CStateComponent*)Component)->SetAnimationMeshComponent(AnimMeshComp);

						m_TreeEditorWindow->UpdateLoadNode((CCompositeNode*)(StateComp->GetBehaviorTree()->GetRootNode()));
					}

					m_TreeEditorWindow->Open();
				}
			}
		}
	}
}
