#pragma once
#include "IMGUIBeginMenu.h"

class CResourceMenu :
    public CIMGUIBeginMenu
{
public:
    CResourceMenu();
    virtual ~CResourceMenu();
private :
    class CIMGUIMenuItem* m_ParticleEditorToggle;
    class CIMGUIMenuItem* m_AnimationEditorToggle;
    class CIMGUIMenuItem* m_MaterialEditorToggle;
    class CIMGUIMenuItem* m_ResourceDisplayToggle;
    class CIMGUIMenuItem* m_SocketDisplayToggle;

public:
    virtual bool Init() override;
    virtual void Render() override;
private:
    void OnToggleParticleEditorCallback();
    void OnToggleAnimationEditorCallback();
    void OnToggleMaterialEditorCallback();
    void OnToggleResourceDisplayEditorCallback();
    void OnToggleSocketDisplayEditorCallback();
};

