#pragma once

#include "IMGUIWindow.h"
#include "IMGUIListBox.h"

class CObjectComponentWindow :
    public CIMGUIWindow
{
public:
	CObjectComponentWindow();
	virtual ~CObjectComponentWindow();

private:
	class CIMGUIButton*					m_ComponentCreatePopUpButton;
	class CIMGUIButton*					m_ComponentDeleteButton;
	class CObjectComponentCreateModal*	m_ComponentCreateModal;
	class CIMGUIListBox*				m_ComponentListBox;
	int m_SelectIndex;
	std::string m_SelectLabel;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	// Component 생성 팝업창 띄워주는 콜백
	void OnCreateComponentPopUp();
	int OnCreateObjectComponent(const std::string& Name);
	void OnRenameComponent(const std::string& NewName, const std::string& PrevName);
	void OnDeleteComponent();
	void OnRefreshObjectComponentList(class CGameObject* Object);
	void OnSelectComponent(int Index, const char* Label);
	void Clear();

	// Component Save&Load
	void OnSaveComponent();
	void OnLoadComponent();

public:
	int GetSelectIndex()	const
	{
		return m_SelectIndex;
	}

	std::string GetSelectLabel()	const
	{
		return m_SelectLabel;
	}

	void ClearListBox()
	{
		m_ComponentListBox->Clear();
	}
};

