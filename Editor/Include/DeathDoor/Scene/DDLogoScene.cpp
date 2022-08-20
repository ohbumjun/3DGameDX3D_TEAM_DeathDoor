#include "DDLogoScene.h"
#include "../UI/UIManager.h"
#include "Widget/Button.h"
#include "Render/RenderManager.h"
#include "../Thread/DDSceneLoadingThread.h"
#include "Scene/SceneManager.h"

CDDLogoScene::CDDLogoScene()
{
	SetTypeID<CDDLogoScene>();
}

CDDLogoScene::~CDDLogoScene()
{
	SAFE_DELETE(m_NextSceneLoadingThread);
}

bool CDDLogoScene::Init()
{
	CSceneMode::Init();

	return true;
}

void CDDLogoScene::Start()
{
	CSceneMode::Start();

	CUIManager::GetInst()->SetViewport(m_Scene->GetViewport());
	CUIManager::GetInst()->CreateLogoUI(this);

	m_NextSceneLoadingThread = new CDDSceneLoadingThread;
	m_NextSceneLoadingThread->Init();
	m_NextSceneLoadingThread->Load<CDDLogoScene>(this, "MapScene/BossBetty_Final/BossBetty_F2.scn", &CDDLogoScene::OnLoadEnd);

	m_Scene->Play();
}

void CDDLogoScene::SetStartCallBack(CButton* Button)
{
	m_StartButton = Button;

	Button->ButtonEnable(false);
	Button->SetClickCallback(this, &CDDLogoScene::OnClickStartButton);
}

void CDDLogoScene::OnClickStartButton()
{
	CRenderManager::GetInst()->SetFadeEndCallBack(this, &CDDLogoScene::OnFadeOutEnd);
	CRenderManager::GetInst()->StartFadeEffect(FadeEffecType::FADE_OUT, true);
}

void CDDLogoScene::OnLoadEnd()
{
	m_StartButton->ButtonEnable(true);
}

void CDDLogoScene::OnFadeOutEnd()
{
	// Scene Change
	CSceneManager::GetInst()->ChangeNextScene();
}
