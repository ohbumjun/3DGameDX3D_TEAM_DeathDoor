#pragma once

#include "IMGUIWidgetList.h"

class CDDSceneModeWidget :
    public CIMGUIWidgetList
{
public:
    CDDSceneModeWidget();
    virtual ~CDDSceneModeWidget();

public:
    virtual bool Init() override;
    virtual void SetSceneMode(class CDDSceneMode* Mode);

public:
    virtual void RefreshWidgets();

protected:
    void OnCheckIntialScene(int Idx, bool Check);
    void OnChangeEntryPoint(const Vector3& Val);
    void OnClickFindEntry();
    void OnClickFindExitPointObj();
    void OnClickSetNextScene();

private:
    class CGameObject* FindObj(const std::string& Name);

protected:
    class CDDSceneMode* m_SceneMode;

    class CIMGUICheckBox* m_InitialScene;

    class CIMGUIInputFloat3* m_EntryPoint;
    class CIMGUITextInput* m_EntryPointNameFind;
    class CIMGUIButton* m_FindEntryPoint;

    class CIMGUITextInput* m_ExitPointObj;
    class CIMGUITextInput* m_ExitPointObjNameFind;
    class CIMGUIButton* m_FindExitPointObj;

    class CIMGUITextInput* m_NextSceneFileName;
    class CIMGUIButton* m_NextSceneFileSet;
};

