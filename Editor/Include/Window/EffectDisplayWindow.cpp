// IMGUI
#include "IMGUITextInput.h"
#include "IMGUIInputFloat.h"
#include "IMGUIInputInt.h"
#include "IMGUIInputFloat3.h"
#include "IMGUIText.h"
#include "IMGUIButton.h"
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
#include "IMGUITree.h"
// Window
#include "EffectEditor.h"
#include "../EditorManager.h"
#include "../Window/ResourceDisplayWindow.h"
#include "EffectDisplayWindow.h"
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
#include "../EditorInfo.h"

CEffectDisplayWindow::CEffectDisplayWindow()
{
}

CEffectDisplayWindow::~CEffectDisplayWindow()
{
}

bool CEffectDisplayWindow::Init()
{
    // 해당 해상도 만큼 => 3DParticleObject 의 Camera Component가 해상도를 세팅하게 되어있다.
    // m_ParticleRenderTarget = AddWidget<CIMGUIImage>("Render Target", 600.f, 600.f);
    m_ParticleRenderTarget = AddWidget<CIMGUIImage>("Render Target", ParticleEditorWidth, ParticleEditorHeight);
    m_ParticleRenderTarget->SetRenderTargetImage(true);
    m_ParticleRenderTarget->SetTexture(CRenderManager::GetInst()->GetParticleEffectRenderTarget());
    // m_ParticleRenderTarget->SetTexture(CResourceManager::GetInst()->FindTexture("FinalScreen"));
    m_ParticleRenderTarget->SetBorderColor(10, 10, 255);
    m_ParticleRenderTarget->SetTableTitle("Render Target");

	return true;
}

void CEffectDisplayWindow::Update(float DeltaTime)
{
    CIMGUIWindow::Update(DeltaTime);

    // m_ParticleRenderTarget->SetTexture(CRenderManager::GetInst()->GetParticleEffectRenderTarget());
}
