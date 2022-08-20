#pragma once

#include "IMGUIWidgetList.h"

class CGameObjectWidget :
    public CIMGUIWidgetList
{
public:
    CGameObjectWidget();
    virtual ~CGameObjectWidget();

public:
    virtual bool Init() override;

public:
    void ClearComponentWidget();
    void CreateSceneComponentWidget(class CSceneComponent* Com);
    void CreateObjectComponentWidget(class CObjectComponent* Com);
    bool FindSceneComponent(CSceneComponent* Com);
public:
    void DeleteSceneComponentWidget(class CSceneComponent* Com);

public:
    void SetGameObject(class CGameObject* Obj);

public:
    class CGameObject* GetGameObject() const
    {
        return m_Object;
    }

private:
    void OnClickRenameButton();
    void OnCheckEnableCheckBox(int Idx, bool Check);
    void OnCheckEnemyCheckBox(int Idx, bool Check);
    void OnSelectObjectType(int Idx, const char* label);

private:
    class CGameObject* m_Object;
    class CIMGUITextInput* m_NameInput;
    class CIMGUIButton* m_RenameButton;
    class CIMGUICheckBox* m_EnableCheckBox;
    class CIMGUICheckBox* m_EnemyCheckBox;
    class CIMGUIComboBox* m_ObjectTypeCombo;

    std::list<class CSceneComponentWidget*> m_SceneComponentWidgetList;
    std::list<class CObjectComponentWidget*> m_ObjectComponentWidgetList;
};

