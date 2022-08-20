#pragma once
#include "IMGUIWidget.h"

class CIMGUIImageButton :
    public CIMGUIWidget
{
public:
	CIMGUIImageButton();
	virtual ~CIMGUIImageButton();

protected:
	CSharedPtr<class CTexture>	m_Texture;
	ImVec4			m_BackgroundColor;
	ImVec4			m_TintColor;
	int				m_PressCount;

	std::function<void(const std::string&)> m_DoubleClickCallback;
	std::function<void(const std::string&)> m_DragSrcCallback;

public:
	void SetBackgroundColor(const ImVec4& Color)
	{
		m_BackgroundColor = Color;
	}

	void SetTintColor(const ImVec4& Color)
	{
		m_TintColor = Color;
	}

	int GetPressCount()	const
	{
		return m_PressCount;
	}

	bool SetTexture(class CTexture* Texture);
	bool SetTexture(const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
	bool SetTextureFullPath(const std::string& Name, const TCHAR* FullPath);

public:
	virtual bool Init();
	virtual void Render();
	
	template <typename T>
	void SetDoubleClickCallback(T* Obj, void(T::*Func)(const std::string&))
	{
		m_DoubleClickCallback = std::bind(Func, Obj, std::placeholders::_1);
	}

	template <typename T>
	void SetDragSrcCallback(T* Obj, void(T::* Func)(const std::string&))
	{
		m_DragSrcCallback = std::bind(Func, Obj, std::placeholders::_1);
	}
};

