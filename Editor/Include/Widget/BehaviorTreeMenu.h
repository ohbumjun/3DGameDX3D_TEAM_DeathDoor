#pragma once
#include "IMGUIBeginMenu.h"

class CBehaviorTreeMenu :
    public CIMGUIBeginMenu
{
public:
    CBehaviorTreeMenu();
    virtual ~CBehaviorTreeMenu();
private :
    class CIMGUIMenuItem* m_OpenTreeEditorMenu;
private :
    class CBehaviorTreeWindow* m_TreeEditorWindow;
public:
    virtual bool Init() override;
    virtual void Render() override;
private:
    void OnOpenTreeEditorCallback();
};

