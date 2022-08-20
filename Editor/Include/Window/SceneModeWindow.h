#pragma once

#include "IMGUIWindow.h"

class CSceneModeWindow :
    public CIMGUIWindow
{
public:
    CSceneModeWindow();
    virtual ~CSceneModeWindow();

public:
    virtual bool Init() override;

public:
    void OnRefreshSceneModeWidget(class CSceneMode* Mode);

private:
    void OnSelectSceneType(int Idx, const char* Label);
    void CreateModeWidget(class CSceneMode* Mode);

private:
    class CIMGUIComboBox* m_DDSceneTypeCombo;
    class CDDSceneModeWidget* m_SceneModeWidget;
};

