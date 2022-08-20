#pragma once

#include "GameInfo.h"

class CIMGUIWindow
{
public:
	CIMGUIWindow();
	virtual ~CIMGUIWindow();

protected:
	std::string	m_Name;
	bool	m_Open;
	int		m_WindowFlag;
	std::vector<class CIMGUIWidget*>	m_vecWidget;
	Vector2 m_WindowSize;
	Vector2 m_WindowStartPos;

protected:
	std::string	m_PopupTitle;
	bool		m_ModalPopup;
	std::vector<class CIMGUIWidget*>	m_vecPopupWidget;

protected:
	int m_WidgetID; // Window의 Widget마다 가지고 있는 고유 ID를 발급해주기 위한 변수

public:
	int GetWidgetID()	const
	{
		return m_WidgetID;
	}

	void IncreaseWidgetID()
	{
		++m_WidgetID;
	}

	const Vector2& GetWindowSize()	const
	{
		return m_WindowSize;
	}

	const Vector2& GetWindowStartPos()	const
	{
		return m_WindowStartPos;
	}

public:
	void SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	void AddWindowFlag(ImGuiWindowFlags_ Flag)
	{
		m_WindowFlag |= Flag;
	}

	void EnableModalPopup()
	{
		m_ModalPopup = true;
	}

	void SetPopupWindowTitle(const std::string& Title)
	{
		m_PopupTitle = Title;
	}

	bool IsOpen() const
	{
		return m_Open;
	}

public:
	void Open();
	void Close();
	void ClosePopup();
	class CIMGUIWidget* FindWidget(const std::string& Name);
	// 갖고 있는 Widget을 m_vecWidget에서 지워주기만하고 메모리 해제는 하지 않는 함수
	void EraseWidget(const std::string& Name);
	
	void DeleteWidget(const std::string& Name);

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	template <typename T>
	T* AddWidget(const std::string& Name, float Width = 100.f, float Height = 100.f)
	{
		T* Widget = new T;

		Widget->SetName(Name);
		Widget->SetOwner(this);
		Widget->SetSize(Width, Height);

		if (!Widget->Init())
		{
			SAFE_DELETE(Widget);
			return nullptr;
		}

		m_vecWidget.push_back(Widget);

		return Widget;
	}

	template <typename T>
	T* AddPopupWidget(const std::string& Name, float Width = 100.f, float Height = 100.f)
	{
		T* Widget = new T;

		Widget->SetName(Name);
		Widget->SetOwner(this);
		Widget->SetSize(Width, Height);

		if (!Widget->Init())
		{
			SAFE_DELETE(Widget);
			return nullptr;
		}

		m_vecPopupWidget.push_back(Widget);

		return Widget;
	}
};


