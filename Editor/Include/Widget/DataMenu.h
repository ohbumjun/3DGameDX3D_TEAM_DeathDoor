#pragma once

#include "IMGUIBeginMenu.h"

class CDataMenu :
    public CIMGUIBeginMenu
{
public:
    CDataMenu();
    virtual ~CDataMenu();

public:
    virtual bool Init() override;

private:
    void OnToggleCollisionProfileToggle();
    void OnToggleInspectorToggle();
    void OnToggleOverAllObjectDataToggle();

private:
    class CIMGUIMenuItem* m_CollisionProfileToggle;
    class CIMGUIMenuItem* m_InspectorToggle;
    class CIMGUIMenuItem* m_OverAllObjectDataToggle;
};

