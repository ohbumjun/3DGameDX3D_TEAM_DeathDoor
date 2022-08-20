
#include "UIManager.h"
#include "Widget/WidgetWindow.h"
#include "Widget/Image.h"
#include "Widget/Button.h"
#include "Resource/ResourceManager.h"
#include "PathManager.h"
#include "Scene/SceneManager.h"
#include "../Scene/DDSceneMode.h"
#include "../Scene/DDLogoScene.h"
#include "../UI/DDMouseWidgetWindow.h"
#include "../Component/PlayerDataComponent.h"

#include <sstream>

DEFINITION_SINGLE(CUIManager)

CUIManager::CUIManager()	:
	m_Viewport(nullptr),
	m_AbilityIconOriginSize(Vector2(50.f, 50.f)),
	m_AbilityBoxOriginSize(Vector2(85.f, 85.f)),
	m_Window(nullptr)
{
}

CUIManager::~CUIManager()
{

}

void CUIManager::Init()
{
	CSceneManager::GetInst()->SetIsKeepUIFunction(this, &CUIManager::OnSceneChangeKeepUI);
}

void CUIManager::ActivateAbility(Player_Ability Ability)
{
	if (!m_Window)
	{
		return;
	}

	switch (Ability)
	{
	case Player_Ability::Arrow:
	{
		CImage* ArrowIcon = m_Window->FindWidget<CImage>("ArrowIcon");
		CImage* ArrowBox = m_Window->FindWidget<CImage>("Arrow_abilty_box");
		ArrowIcon->SetSize(m_AbilityIconOriginSize * 1.1f);
		ArrowBox->SetSize(m_AbilityBoxOriginSize * 1.1f);

		CImage* BombIcon = m_Window->FindWidget<CImage>("BombIcon");
		CImage* HookIcon = m_Window->FindWidget<CImage>("HookIcon");
		CImage* BombBox = m_Window->FindWidget<CImage>("Bomb_abilty_box");
		CImage* HookBox = m_Window->FindWidget<CImage>("Hook_abilty_box");
		BombIcon->SetSize(m_AbilityIconOriginSize);
		HookIcon->SetSize(m_AbilityIconOriginSize);
		BombBox->SetSize(m_AbilityBoxOriginSize);
		HookBox->SetSize(m_AbilityBoxOriginSize);
		break;
	}
	case Player_Ability::Hook:
	{
		CImage* HookIcon = m_Window->FindWidget<CImage>("HookIcon");
		CImage* HookBox = m_Window->FindWidget<CImage>("Hook_abilty_box");
		HookIcon->SetSize(m_AbilityIconOriginSize * 1.1f);
		HookBox->SetSize(m_AbilityBoxOriginSize * 1.1f);

		CImage* BombIcon = m_Window->FindWidget<CImage>("BombIcon");
		CImage* ArrowIcon = m_Window->FindWidget<CImage>("ArrowIcon");
		CImage* BombBox = m_Window->FindWidget<CImage>("Bomb_abilty_box");
		CImage* ArrowBox = m_Window->FindWidget<CImage>("Arrow_abilty_box");
		BombIcon->SetSize(m_AbilityIconOriginSize);
		ArrowIcon->SetSize(m_AbilityIconOriginSize);
		BombBox->SetSize(m_AbilityBoxOriginSize);
		ArrowBox->SetSize(m_AbilityBoxOriginSize);
		break;
	}
	case Player_Ability::Bomb:
	{
		CImage* BombIcon = m_Window->FindWidget<CImage>("BombIcon");
		BombIcon->SetSize(m_AbilityIconOriginSize * 1.1f);

		CImage* HookIcon = m_Window->FindWidget<CImage>("HookIcon");
		CImage* ArrowIcon = m_Window->FindWidget<CImage>("ArrowIcon");
		CImage* HookBox = m_Window->FindWidget<CImage>("Hook_abilty_box");
		CImage* ArrowBox = m_Window->FindWidget<CImage>("Arrow_abilty_box");
		HookIcon->SetSize(m_AbilityIconOriginSize);
		ArrowIcon->SetSize(m_AbilityIconOriginSize);
		HookBox->SetSize(m_AbilityBoxOriginSize);
		ArrowBox->SetSize(m_AbilityBoxOriginSize);
		break;
	}
	}
}

bool CUIManager::OnSceneChangeKeepUI(CScene* CurScene, CScene* NextScene)
{
	CDDSceneMode* CurSceneMode = dynamic_cast<CDDSceneMode*>(CurScene->GetSceneMode());
	CDDSceneMode* NextSceneMode = dynamic_cast<CDDSceneMode*>(NextScene->GetSceneMode());

	// 현재 씬과 다음 씬이 하나라도 DDSceneMode가 아니라면
	if (!CurSceneMode || !NextSceneMode)
	{
		return false;
	}

	return true;
}

void CUIManager::CreateLogoUI(class CDDLogoScene* LogoScene)
{
	m_LogoWindow = m_Viewport->CreateWidgetWindow<CWidgetWindow>("LogoWindow");
	m_LogoWindow->SetPos(0.f, 0.f);
	m_LogoWindow->SetSize(1280.f, 720.f);

	CImage* Widget = m_LogoWindow->CreateWidget<CImage>("BackGround");
	Widget->SetTexture("BackGroundTex", TEXT("UI/DDBackGround.jpg"));
	Widget->SetPos(0.f, 0.f);
	Widget->SetSize(1280.f, 720.f);
	Widget->SetZOrder(0);
	Widget->SetCollsionMouseEnable(false);

	CButton* Button = m_LogoWindow->CreateWidget<CButton>("StartButton");
	Button->SetTexture(Button_State::Normal, "StartButton", TEXT("UI/Start.png"));
	Button->SetTexture(Button_State::Click, "StartButton", TEXT("UI/Start.png"));
	Button->SetTexture(Button_State::MouseOn, "StartButton", TEXT("UI/Start.png"));
	Button->SetPos(512.f, 100.f);
	Button->SetSize(256.f, 128.f);
	Button->ButtonEnable(true);
	Button->SetZOrder(1);
	LogoScene->SetStartCallBack(Button);
}

