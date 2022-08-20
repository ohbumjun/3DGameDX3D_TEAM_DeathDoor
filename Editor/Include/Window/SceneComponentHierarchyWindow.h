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
	// ParentNode ���ڰ� nullptr�̸� �ֻ��� �θ��� SceneComponents�� �ڽĿ��� ã�Ƽ� �����ش�
	bool DeleteTreeNode(class CSceneComponent* Comp, class CIMGUITree* ParentNode = nullptr);
	bool AddTreeNode(class CSceneComponent* Comp, class CIMGUITree* ParentNode = nullptr);
	CIMGUITree* FindTreeNode(class CSceneComponent* Comp);

public:
	// Inspector���� GameObject �̸� ����Ǿ��� �� ȣ��
	void OnRenameComponent(const std::string& NewName, const std::string& PrevName);
	// Component ���� �˾�â ����ִ� �ݹ�
	void OnCreateComponentPopUp();

public:
	void OnDragDropSrc(class CIMGUITree* SrcTree);
	void OnDragDropDest(class CIMGUITree* DestTree, const std::string& ParentName, const std::string& NewChildName);
	
	// Object Hierarchy Window���� ���ο� Object�� �����Ҷ����� SceneComponentWindow�� ����� ���� 
	void OnUpdateSceneComponentWindow(class CGameObject* Object);

	void OnCreateComponent(bool IsRoot, class CSceneComponent* Component);
	void OnLoadGameObject(class CGameObject* Object);

	// Object Hierarchy Window���� Object�� Delete�Ǹ� SceneComponentHierarchyWindow�� Component�� ���� ����
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

