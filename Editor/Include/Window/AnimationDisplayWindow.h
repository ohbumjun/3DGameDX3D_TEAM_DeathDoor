#pragma once
#include "IMGUIWindow.h"

class CAnimationDisplayWindow :
    public CIMGUIWindow
{
    friend class CEditorManager;
public:
    CAnimationDisplayWindow();
    ~CAnimationDisplayWindow();
public:
    virtual bool Init() override;
	// Animation Render Target
private:
    class CIMGUIImage* m_AnimationRenderTarget;

    // Animation Mesh Scale 조정해주기
    class CIMGUIInputFloat3* m_AnimMeshScaleEdit;
public :
    void SetAnimMeshScaleToIMGUI(const Vector3& Scale);
private :
    void OnEditAnimRelativeScale(const Vector3& RelativeScale);
};

