#pragma once
#include "IMGUIWindow.h"

class CBaseMenuBar :
    public CIMGUIWindow
{
public:
	CBaseMenuBar();
	virtual ~CBaseMenuBar();

private :
	class CIMGUIBeginMenu* m_SaveLoadBeginMenu;
	class CIMGUIBeginMenu* m_ResourceBeginMenu;
	class CIMGUIBeginMenu* m_BehaviorTreeBeginMenu;
	class CIMGUIBeginMenu* m_UtilBeginMenu;
	class CIMGUIBeginMenu* m_DataBeginMenu;
	class CIMGUIBeginMenu* m_SceneBeginMenu;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

public:
	template<typename T>
	T* AddWidget(const std::string& name, const float width = 100.f, const float height = 100.f)
	{
		T* widget = new T;

		widget->SetName(name);
		widget->SetOwner(this);
		widget->SetSize(width, height);

		if (!widget->Init())
		{
			SAFE_DELETE(widget);
			assert(false);
			return nullptr;
		}

		m_vecBeginMenuChild.push_back(widget);

		return widget;
	}

protected:
	std::vector<class CIMGUIBeginMenu*> m_vecBeginMenuChild;
	size_t mSize;
};

