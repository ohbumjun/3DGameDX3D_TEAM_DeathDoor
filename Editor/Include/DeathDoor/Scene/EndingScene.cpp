
#include "EndingScene.h"
#include "Scene/Scene.h"
#include "Widget/Image.h"
#include "Render/RenderManager.h"

CEndingScene::CEndingScene()
{
	SetTypeID<CEndingScene>();
}

CEndingScene::~CEndingScene()
{
}

bool CEndingScene::Init()
{
	return true;
}

void CEndingScene::Start()
{
	if (m_Start)
		return;

	CRenderManager::GetInst()->StartFadeEffect(FadeEffecType::FADE_IN, false);

	m_Scene->GetResource()->SoundPlay("Ending");

	CWidgetWindow* Ending = m_Scene->GetViewport()->CreateWidgetWindow<CWidgetWindow>("EndingSceneWindow");
	Ending->SetPos(300.f, 300.f);

	CImage* EndingBack = Ending->CreateWidget<CImage>("EndingSceneBack");
	CImage* TheEndWidget = Ending->CreateWidget<CImage>("EndingSceneWidget");

	char FileName[MAX_PATH];
	strcpy_s(FileName, "UI/DeathDoorEnding.png");
	TCHAR FileNameTChar[MAX_PATH] = {};

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, FileName, strlen(FileName), FileNameTChar, MAX_PATH);

	m_Scene->GetResource()->LoadTexture("DeathDoorEnding", FileNameTChar);
	CTexture* EndingTexture = m_Scene->GetResource()->FindTexture("DeathDoorEnding");

	EndingBack->SetTexture(EndingTexture);
	EndingBack->SetSize(600.f, 300.f);
	EndingBack->SetPos(50.f, 50.f);

	char FileName2[MAX_PATH];
	strcpy_s(FileName2, "UI/TheEnd.jpg");
	TCHAR FileNameTChar2[MAX_PATH] = {};

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, FileName2, strlen(FileName2), FileNameTChar2, MAX_PATH);

	m_Scene->GetResource()->LoadTexture("TheEnd", FileNameTChar2);
	CTexture* TheEndTexture = m_Scene->GetResource()->FindTexture("TheEnd");

	TheEndWidget->SetTexture(TheEndTexture);
	TheEndWidget->SetSize(400.f, 200.f);
	TheEndWidget->SetPos(150.f, -100.f);

	m_Start = true;
}

bool CEndingScene::Save(FILE* File)
{
	return CRef::Save(File);
}

bool CEndingScene::Load(FILE* File)
{
	return CRef::Load(File);
}
