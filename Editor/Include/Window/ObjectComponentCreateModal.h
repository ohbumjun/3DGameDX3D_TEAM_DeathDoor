#pragma once

#include "IMGUIPopUpModal.h"

class CObjectComponentCreateModal :
    public CIMGUIPopUpModal
{
public:
	CObjectComponentCreateModal();
	virtual ~CObjectComponentCreateModal();

private:

	class CIMGUIComboBox* m_ComponentCombo;
	class CIMGUITextInput* m_NameTextInput;
	class CIMGUIButton* m_ComponentCreateButton;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	std::string GetComponentNameInput()		const;

	// Component ���� �˾�â������ Create��ư ������ ȣ��Ǵ� �ݹ�
	void OnCreateComponent();
};

