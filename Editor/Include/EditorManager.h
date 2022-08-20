#pragma once

#include "GameInfo.h"

enum class EditMode
{
	Scene,
	Sprite,
	TileMap
};

class CEditorManager
{
private:
	EditMode	m_EditMode;
	class CDragObject* m_DragObj;
	class CFileBrowser* m_FileBrowser;
	class CFileBrowserTree* m_FileBrowserTree;
	class CObjectHierarchyWindow* m_ObjectHierarchyWindow;
	class CSceneComponentHierarchyWindow* m_ComponentHierarchyWindow;
	class CObjectComponentWindow* m_ObjectComponentWindow;

	class CAnimationEditor* m_AnimationEditor;
	class CAnimationDisplayWindow* m_AnimationDisplayWindow;

	class CEffectEditor* m_EffectEditor;
	class CEffectDisplayWindow* m_EffectDisplayWindow;

	class CInspectorWindow* m_InspectorWindow;
	class CToolWindow* m_ToolWindow;
	class CBehaviorTreeMenuBar* m_BehaviorTreeMenuBar;
	class CBaseMenuBar* m_BaseMenuBar;
	class CMaterialEditor* m_MaterialEditor;
	class CResourceDisplayWindow* m_ResourceDisplayWindow;
	class C3DCameraObject* m_CameraObject;
	class CFBXConvertWindow* m_FBXConvertWindow;
	class CCollisionProfileEditor* m_CollisionProfileEditor;
	class CSocketWindow* m_SocketWindow;
	class CSceneModeWindow* m_SceneModeWindow;

	bool				m_MousePush;
	float				m_CameraMoveSpeed;

	class CGameStateManager* m_StateManager;

public:
	bool GetLButtonPush()	const
	{
		return m_MousePush;
	}
	class CDragObject* GetDragObj()	const
	{
		return m_DragObj;
	}
	class CInspectorWindow* GetInspectorWindow() const
	{
		return m_InspectorWindow;;
	}
	class CAnimationDisplayWindow* GetAnimationDisplayWindow() const
	{
		return m_AnimationDisplayWindow;
	}
	class CFBXConvertWindow* GetFBXConvertWindow()	const
	{
		return m_FBXConvertWindow;
	}
	class CFileBrowser* GetFileBrowser()	const
	{
		return m_FileBrowser;
	}
	class CFileBrowserTree* GetFileBrowserTree()	const
	{
		return m_FileBrowserTree;
	}
	class CObjectHierarchyWindow* GetObjectHierarchyWindow()	const
	{
		return m_ObjectHierarchyWindow;
	}
	class CObjectComponentWindow* GetObjectComponentWindow()	const
	{
		return m_ObjectComponentWindow;
	}
	class CSceneComponentHierarchyWindow* GetComponentHierarchyWindow()	const
	{
		return m_ComponentHierarchyWindow;
	}
	class CEffectEditor* GetEffectEditor() const
	{
		return m_EffectEditor;
	}
	class CEffectDisplayWindow* GetEffectDisplayWindow() const
	{
		return m_EffectDisplayWindow;
	}
	class CMaterialEditor* GetMaterialEditor() const
	{
		return m_MaterialEditor;
	}
	class CResourceDisplayWindow* GetResourceDisplayWindow() const
	{
		return m_ResourceDisplayWindow;
	}
	class CSocketWindow* GetSocketWindow()	const
	{
		return m_SocketWindow;
	}
	class CAnimationEditor* GetAnimationEditor()	const
	{
		return m_AnimationEditor;
	}
	class CCollisionProfileEditor* GetCollisionProfileEditor()	const
	{
		return m_CollisionProfileEditor;
	}
	class C3DCameraObject* Get3DCameraObject()	const
	{
		return m_CameraObject;
	}
	class CToolWindow* GetToolWindow() const
	{
		return m_ToolWindow;
	}
	class CSceneModeWindow* GetSceneModeWindow() const
	{
		return m_SceneModeWindow;
	}
	EditMode GetEditMode()
	{
		return m_EditMode;
	}

	class CGameStateManager* GetStateManager()	const;

	void SetEditMode(EditMode Mode);

public:
	bool Init(HINSTANCE hInst);
	void CreateDefaultSceneMode();
	int Run();

public:
	void MouseLButtonDown(float DeltaTime);
	void MouseLButtonPush(float DeltaTime);
	void MouseLButtonUp(float DeltaTime);

	void KeyboardUp(float DeltaTime);
	void KeyboardDown(float DeltaTime);
	void KeyboardLeft(float DeltaTime);
	void KeyboardRight(float DeltaTime);

public:
	void CreateSceneMode(class CScene* Scene, size_t Type);
	class CGameObject* CreateObject(class CScene* Scene, size_t Type);
	class CComponent* CreateComponent(class CGameObject* Obj, size_t Type);
	void CreateAnimInstance(class CSpriteComponent* Sprite, size_t Type);
	

private:
	void CreateKey();
	void CreateEditorObjects();
	void SetEditorSceneCallBack();
	void LoadEditorResources();
	void CreateWindows();
	void CreateEditorCamera();


	DECLARE_SINGLE(CEditorManager)
};