void CUIManager::DecreaseHP()
{
	CGameObject* Player = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

	if (!Player)
		return;

	CPlayerDataComponent* Comp = Player->FindObjectComponentFromType<CPlayerDataComponent>();

	int HP = Comp->GetHP();

	if (HP == 0)
		return;

	if (HP == 5)
	{
		return;
	}

	//CTexture* EmptyHPBox = CResourceManager::GetInst()->FindTexture("HPBoxWrapper");

	CImage* CurrentHPBox = FindCurrentHPBox(HP);

	CurrentHPBox->SetOpacity(0.f);
}

void CUIManager::IncreaseHP()
{
	CGameObject* Player = CSceneManager::GetInst()->GetScene()->GetPlayerObject();

	if (!Player)
		return;

	CPlayerDataComponent* Comp = Player->FindObjectComponentFromType<CPlayerDataComponent>();

	int HP = Comp->GetHP();

	if (HP > 4 || HP < 0)
		return;

	//CTexture* HPBoxTexture = CResourceManager::GetInst()->FindTexture("HPBoxValid1");

	CImage* CurrentHPBox = FindCurrentHPBox(HP);

	//CurrentHPBox->SetTexture(HPBoxTexture);
	//CurrentHPBox->SetUseTexture(true);
	CurrentHPBox->SetOpacity(1.f);
}

CImage* CUIManager::FindCurrentHPBox(int Index)
{
	std::string BoxName = "HPBoxValid";

	switch (Index)
	{
	case 0:
		BoxName += "1";
		break;
	case 1:
		BoxName += "2";
		break;
	case 2:
		BoxName += "3";
		break;
	case 3:
		BoxName += "4";
		break;
	case 4:
		BoxName += "5";
		break;
	}

	size_t Count = m_vecHPBox.size();

	for (size_t i = 0; i < Count; ++i)
	{
		if (m_vecHPBox[i]->GetName() == BoxName)
			return m_vecHPBox[i];
	}

	return nullptr;
}

void CUIManager::CreateDeathDoorUI()
{
	CResourceManager::GetInst()->LoadCSV("UI.csv");

	CExcelData* Excel = CResourceManager::GetInst()->FindCSV("DeathDoor_UI");

	const Table& table = Excel->GetTable();
	size_t LabelCount = Excel->GetLabels().size();
	size_t Index = 0;

	auto iter = table.begin();
	auto iterEnd = table.end();

	m_Window = m_Viewport->CreateWidgetWindow<CWidgetWindow>("HUD");
	m_Window->SetPos(0.f, 0.f);

	for (; iter != iterEnd; ++iter)
	{
		Row* row = iter->second;
		std::stringstream ss;
		float PosX = 0;
		float PosY = 0;
		float SizeX = 0;
		float SizeY = 0;
		int ZOrder = 0;
		std::string Path;
		std::string Key = iter->first;

		for (size_t i = 0; i < LabelCount; ++i)
		{
			ss.str((*row)[i]);

			switch (i)
			{
			case 0:
				ss >> PosX;
				while (ss.fail())
				{
					ss.clear();
					ss.str((*row)[i]);
				}

				ss >> PosX;
				break;
			case 1:
				ss >> PosY;
				while (ss.fail())
				{
					ss.clear();
					ss.str((*row)[i]);
				}

				ss >> PosY;
				break;
			case 2:
				ss >> SizeX;
				while (ss.fail())
				{
					ss.clear();
					ss.str((*row)[i]);
				}
				ss >> SizeX;
				break;
			case 3:
				ss >> SizeY;
				while (ss.fail())
				{
					ss.clear();
					ss.str((*row)[i]);
				}
				ss >> SizeY;
				break;
			case 4:
				ss >> Path;
				while (ss.fail())
				{
					ss.clear();
					ss.str((*row)[i]);
				}
				ss >> Path;
				break;
			case 5:
				ss >> ZOrder;
				while (ss.fail())
				{
					ss.clear();
					ss.str((*row)[i]);
				}
				ss >> ZOrder;
				break;
			}

			ss.str("");
		}

		char FileName[MAX_PATH];
		strcpy_s(FileName, Path.c_str());
		TCHAR FileNameTChar[MAX_PATH] = {};

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, FileName, strlen(FileName), FileNameTChar, MAX_PATH);

		CImage* Widget = m_Window->CreateWidget<CImage>(Key);
		Widget->SetTexture(Key, FileNameTChar);
		Widget->SetPos(PosX, PosY);
		Widget->SetSize(SizeX, SizeY);
		Widget->SetZOrder(ZOrder);

		if (Key.find("HPBoxValid") != std::string::npos)
		{
			m_vecHPBox.push_back(Widget);
		}

	} 

	CEngine::GetInst()->CreateMouse<CDDMouseWidgetWindow>(Mouse_State::Normal, "DeathDoorMouse");
}
