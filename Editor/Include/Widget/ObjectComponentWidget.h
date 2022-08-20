#pragma once

#include "IMGUIWidgetList.h"

class CObjectComponentWidget :
    public CIMGUIWidgetList
{
public:
    CObjectComponentWidget();
    virtual ~CObjectComponentWidget();

public:
    virtual bool Init() override;

public:
    // 반드시 Init 이후 이 함수 호출해야 함
    virtual void SetObjectComponent(class CObjectComponent* Com);

public:
    void OnGameObjectEnable(bool Enable);

private:
    void OnClickRenameButton();
    void OnCheckEnableCheckBox(int Idx, bool Check);

protected:
    class CObjectComponent* m_Component;
    class CIMGUIText* m_ComponentTypeText;
    class CIMGUITextInput* m_NameInput;
    class CIMGUIButton* m_RenameButton;
    class CIMGUICheckBox* m_EnableCheckBox;
    std::string m_PrevName;
};

