#pragma once

#include "IMGUIWidget.h"

// 맨 위 툴바를 만드는 위젯
// BeginMenu를 하위 위젯으로 가지고 있음
// 이 위젯은 하나만 만들어야 함

class CIMGUIMainMenuBar :
    public CIMGUIWidget
{
public:
    CIMGUIMainMenuBar();
    virtual ~CIMGUIMainMenuBar();

public:
    virtual bool Init() override;
    virtual void Render() override;

public:
    template <typename T>
    T* AddWidget(const std::string& name, const float width = 100.f, const float height = 100.f)
    {
		T* widget = new T;

		widget->SetName(name);
		widget->SetOwner(m_Owner);
		widget->SetSize(width, height);

		if (!widget->Init())
		{
			SAFE_DELETE(widget);
			assert(false);
			return nullptr;
		}
        mVecChild.push_back(widget);
        return widget;
    }

protected:
    std::vector<class CIMGUIWidget*> mVecChild;
    size_t mSize;
};

