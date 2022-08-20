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

	// Component 생성 팝업창내에서 Create버튼 누르면 호출되는 콜백
	void OnCreateComponent();
};

