#pragma once

#include "GameInfo.h"
#include "Scene/Viewport.h"

class CUIManager
{
private:
	CViewport* m_Viewport;
	Vector2 m_AbilityIconOriginSize;
	Vector2 m_AbilityBoxOriginSize;
	class CWidgetWindow* m_LogoWindow;
	class CWidgetWindow* m_Window;
	class CWidgetWindow* m_MouseWindow;
	std::vector<class CImage*> m_vecHPBox;

public:
	void Init();
	void ActivateAbility(Player_Ability Ability);
	void DecreaseHP();
	void IncreaseHP();
	class CImage* FindCurrentHPBox(int Index);
	bool OnSceneChangeKeepUI(class CScene* CurScene, class CScene* NextScene);

public:
	void CreateLogoUI(class CDDLogoScene* LogoScene);
	void CreateDeathDoorUI();

	template <typename T>
	T* CreateWidgetWindow(const std::string& Name)
	{
		return m_Viewport->CreateWidgetWindow<T>(Name);
	}

	template <typename T>
	T* FindWidgetWindow(const std::string& Name)
	{
		return m_Viewport->FindWidgetWindow<T>(Name);
	}

public:
	void SetViewport(CViewport* Viewport)
	{
		m_Viewport = Viewport;
	}

	DECLARE_SINGLE(CUIManager)
};

