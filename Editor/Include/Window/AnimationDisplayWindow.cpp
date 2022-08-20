// IMGUI
#include "IMGUITextInput.h"
#include "IMGUIInputFloat.h"
#include "IMGUIInputInt.h"
#include "IMGUIInputFloat3.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
#include "IMGUIChild.h"
#include "../EditorInfo.h"
#include "../EditorUtil.h"
#include "IMGUICheckBox.h"
#include "IMGUISameLine.h"
#include "IMGUISeperator.h"
#include "IMGUIImage.h"
#include "IMGUISliderFloat.h"
#include "../Window/ObjectHierarchyWindow.h"
#include "IMGUIManager.h"
#include "IMGUIColor3.h"
#include "IMGUIColor4.h"
#include "IMGUIInputFloat.h"
#include "IMGUIComboBox.h"
#include "IMGUIDummy.h"
#include "IMGUIProgressBar.h"
#include "IMGUITree.h"
// Window
#include "AnimationEditor.h"
#include "../EditorManager.h"
#include "../Window/ResourceDisplayWindow.h"
#include "AnimationDisplayWindow.h"
// Object
#include "../Object/3DParticleObject.h"
// Engine
#include "Component/ParticleComponent.h"
#include "Component/SpriteComponent.h"
#include "Component/Arm.h"
#include "Engine.h"
#include "PathManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Render/RenderManager.h"
#include "Resource/Particle/ParticleManager.h"
#include "Scene/SceneResource.h"
#include "GameObject/SkyObject.h"
#include "AnimationEditor.h"
#include "../EditorManager.h"
#include "PathManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Animation/AnimationSequenceInstance.h"
#include "../EditorUtil.h"
#include "Device.h"
#include "../Object/Anim3DObject.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "../AnimationInstanceConvertThread.h"

CAnimationDisplayWindow::CAnimationDisplayWindow()
{
}

CAnimationDisplayWindow::~CAnimationDisplayWindow()
{
}

bool CAnimationDisplayWindow::Init()
{
    m_AnimationRenderTarget = AddWidget<CIMGUIImage>("Render Target", 400.f, 400.f);
    m_AnimationRenderTarget->SetRenderTargetImage(true);
    m_AnimationRenderTarget->SetTexture(CRenderManager::GetInst()->GetAnimationRenderTarget());
    m_AnimationRenderTarget->SetBorderColor(10, 10, 255);
    m_AnimationRenderTarget->SetTableTitle("Render Target");

    m_AnimMeshScaleEdit = AddWidget<CIMGUIInputFloat3>("Relative Scale", 250.f);
    m_AnimMeshScaleEdit->SetCallBack<CAnimationDisplayWindow>(this, &CAnimationDisplayWindow::OnEditAnimRelativeScale);

    return true;
}

void CAnimationDisplayWindow::SetAnimMeshScaleToIMGUI(const Vector3& Scale)
{
    m_AnimMeshScaleEdit->SetVal(Scale);
}

void CAnimationDisplayWindow::OnEditAnimRelativeScale(const Vector3& RelativeScale)
{
    CAnim3DObject* Anim3DObject = CEditorManager::GetInst()->GetAnimationEditor()->GetAnim3DObjectFromAnimationEditor();

    if (Anim3DObject == nullptr)
        return;

    dynamic_cast<CAnimationMeshComponent*>(Anim3DObject->GetRootComponent())->SetRelativeScale(RelativeScale);
}

