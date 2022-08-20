#pragma once

#include "GameInfo.h"

class CIMGUIWidget
{
	friend class CIMGUIWindow;
	friend class CIMGUITree;

public:
	CIMGUIWidget();
	virtual ~CIMGUIWidget();

protected:
	std::string		m_Name;
	class CIMGUIWindow* m_Owner;
	ImVec2			m_Size;
	ImColor			m_Color;
	ImFont* m_Font;
	int				m_WidgetID; // Window내에서 Widget이 가지고 있는 고유한 ID값
	bool			m_Render; // Render 여부

public:
	void SetOwner(class CIMGUIWindow* Owner)
	{
		m_Owner = Owner;
	}

	void SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	std::string GetName()	const
	{
		return m_Name;
	}

	void SetHideName(bool Hide)
	{
		if (Hide)
			m_Name = "##" + m_Name;
	}

	void SetSize(const ImVec2& Size)
	{
		m_Size = Size;
	}

	void SetSize(const Vector2& Size)
	{
		m_Size.x = Size.x;
		m_Size.y = Size.y;
	}

	void SetSize(float x, float y)
	{
		m_Size.x = x;
		m_Size.y = y;
	}

	void SetColor(unsigned char r, unsigned char g, unsigned char b,
		unsigned char a = 255)
	{
		m_Color.Value.x = r / 255.f;
		m_Color.Value.y = g / 255.f;
		m_Color.Value.z = b / 255.f;
		m_Color.Value.w = a / 255.f;
	}

	void SetRender(bool Render)
	{
		m_Render = Render;
	}

	bool IsRender()	const
	{
		return m_Render;
	}

public:
	virtual bool Init();
	virtual void Render();
};


