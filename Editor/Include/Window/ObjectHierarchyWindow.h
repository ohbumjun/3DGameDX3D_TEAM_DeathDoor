#pragma once

#include "IMGUIWindow.h"

class CObjectHierarchyWindow :
	public CIMGUIWindow
{
public:
	CObjectHierarchyWindow();
	virtual ~CObjectHierarchyWindow();

private:
	std::vector<class CIMGUITree*>	m_vecObjectTree;
	class CIMGUIButton* m_ObjectCreatePopUpButton;
	class CObjectCreateModal* m_ObjectCreateModal;
	class CIMGUIButton* m_ObjectDeleteButton;
	class CGameObject* m_SelectObject;
	class CIMGUICheckBox* m_AddAsSocket;
	class CIMGUITextInput* m_SocketName;


	class CIMGUITree* m_Root;
	class CIMGUITree* m_SelectNode;
	class CIMGUITree* m_DragSrc;
	class CIMGUITree* m_DragDest;

	class CGameObject* m_SocketParentObject;

public:
	class CIMGUITree* GetRoot()	const;
	class CGameObject* GetSelectObject()	const;

public :
	CObjectCreateModal* GetObjectCreateModal() const
	{
		return m_ObjectCreateModal;
	}

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	// Inspector���� GameObject �̸� ����Ǿ��� �� ȣ��
	void OnRenameObject(const std::string& Name);
	void OnCreateObject(class CGameObject* Object);
	void OnCreateObjectRecursive(class CGameObject* Object, class CIMGUITree* ParentTreeNode);

	void Clear();

	// ������Ʈ ���� �˾�â ����ִ� �ݹ�
	void OnCreateObjectPopUp();
	void OnDeleteObject();

	// 2��° ���ڿ� ���� ���õǾ� �ִ� ��带 m_SelectNode�� �־��ش� �Լ�
	// ������ �Ʒ��� Ž���ϹǷ� 1��° ���ڿ��� ��Ʈ ��带 �־������
	//void FindSelectNode(class CIMGUITree* RootNode);
	CIMGUITree* GetSelectNode()	const;
	void OnSetSelectNode(CIMGUITree* Tree);
	void OnDragDropSrc(class CIMGUITree* SrcTree);
	void OnDragDropDest(class CIMGUITree* DestTree, const std::string& ParentName, const std::string& NewChildName);


public:
	class CGameObject* GetSelectGameObject();
};


