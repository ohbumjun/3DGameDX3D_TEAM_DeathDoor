#pragma once

#include "IMGUIPopUpModal.h"

class CObjectCreateModal :
    public CIMGUIPopUpModal
{
public:
	CObjectCreateModal();
	virtual ~CObjectCreateModal();

private:
	class CIMGUIComboBox* m_ObjectTypeCombo;
	class CIMGUITextInput* m_NameTextInput;
	class CIMGUIButton* m_ObjectCreateButton;
	class CIMGUIRadioButton* m_ObjectTypeCheckBox;
	class CIMGUICheckBox* m_EnemyCheckBox;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	std::string GetObjectNameInput()	const;

	// ������Ʈ ���� �˾�â������ Create��ư ������ ȣ��Ǵ� �ݹ�
	void OnCreateObject();

	class CGameObject* OnCreateObject(class CGameObject* Object);

	void SetObjectType(int Index, bool Boolean);
};

