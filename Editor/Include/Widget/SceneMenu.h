#pragma once

#include "IMGUIBeginMenu.h"

class CSceneMenu :
    public CIMGUIBeginMenu
{
public:
    CSceneMenu();
    virtual ~CSceneMenu();

public:
    virtual bool Init() override;

private:
    void OnToggleSceneMode();

private:
    class CIMGUIMenuItem* m_SceneModeToggle;
};

