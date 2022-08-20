#pragma once

#include "IMGUIWindow.h"

class CSceneComponentHierarchyWindow :
    public CIMGUIWindow
{
public:
	CSceneComponentHierarchyWindow();
	virtual ~CSceneComponentHierarchyWindow();

private:
	std::vector<class CIMGUITree*>	m_vecComponentTree;
	class CIMGUIButton*				m_ComponentCreatePopUpButton;
	class CSceneComponentCreateModal*	m_ComponentCreateModal;
	class CIMGUIButton*					m_ComponentDeleteButton;

	class CIMGUITree* m_Root;
	class CIMGUITree* m_SelectNode;
	class CIMGUITree* m_DragSrc;
	class CIMGUITree* m_DragDest;

public :
	class CSceneComponentCreateModal* GetSceneComponentCreateModal() const
	{
		return m_ComponentCreateModal;
	}

public:
	class CIMGUITree* GetRoot()	const;

	class CIMGUITree* GetDragSrc()	const;
	class CIMGUITree* GetDragDest()	const;

	void SetDragSrc(class CIMGUITree* DragSrc);
	void SetDragDest(class CIMGUITree* DragDest);

	void ClearExistingHierarchy();
public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	// ParentNode 인자가 nullptr이면 최상위 부모인 SceneComponents의 자식에서 찾아서 지워준다
	bool DeleteTreeNode(class CSceneComponent* Comp, class CIMGUITree* ParentNode = nullptr);
	bool AddTreeNode(class CSceneComponent* Comp, class CIMGUITree* ParentNode = nullptr);
	CIMGUITree* FindTreeNode(class CSceneComponent* Comp);

public:
	// Inspector에서 GameObject 이름 변경되었을 때 호출
	void OnRenameComponent(const std::string& NewName, const std::string& PrevName);
	// Component 생성 팝업창 띄워주는 콜백
	void OnCreateComponentPopUp();

public:
	void OnDragDropSrc(class CIMGUITree* SrcTree);
	void OnDragDropDest(class CIMGUITree* DestTree, const std::string& ParentName, const std::string& NewChildName);
	
	// Object Hierarchy Window에서 새로운 Object를 선택할때마다 SceneComponentWindow에 목록을 갱신 
	void OnUpdateSceneComponentWindow(class CGameObject* Object);

	void OnCreateComponent(bool IsRoot, class CSceneComponent* Component);
	void OnLoadGameObject(class CGameObject* Object);

	// Object Hierarchy Window에서 Object가 Delete되면 SceneComponentHierarchyWindow에 Component들 전부 제거
	void OnClearComponents(const std::string& RootComponentName);

	void OnSetSelectNode(CIMGUITree* Tree);
	void OnDeleteComponent();

	// Component Save&Load
	void OnSaveComponent();
	void OnLoadComponent();
	class CComponent* FindSelectComponent();

private:
	void OnAddComponent(class CGameObject* Object, class CSceneComponent* Component);
	void MakeHierachyRecursive(class CSceneComponent* Parent, CIMGUITree* ParentTree);
	void DisableCurrent();
};

